/**
 * This software was developed at the National Institute of Standards and
 * Technology by employees of the Federal Government in the course of
 * their official duties. Pursuant to title 17 Section 105 of the United
 * States Code this software is not subject to copyright protection and
 * is in the public domain.
 * 
 * NIST assumes no responsibility whatsoever for its use by other parties,
 * and makes no guarantees, expressed or implied, about its quality,
 * reliability, or any other characteristic.
 * 
 * We would appreciate acknowledgment if the software is used.
 * 
 * NIST ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS" CONDITION AND
 * DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING
 * FROM THE USE OF THIS SOFTWARE.
 * 
 * 
 * This software might use libraries that are under GNU public license or
 * other licenses. Please refer to the licenses of all libraries required 
 * by this software.
 *
 * Provides functionality to handle the SRx server socket.
 *
  * @version 0.6.2.2
 *
 * Changelog:
 * -----------------------------------------------------------------------------
 *  0.6.2.2 - 2024/09/30 - oborchert
 *            * Added query for BRIO define if code is part of brio rather than
 *              srx-server.
 *            * Renamed single_packetHandler into single_SRxProxyPacketHandler.
 *            * Renamed single_handleClient into single_handleSRxProxyClient.
 *            * Rerformated dome version control entries.
 *  0.6.1.3 - 2024/06/12 - oborchert
 *            * Fixed linker error in 'ROCKY 9' regarding the variable 
 *              declaration int g_single_thread_client_fd which needs to be 
 *              declared in the .c file and listed here as extern.
 *  0.5.0.0 - 2017/06/16 - oborchert
 *            * Version 0.4.1.0 is trashed and moved to 0.5.0.0
 *          - 2016/10/26 - oborchert
 *            * BZ1037: Replaces legacy calls to bzero with memset
 *          - 2016/08/19 - oborchert
 *            * Moved socket connection error strings to the header file.
 *            * Modified connection error message for ports in system space
 *  0.3.0.10- 2013/01/25 - oborchert
 *            * Removed un-used include glib.h 
 *  0.3.0.0 - 2013/01/25 - oborchert
 *           * Removed error output if an attempt of removing an already removed
 *             client thread from the client list.
 *           * Removed dead code.
 *            * Re-formated some documentation and code.
 *          - 2013/01/04 - oborchert
 *            * Added parameter goodByeReceived to ClientThread structure.
 *          - 2012/12/13 - oborchert
 *            * //TODO Make SVN compare
 *  0.2.0.0 - 2011/01/07 - oborchert
 *            * Changelog added with version 0.2.0 and date 2011/01/07
 *            * Version tag added
 *  0.1.0.0 - 2009/12/23 - pgleichm
 *            * Code Created
 * -----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#ifdef BRIO
  #include "antd-util/log.h"
  #include "srx-common/mutex.h"
  #include "srx-common/slist.h"
  #include "srx-common/socket.h"
  #include "srx-common/server_socket.h"

  typedef void SRXPROXY_BasicHeader;
#else
  #include "util/log.h"
  #include "util/mutex.h"
  #include "util/packet.h"
  #include "util/slist.h"
  #include "util/socket.h"
  #include "util/server_socket.h"
#endif


#define HDR  "([0x%08X] Server Socket): "

/**
 * A single client thread.
 */

/** The file descriptor to the single thread. Moved from header file into c file.*/
int g_single_thread_client_fd=0;

/**
 * Sends data as a packet (length, data).
 *
 * USE sendNum directly
 *
 * @param fd Socket file-descriptor
 * @param data Data to send
 * @param len Size of the data
 */
static inline void sendData(int* fd, void* data, PacketLength len)
{
  if (!sendNum(fd, data, (size_t)len))
  {
    RAISE_ERROR("Data could not be send!");
  }
}

/**
 * NULL-safe frees memory.
 *
 * @param ptr Memory to free
 */
static inline void safeFree(void* ptr)
{
  if (ptr != NULL)
  {
    free(ptr);
  }
}

/**
 * Initializes the writeMutex inside ClientThread.
 * 
 * @param cthread Instance of the client thread
 * @return \c true = created, \c false = failed to create
 */
static bool initWriteMutex(ClientThread* cthread)
{
  if (!initMutex(&cthread->writeMutex))
  {
    RAISE_ERROR("Failed to create a write-mutex for a client connection");
    return false;
  }
  return true;
}

/**
 * Clean-up of a single ClientThread.
 *
 * @param mode Client mode
 * @param ct Instance
 */
static void clientThreadCleanup(ClientMode mode, ClientThread* ct)
{
  // Let the user know about the client loss
  if (ct->svrSock->statusCallback != NULL)
  {
    ct->svrSock->statusCallback(ct->svrSock,
                                (mode == MODE_SINGLE_CLIENT) ? ct : NULL,
                                ct->clientFD, false, ct->svrSock->user);
  }

  // Information
  if (ct->svrSock->verbose)
  {
    char buf[MAX_SOCKET_STRING_LEN];

    LOG(LEVEL_INFO, "Client disconnected: %s",
        socketToStr(ct->clientFD, true, buf, MAX_SOCKET_STRING_LEN));
  }

  // The instance can be reused
  releaseMutex(&ct->writeMutex);
  ct->active = false;
}

/*----------------------------
 * MODE_SINGLE_CLIENT routines
 */

/** 
 * Sends a single packet.
 *
 * @note MODE_SINGLE_CLIENT
 * 
 * @param client The client thread for this connection.
 * @param data   The packet to send
 * @param size   The size of the packet to send.
 */
static bool single_sendResult(ServerClient* client, void* data, size_t size)
{
  ClientThread* clt = (ClientThread*)client;
#ifdef USE_GRPC
  bool retVal = false;
#else
  bool retVal = true;
#endif

#ifdef USE_GRPC
  if(!clt->type_grpc_client)
  {
#endif
  // Only when still active
  if (clt->active)
  {
    lockMutex(&clt->writeMutex);
    sendData(&clt->clientFD, data, (PacketLength)size);
    unlockMutex(&clt->writeMutex);
#ifdef USE_GRPC
      retVal = true;
#endif // USE_GRPC
  }
  else
  {
    RAISE_ERROR("Trying to send a packet over an inactive connection");
    retVal = false;
  }
#ifdef USE_GRPC
  }
#endif // USE_GRPC
  return retVal;
}

/**
 * PacketReceived to ServerPacketReceived wrapper.
 * 
 * @param header the received data
 * @param clientThread the user ClientThread instance
 */
static void single_SRxProxyPacketHandler(SRXPROXY_BasicHeader* header,
                                         void* clientThread)
{
#ifdef BRIO
  RAISE_ERROR("Invalid call of single_SRxProxyPacketHandler from BRIO");
#else
  ClientThread* cthread = (ClientThread*)clientThread;
  cthread->svrSock->modeCallback(cthread->svrSock, cthread, header,  // handlePacket
                                 ntohl(header->length), cthread->svrSock->user);
#endif                              
}

/** 
 * Thread that handles the packets of a single client.
 *
 * @note MODE_SINGLE_CLIENT
 * @note PThread syntax
 *
 * @param data ClientThread instance
 */
#include <signal.h>

void sigusr_pipe_handler(int signo)
{
  LOG(LEVEL_DEBUG, "([0x%08X]) received SIGPIPE from broken socket --> keep alive ", pthread_self());
  //shutdown(g_single_thread_client_fd, SHUT_RDWR);
  close(g_single_thread_client_fd);
}


static void* single_handleSRxProxyClient(void* clientThread)
{
#ifdef BRIO
  RAISE_ERROR("Invalid call of single_handleSRxProxyClient within BRIO");
#else
  ClientThread* cthread = (ClientThread*)clientThread;

  struct sigaction act;
  sigset_t errmask;
  sigemptyset(&errmask);
  sigaddset(&errmask, SIGPIPE);
  act.sa_handler = sigusr_pipe_handler;
  sigaction(SIGPIPE, &act, NULL);
  pthread_sigmask(SIG_UNBLOCK, &errmask, NULL);
  g_single_thread_client_fd = cthread->clientFD;

  LOG(LEVEL_DEBUG, "([0x%08X]) > Proxy Client Connection Thread started "
                "(ServerSocket::single_handleSRxProxyClient)", pthread_self());
  LOG(LEVEL_DEBUG, HDR "Inside new client thread, about to start traffic "
                    "listener.", pthread_self());
  if (initWriteMutex(cthread))
  {
    // Start the receiver loop of this client connection.
    receiveSRxProxyPackets(&cthread->clientFD, single_SRxProxyPacketHandler, 
                           cthread, PHT_SERVER);
  }

  clientThreadCleanup(MODE_SINGLE_CLIENT, cthread);
  
  LOG(LEVEL_DEBUG, "([0x%08X]) > Proxy Client Connection Thread stopped "
                "(ServerSocket::single_handleSRxProxyClient)", pthread_self());
  
  pthread_exit(0);
#endif
}

/*----------------------
 * MODE_MULTIPLE_CLIENTS
 */

/**
 * A single packet thread.
 *
 * @note MODE_MULTIPLE_CLIENTS
 */
typedef struct
{
  // Data that never changes - just to pass it
  ClientThread* clThread;
  Mutex* readMutex;
  Mutex* writeMutex; // Unlike "readMutex" this is just a weak copy

  // "Pool" data
  pthread_t thread;
  bool active;
  void* buffer;
  size_t bufferSize;

  // Data that changes on every access
#pragma pack(1)

  struct
  {
    uint32_t id;
    PacketLength packetLen;
  } hdr;
#pragma pack(0)
} PacketThread;

/**
 * Sends the result back to the client.
 *
 * @note MODE_MULTIPLE_CLIENTS
 *
 * @param client Client
 * @param data Data (= result) to send
 * @param size Size of the \c data in Bytes
 */
static bool multi_sendResult(ServerClient* client, void* data, size_t size)
{
  PacketThread* pt = (PacketThread*)client;

  if (pt->clThread->active)
  {
    // Lock so that id and packet do not get separated
    lockMutex(pt->writeMutex);

    // Send the id and data
    if (sendNum(&pt->clThread->clientFD,
                &pt->hdr.id, sizeof (uint32_t)))
    {
      sendData(&pt->clThread->clientFD, data, (PacketLength)size);
    }

    // Now other threads can send too
    unlockMutex(pt->writeMutex);
    return true;
  }

  RAISE_ERROR("Invalid call - invoke inside the 'received' callback");
  return false;
}

/**
 * Receives a single packet, calls the callback and then waits until the
 * client-thread unblocks this call.
 *
 * @note MODE_MULTIPLE_CLIENTS
 * @note PThread syntax
 *
 * @param arg PacketThread instance
 * @return \c 0 = successful, \c 1+ = error
 */
static void* multi_handleSinglePacket(void* arg)
{
  PacketThread* pt = (PacketThread*)arg;
  bool succ;

  LOG (LEVEL_DEBUG, "([0x%08X]) > Server Socket (SinglePacketHandler) thread "
                    "started!", pthread_self());
  
  // Buffer not large enough - resize
  if (pt->bufferSize < pt->hdr.packetLen)
  {
    void* newBuf = realloc(pt->buffer, pt->hdr.packetLen);

    // Not enough memory
    if (newBuf == NULL)
    {
      if (pt->bufferSize > 0)
      {
        free(pt->buffer);
      }
      RAISE_SYS_ERROR("Not enough memory for the packet data");
      pthread_exit((void*)1);
    }

    pt->buffer = newBuf;
    pt->bufferSize = pt->hdr.packetLen;
  }

  // Receive the packet
  succ = recvNum(&pt->clThread->clientFD, pt->buffer, pt->hdr.packetLen);
  unlockMutex(pt->readMutex);

  // Let the user-callback process the packet 
  if (succ)
  {
    ((ServerPacketReceived)pt->clThread->svrSock->modeCallback)(
                                                  pt->clThread->svrSock, pt,
                                                  pt->buffer, pt->hdr.packetLen,
                                                  pt->clThread->svrSock->user);
  }

  pt->active = false;
  
  LOG (LEVEL_DEBUG, "([0x%08X]) < Server Socket (SinglePacketHandler) thread "
                    "stopped!", pthread_self());      
  
  pthread_exit(succ ? 0 : (void*)2);
}

/**
 * Thread that reads all packets and passes them to a callback.
 *
 * @note MODE_MULTIPLE_CLIENTS
 * @note PThread syntax
 *
 * @param data ClientThread instance
 * @return Always \c 0
 */
static void* multi_handleClient(void* data)
{
  RAISE_SYS_ERROR("LOOK1 ****************************************************");
  
  ClientThread* cthread = (ClientThread*)data;
  Mutex rmutex;
  SList packetThreads;
  SListNode* node;
  PacketThread* currPT;
  
  LOG(LEVEL_DEBUG, "([0x%08X]) > Proxy Client Connection Thread started "
                   "(ServerSocket::multi_handleClient)", pthread_self());
  
  // Write-mutex
  if (!initWriteMutex(cthread))
  {
    clientThreadCleanup(MODE_MULTIPLE_CLIENTS, cthread);
    
    LOG(LEVEL_DEBUG, "([0x%08X]) < Proxy Client Connection Thread stopped (1) "
                     " (ServerSocket::multi_handleClient)", pthread_self());
    
    pthread_exit((void*)1);
  }

  // Create the read-mutex
  if (!initMutex(&rmutex))
  {
    RAISE_ERROR("Failed to create a read mutex for the client thread");
    clientThreadCleanup(MODE_MULTIPLE_CLIENTS, cthread);

    LOG(LEVEL_DEBUG, "([0x%08X]) < Proxy Client Connection Thread stopped (2) "
                     "(ServerSocket::multi_handleClient)", pthread_self());
    
    pthread_exit((void*)2);
  }

  // A thread per command
  initSList(&packetThreads);

  // Process all packets
  for (;;)
  {
    // Look for an inactive node and recycle it

    FOREACH_SLIST(&packetThreads, node)
    {
      if (!((PacketThread*)getDataOfSListNode(node))->active)
      {
        break;
      }
    }

    // Found an inactive node - use it
    if (node != NULL)
    {
      currPT = (PacketThread*)getDataOfSListNode(node);

      // Otherwise, create a new node
    }
    else
    {
      currPT = appendToSList(&packetThreads, sizeof (PacketThread));
      if (currPT == NULL)
      {
        RAISE_SYS_ERROR("Not enough space to receive another packet");
        break;
      }
      currPT->clThread = cthread;
      currPT->writeMutex = &cthread->writeMutex;
      currPT->readMutex = &rmutex;
      currPT->buffer = NULL;
      currPT->bufferSize = 0;
    }

    // Get the id and packet length
    lockMutex(&rmutex);
    if (!recvNum(&cthread->clientFD, (void*)&currPT->hdr,
                 sizeof (PacketLength) + sizeof (uint32_t)))
    {
      break;
    }

    // Receive and process in a separate thread
    currPT->active = true;
    pthread_create(&currPT->thread, NULL, multi_handleSinglePacket,
                   (void*)currPT);
  }

  // Thread list and internal buffers clean-up

  FOREACH_SLIST(&packetThreads, node)
  {
    safeFree(((PacketThread*)getDataOfSListNode(node))->buffer);
  }
  releaseSList(&packetThreads);

  // Mutex
  releaseMutex(&rmutex);

  // Finish up
  clientThreadCleanup(MODE_MULTIPLE_CLIENTS, cthread);

  LOG(LEVEL_DEBUG, "([0x%08X]) < Proxy Client Connection Thread stopped (3) "
                   "(ServerSocket::multi_handleClient)", pthread_self());
  
  pthread_exit(0);
}

/*---------------------
 * MODE_CUSTOM_CALLBACK
 */

/**
 * Thread that simply passes execution to the user's callback.
 *
 * @note MODE_CUSTOM_CALLBACK
 * @note PThread syntax
 *
 * @param data ClientThread instance
 * @return Always \c 0
 */
static void* custom_handleClient(void* data)
{
  ClientThread* cthread = (ClientThread*)data;
  
  LOG(LEVEL_DEBUG, "([0x%08X]) > Proxy Client Connection Thread started "
                   "(ServerSocket::custom_handleClient)", pthread_self());

  if (initWriteMutex(cthread))
  {
    cthread->svrSock->modeCallback(cthread->svrSock, cthread->clientFD, 
                                   cthread->svrSock->user);
// ((ClientConnectionAccepted)cthread->svrSock->modeCallback)(cthread->svrSock, 
//                                                            cthread->clientFD,
//                                                      cthread->svrSock->user);
  }
  clientThreadCleanup(MODE_CUSTOM_CALLBACK, cthread);
  
  LOG(LEVEL_DEBUG, "([0x%08X]) < Proxy Client Connection Thread stopped "
                   "(ServerSocket::custom_handleClient)", pthread_self());
  
  pthread_exit(0);
}

/*--------
 * Exports
 */

/**
 * Create the server socket where SRx is listening on.
 * @param self
 * @param port
 * @param verbose
 * @return
 */
bool createServerSocket(ServerSocket* self, int port, bool verbose)
{
  struct sockaddr_in addr;
  int yes = 1;

  // Create a TCP socket
  self->serverFD = socket(AF_INET, SOCK_STREAM, 0);
  if (self->serverFD < 0)
  {
    RAISE_SYS_ERROR("Failed to open a socket");
    return false;
  }

  // Bind to a server-address
  memset(&addr, 0, sizeof (struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY; // inet_pton
  addr.sin_port = htons(port);

  // Inserted to be able to restart after crash without having to wait for the 
  // socket to be released by the OS.
  setsockopt(self->serverFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (yes));

  if (bind(self->serverFD, (struct sockaddr*)&addr, 
           sizeof (struct sockaddr_in)) < 0)
  {
    char* errStr = NULL;
    switch (errno)
    {
      case EADDRINUSE:
        errStr = SOC_ERR_EADDRINUSE;                
        RAISE_ERROR("%s", errStr);
        break;
      case EADDRNOTAVAIL:
        errStr = SOC_ERR_EADDRNOTAVAIL;
        RAISE_ERROR("%s", errStr);
        break;
      case EAFNOSUPPORT:
        errStr = SOC_ERR_EAFNOSUPPORT;
        RAISE_ERROR("%s", errStr);
        break;
      case EBADF:
        errStr = SOC_ERR_EBADF;
        RAISE_ERROR("%s", errStr);
        break;
      case EINVAL:
        errStr = SOC_ERR_EINVAL;
        RAISE_ERROR("%s", errStr);
        break;
      case ENOTSOCK:
        errStr = SOC_ERR_ENOTSOCK;
        RAISE_ERROR("%s", errStr);
        break;
      case EOPNOTSUPP:
        errStr = SOC_ERR_EOPNOTSUPP;
        RAISE_ERROR("%s", errStr);
        break;
      case EACCES:
        errStr = SOC_ERR_EACCES;
        RAISE_ERROR("%s", errStr);
        break;
      case EDESTADDRREQ:
        errStr = SOC_ERR_EDESTADDRREQ;
        RAISE_ERROR("%s", errStr);
        break;
      case EISDIR:
        errStr = SOC_ERR_EISDIR;
        RAISE_ERROR("%s", errStr);
        break;
      case EIO:
        errStr = SOC_ERR_EIO;
        RAISE_ERROR("%s", errStr);
        break;
      case ELOOP:
        errStr = SOC_ERR_ELOOP;
        RAISE_ERROR("%s", errStr);
        break;
      case ENAMETOOLONG:
        errStr = SOC_ERR_ENAMETOOLONG;
        RAISE_ERROR("%s", errStr);
        break;
      case ENOENT:
        errStr = SOC_ERR_ENOENT;
        RAISE_ERROR("%s", errStr);
        break;
      case ENOTDIR:
        errStr = SOC_ERR_ENOTDIR;
        RAISE_ERROR("%s", errStr);
        break;
      case EROFS:
        errStr = SOC_ERR_EROFS;
        RAISE_ERROR("%s", errStr);
        break;
      case EISCONN:
        errStr = SOC_ERR_EISCONN;
        RAISE_ERROR("%s", errStr);
        break;
      case ENOBUFS:
        errStr = SOC_ERR_ENOBUFS;
        RAISE_ERROR("%s", errStr);
        break;
      default:
        errStr = SOC_ERR_UNKOWN;
        RAISE_ERROR("%s", errStr);
    }
    if (port < 1024)
    {
      LOG(LEVEL_ERROR, "Failed to bind the socket to the address, check if "
        "user has permission to bind system ports (<1024)\n");      
    }
    else
    {
      LOG(LEVEL_INFO, "Failed to bind the socket to the address, check if "
        "another process locks the port \'fuser -n tcp %u\'\n", port);
    }
    close(self->serverFD);
    return false;
  }

  // Misc. variables
  self->stopping = 0;
  self->verbose = verbose;

  return true;
}

/**
 * This is the server loop for the SRx - Proxy server connection.
 * 
 * @param self
 * @param clMode
 * @param modeCallback
 * @param statusCallback
 * @param user
 */
void runServerLoop(ServerSocket* self, ClientMode clMode,
                   void (*modeCallback)(), ClientStatusChanged statusCallback,
                   void* user)
{
  static void* (*CL_THREAD_ROUTINES[NUM_CLIENT_MODES])(void*) = {
                               single_handleSRxProxyClient,
                               multi_handleClient,
                               custom_handleClient
  };

  int cliendFD;
  struct sockaddr caddr;
  socklen_t caddrSize;
  char infoBuffer[MAX_SOCKET_STRING_LEN];
  ClientThread* cthread;
  int ret;

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  // Store the arguments
  self->mode = clMode;
  self->modeCallback = modeCallback;
  self->statusCallback = statusCallback;
  self->user = user;

  // No active threads
  initSList(&self->cthreads);

  // Prepare socket to accept connections
  listen(self->serverFD, MAX_PENDING_CONNECTIONS);
  
  for (;;)
  {
    LOG(LEVEL_DEBUG, HDR "Server loop, wait for clients...", pthread_self());
    // Wait for a connection from SRX proxy
    caddrSize = sizeof (struct sockaddr);
    cliendFD = accept(self->serverFD, &caddr, &caddrSize);

    // An (maybe intentional) error occurred - quit the loop
    if (cliendFD < 0)
    {
      // Socket has been closed
      if (errno == EBADF || errno == ECONNABORTED)
      {
        break;
      }

      // A non-processable error
      RAISE_SYS_ERROR("An error occurred while waiting for connections");
      break;
    }

    // Information
    if (self->verbose)
    {
      LOG(LEVEL_DEBUG, HDR "New client connection: %s", pthread_self(),
          sockAddrToStr(&caddr, infoBuffer, MAX_SOCKET_STRING_LEN));
      LOG(LEVEL_INFO, "New client connection: %s",
          sockAddrToStr(&caddr, infoBuffer, MAX_SOCKET_STRING_LEN));
    }

    // Spawn a thread for the new connection
    cthread = (ClientThread*)appendToSList(&self->cthreads,
                                           sizeof (ClientThread));
    if (cthread == NULL)
    {
      RAISE_ERROR("Not enough memory for another connection");
      close(cliendFD);
    }
    else
    {
      bool accepted = true;
      
      // Let the user know about the new client
      if (self->statusCallback != NULL)
      {
////////////////////////////////////////////////////////////////////////////////
        //TODO: the mode might not be needed anymore
        accepted = self->statusCallback(self,
                                        (clMode == MODE_SINGLE_CLIENT) ? cthread
                                                                       : NULL,
                                        cliendFD, true, self->user);
      }

      // Start the thread
      if (accepted)
      {
        cthread->active          = true;
        cthread->initialized     = false;
        cthread->goodByeReceived = false;
        
        cthread->proxyID  = 0; // will be changed for srx-proxy during handshake
        cthread->routerID = 0; // Indicates that it is currently not usable, 
                               // must be set during handshake
        cthread->clientFD = cliendFD;
        cthread->svrSock  = self;
        cthread->caddr	  = caddr;
#ifdef USE_GRPC
        cthread->type_grpc_client = false;
#endif

        ret = pthread_create(&(cthread->thread), &attr,
                             CL_THREAD_ROUTINES[clMode],
                             (void*)cthread);
        if (ret != 0)
        {
          accepted = false;
          RAISE_ERROR("Failed to create a client thread");
        }
      }

      // Error or the callback denied the client
      if (!accepted)
      {
        close(cliendFD);
        deleteFromSList(&self->cthreads, cthread);
      }
    }
  }
}

/**
 * Stops the particular client thread by closing the connection, ending the 
 * thread and releasing the mutex.
 *
 * @param clt A ClientThread instance
 */
static void _killClientThread(void* clt)
{
  ClientThread* clientThread = (ClientThread*)clt;

  if (clientThread->active)
  {
    // Close the client connection
#ifdef USE_GRPC
    if(!clientThread->type_grpc_client)
      close(clientThread->clientFD);
#else
    close(clientThread->clientFD);
#endif

    // Wait until the thread terminated - if necessary
    //pthread_join(clientThread->thread, NULL);
    pthread_cancel(clientThread->thread);

    // Release the write-mutex
    releaseMutex(&clientThread->writeMutex);
    
    // Set it inactive
    clientThread->active = false;
  }
}

/**
 * Stop the server loop, kills all client threads and closes the sockets.
 * 
 * @param self The server socket instance.
 */
void stopServerLoop(ServerSocket* self)
{
  if (++self->stopping == 1)
  {
    // Stop accepting connections 
    close(self->serverFD);

    // Kill all threads
    foreachInSList(&self->cthreads, _killClientThread);
    releaseSList(&self->cthreads);
  }
}

//TODO: Maybe Change the signature and remove the ServerSocket instance. 
// Not needed in the future, it only provides the mode and this will disappear
bool sendPacketToClient(ServerSocket* self, ServerClient* client,
                        void* data, size_t size)
{
  if (self == NULL)
  {
    RAISE_ERROR("Server Socket instance is NULL");
    return false;
  }

  if (self->mode == MODE_SINGLE_CLIENT)
  {
    return single_sendResult(client, data, size);
  }
  if (self->mode == MODE_MULTIPLE_CLIENTS)
  {
    return multi_sendResult(client, data, size);
  }
  RAISE_ERROR("Cannot send packets in this mode");
  return false;
}

/**
 * Closes the connection associated with the given client.
 * 
 * @param self The server socket whose client has to be handled,
 * @param client The client connection object to be closed.
 * 
 * @return true if the socket could be closed.
 */
int closeClientConnection(ServerSocket* self, ServerClient* client)
{
  ClientThread* clientThread = (ClientThread*)client;

  LOG(LEVEL_DEBUG, HDR "Close and remove client: Thread [0x%08X]; [ID :%u]; "
                       "[FD: 0x%08X]", pthread_self() , clientThread->thread,
                       clientThread->proxyID, clientThread->clientFD);
#ifdef USE_GRPC
  if (clientThread->svrSock->statusCallback != NULL)
  {
    clientThread->svrSock->statusCallback(clientThread->svrSock, clientThread, 
                                        -1, false, clientThread->svrSock->user);
  }
#endif // USE_GRPC
  
  //deleteMapping(self, clientThread);
  _killClientThread(clientThread);
  LOG(LEVEL_DEBUG, HDR "Client connection [ID:%u] closed!", pthread_self(),
                  clientThread->proxyID);
  LOG(LEVEL_INFO, "Client connection [ID:%u] closed!", clientThread->proxyID);

  deleteFromSList(&self->cthreads, clientThread);
  
  return true;
}





