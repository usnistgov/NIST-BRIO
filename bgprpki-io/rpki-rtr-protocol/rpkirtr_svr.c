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
 * An RPKI/Router Protocol server test harness. This Software simulates an
 * RPKI Validation Cache.
 *
 *  * - "Serial Notify"s are send out no more than once a minute
 *   (see SERVICE_TIMER_INTERVAL)
 * - Removed, i.e. withdrawn routes are kept for one hour
 *   (see CACHE_EXPIRATION_INTERVAL)
 *
 * @version 0.7.0.0
 *
 * Changelog:
 * -----------------------------------------------------------------------------
 * 0.7.0.0  - 2025/01/27 - oborchert
 *            * Fixed test spellers and synchronized Versioning.
 * 0.6.2.2  - 2024/09/29 - oborchert
 *            * Moved code over to brio and introcduced BRIO tag
 *            * Fixed bug in processEntryRemoval.
 * 0.6.2.1  - 2024/09/24 - oborchert
 *            * Fixed serial generation when replaceing ASPA objects.
 *          - 2024/09/23 - oborchert
 *            * Updated RPKI_RTR_SRV_VER.
 *            * Fixed error handling for incorrect ASPA scripting.
 *            * Fixed memory leak in removal of ASPA objects.
 *          - 2024/09/20 - oborchert
 *            * Added findASPA and remove ASPA objects that will be overwritten
 *              using addASPA.
 *          - 2024/09/11 - oborchert
 *            * Updated cache.version number to represent the latest protocol 
 *              number.
 *          - 2024/09/10 - oborchert
 *            * Changed data types from u_int... to uint... which follows C99
 *            * Added sendEndOfData to allow distinguishing between version 1
 *              and version 2 processing.
 *          - 2024/09/03 - oborchert
 *            * Moved SKIP_IF macro from within function to top of file and 
 *              added one more parameter.
 *            * Fixed some documentation.
 *            * Fixed segmentation fault in ASPA processing.
 *          - 2024/08/27 - oborchert
 *            * Added more printout information to sendErrorPDU. 
 * 0.6.2.0  - 2024/07/26 - oborchert
 *            * Removed AFI from ASPA according to 8210-bis13.
 * 0.6.1.0  - 2021/11/05 - oborchert
 *            * Increased the line buffer for reading files from 255 bytes to 
 *              4KiB. ASPA objects can be rather large.
 * 0.6.0.1  - 2021/07/30 - oborchert
 *            * Fixed speller in output (imported from version 5)
 * 0.6.0.0  - 2021/03/30 - oborchert
 *            * Added missing version control. Also moved modifications labeled 
 *              as version 0.5.2.0 to 0.6.0.0 (0.5.2.0 was skipped)
 *            * Cleaned up some merger left overs and synchronized with naming 
 *              used conventions.
 *          - 2021/02/17 - oborchert
 *            * Fixed incorrect encoding of ASPA afi value during ASPA object 
 *              creation.
 *          - 2021/02/12 - oborchert
 *            * Renamed function sendPrefixes() to sendCacheObjects() to reflect
 *              more the correct function.
 *            * Added proper version handling for keys in version 2
 *          - 2021/02/10 - oborchert
 *            * Removed in valid printout for addKey help. It incorrectly
 *              stated an algorithm ID is required.
 *            * Added identifier during printout of cache objects.
 *          - 2021/02/09 - oborchert
 *            * Added CMD_ERROR as valid return value for commands not executed
 *              due to an error (most likely incomplete commands)
 *            * Redesigned code execution for all append... functions to move
 *              shared code into _append... and have them called with the proper
 *              parameters. This allows to reduce speller prone printouts.
 *            * Added helper function _printAppendError
 *          - 2021/02/08 - oborchert
 *            * Added more ASPA processing. 
 *          - 2020/11/24 - oborchert
 *            * Added ASPA data structure
 * 0.5.1.0  - 2018/06/09 - oborchert
 *            * Added command 'echo' to allow printing messaged from a script
 *              to the console. CMD_ID_ECHO
 *            * Added command waitFor <client-ip> to allow to wait until the 
 *              specific client connects. This will time out after 60 seconds.
 *            * Added command pause to allow to wait until any key is pressed.
 *          - 2018/03/09 - oborchert 
 *            * BZ1263: Merged branch 0.5.0.x (version 0.5.0.4) into trunk 
 *              of 0.5.1.0.
 *          - 2017/10/12 - oborchert
 *            * BZ1103: Fixed incorrect RFC reference
 * 0.5.0.5  - 2018/05/17 - oborchert
 *            * (merged branch 0.5.0 into trunk)
 *          - 2018/04/24 - oborchert
 *            * Modified the function printLogMessage to use the current log 
 *              level rather than hard coded log level.
 *            * Change default value for verbose to false.
 * 0.5.0.4  - 2018/03/08 - oborchert
 *            * Fixed incorrect processing of parameters.
 *            * Fixed incorrect syntax printout.
 * 0.5.0.3  - 2018/03/01 - oborchert
 *            * Added proper program stop when help parameter is provided.
 *            * Fixed printout for router keys.
 *          - 2018/02/28 - oborchert
 *            * Fixed usage of incorrect version number.
 * 0.5.0.1  - 2017/09/25 - oborchert
 *            * Fixed compiler warnings.
 * 0.5.0.0  - 2017/07/08 - oborchert
 *            * Fixed some prompt handling in console
 *            * BZ1185: fixed issue with 'cache' command showing all entries
 *              as SKI's
 *            * Added '*' to allow switching between auto complete and browsing
 *              the file system.
 *          - 2017/07/07 - oborchert
 *            * BZ1183: Fixed issues with history.
 *            * Added auto completion in command window (use tab)
 *          - 2017/06/05 - oborchert
 *            * Added parameter -D <level> to set debug level.
 *              Moved current debug level to LEVEL_ERROR
 *            * fixed segmentation fault for addKey with missing parameters.
 *            * Modified the ley loading aligning it with the command set of
 *              prefix loading.
 *            * Added keyLoc to provide a key location folder
 *            * Added addKeyNow
 *          - 2017/06/16 - kyehwanl
 *            * Updated code to use RFC8210 (former 6810-bis-9)
 *          - 2017/06/16 - oborchert
 *            * Version 0.4.1.0 is trashed and moved to 0.5.0.0
 *          - 2016/08/30 - oborchert
 *            * Added a proper configuration section.
 *          - 2016/08/26 - oborchert
 *            * Changed client list display from using index to file descriptor
 *              which does not change when another client disconnects.
 *          - 2016/08/19 - oborchert
 *            * Modified the CTRL+C handler to use sigaction instead. Added
 *              a more gracefull stop.
 *            * Modified old fix M713 to not call trim on NULL. BZ1017
 * 0.4.0.2  - 2016/08/12 - oborchert
 *            * Changed default port from 50000 to 323 as specified by RFC6810
 * 0.3.0.10 - 2015/11/10 - oborchert
 *            * Added parentheses around comparison in operand & in sendPrefixes
 *            * Removed unused sessionID from function readPrefixData
 * 0.3.0.2  - 2013/07/08 - oborchert
 *            * Added an ID for each command to allow acing on them after they
 *              are executed.
 *            * Allows the exit/quit/\q command to be executed from within a
 *              script
 *            * Allowed to end the program when a script is passed and the
 *              last command is quit BZ# 351
 *            * Changed all command processing methods to return the proper
 *              command id CMD_ID_<command>
 * 0.3.0    - 2013/01/28 - oborchert
 *            * Update to be compliant to draft-ietf-sidr-rpki-rtr.26. This
 *              update does not include the secure protocol section. The
 *              protocol will still use un-encrypted plain TCP
 * 0.2.2    - 2012/12/28 - oborchert
 *            * Modified update 0.2.1. Fix BZ165 caused no ROA to be
 *              installed into the cache. Applied new fix.
 * 0.2.1    - 2012/07/25 - kyehwan
 *            * Fixed segmentation fault while adding ROAs.
 * 0.2.0    - 2011/01/07 - oborchert
 *            * Changelog added with version 0.2.0 and date 2011/01/07
 *            * Version tag added
 *            * M0000713: Cleaned console input string with trim()
 *            * Added capability of adding single "white list" entries through
 *            * console (add ...).
 *            * Added version information.
 *            * Added addNow and removeNow to bypass the 60 seconds delay timer.
 *            * Rewritten code for prototype 2.
 *            * following draft-ietf-sidr-rpki-rtr.10
 * 0.1.0    - 2010/06/02 - pgleichm
 *            * Code Created Prototype 1
              * following draft-ymbk-rtr-protocol-05
 * -----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <uthash.h>
#include <unistd.h>
#include <srx/srxcryptoapi.h>
#ifndef BRIO
  #include "server/srx_server.h"
  #include "shared/rpki_router.h"
  #include "util/debug.h"
  #include "util/log.h"
  #include "util/math.h"
  #include "util/prefix.h"
  #include "util/rwlock.h"
  #include "util/mutex.h"
  #include "util/server_socket.h"
  #include "util/slist.h"
  #include "util/socket.h"
  #include "util/str.h"
  #include "util/timer.h"
  #include "util/prefix.h"
#else
  #include "rpki-rtr-protocol/rpki_router.h"
  #include "antd-util/log.h"
  #include "antd-util/prefix.h"
  #include "srx-common/debug.h"
  #include "srx-common/math.h"
  #include "srx-common/rwlock.h"
  #include "srx-common/mutex.h"
  #include "srx-common/server_socket.h"
  #include "srx-common/slist.h"
  #include "srx-common/socket.h"
  #include "srx-common/str.h"
  #include "srx-common/timer.h"
  #include "rpki-rtr-protocol/rpkirtr_svr.h"
#endif

// Max characters per line - Increase to 4KB. This is needed because
// ASPA objects can become rather large.
#define LINE_BUF_SIZE 4096

/** This structure specified one cache entry. */
typedef struct {
  /** Current serial number of the entry */
  uint32_t  serial;
  /** Previous serial number (before being withdrawn) */
  uint32_t  prevSerial;
  /** When this entry expires, i.e. should be deleted */
  time_t    expires;
  /** true if IPv6 prefix */
  bool      isV6;
  /* router key indicator */
  bool      isKey;
  /* ASPA identifier */
  bool      isASPA;

  /** Prefix (v4, v6) - stored in network Byte order */
  uint8_t   flags;           // Might not be needed.
  /** Length of the prefix */
  uint8_t   prefixLength;
  /** Max length of prefix */
  uint8_t   prefixMaxLength;
  /** The AS number for this entry / Customer AS if ASPA*/
  uint32_t  asNumber;
  /** The IP Address */
  union {
    /** The IPv4 Address */
    IPv4Address v4;
    /** The IPv6 Address */
    IPv6Address v6;
  } address;

  char* ski;            // Subject Key Identifier
  char* pPubKeyData;    // Subject Public Key Info
  
  /** The provider information */
  uint16_t  providerCount;
  /** List of providers == NULL if provider count = 0*/
  uint8_t* providerAS;
} ValCacheEntry;

/** Single client */
typedef struct {
  /** Socket - but also the hash identifier */
  int             fd;
  /** Hash handle */
  UT_hash_handle  hh;
  /** The version used with this client */
  int             version;
} CacheClient;

/**
 * This configuration structure allows to pass some more configuration settings
 * to the server.
 *
 * @since 0.5.0.0
 */
typedef struct {
  /** The configured port */
  int   port;
  /** A script containing cache commands to be executed upon start */
  char* script;
} RPKI_SRV_Configuration;

#define CMD_ID_QUIT          0
#define CMD_ID_UNKNOWN       1
#define CMD_ID_VERBOSE       2
#define CMD_ID_CACHE         3
#define CMD_ID_VERSION       4
#define CMD_ID_HELP          5
#define CMD_ID_CREDITS       6
#define CMD_ID_SESSID        7
#define CMD_ID_EMPTY         8
#define CMD_ID_ADD           9
#define CMD_ID_ADDNOW       10
#define CMD_ID_REMOVE       11
#define CMD_ID_REMOVENOW    12
#define CMD_ID_ERROR        13
#define CMD_ID_NOTIFY       14
#define CMD_ID_RESET        15
#define CMD_ID_CLIENTS      16
#define CMD_ID_RUN          17
#define CMD_ID_SLEEP        18
#define CMD_ID_KEY_LOC      19
#define CMD_ID_ECHO         20
#define CMD_ID_WAIT_CLIENT  21
#define CMD_ID_PAUSE        22
#define CMD_ERROR           30

#define DEF_RPKI_PORT    323
#define UNDEF_VERSION    -1
/*----------
 * Constants
 */
#ifndef BRIO
  #ifdef PACKAGE_VERSION
    const char* RPKI_RTR_SRV_VER          = PACKAGE_VERSION "\0";
  #else
    const char* RPKI_RTR_SRV_VER          = "> 0.6.2\0";
  #endif
  const char* RPKI_RTR_SRV_NAME         = "RPKI Cache Test Harness\0";
  const char* HISTORY_FILENAME          = ".rpkirtr_svr.history\0";
#else
  #ifdef PACKAGE_VERSION
    const char* BRIO_RPKI_CACHE_VER       = PACKAGE_VERSION "\0";
  #else
    const char* BRIO_RPKI_CACHE_VER       = "> 0.6.2\0";
  #endif
  const char* BRIO_RPKI_CACHE_NAME      = "BRIO RPKI Cache Test Harness\0";
  const char* HISTORY_FILENAME          = ".brio_cache.history\0";
#endif
const char* USER_PROMPT               = ">> \0";
const int   SERVICE_TIMER_INTERVAL    = 60;   ///< Service interval (sec)
const int   CACHE_EXPIRATION_INTERVAL = 3600; ///< Sec. to keep removed entries

#define PUB_KEY_OCTET 91
#define KEY_BIN_SIZE 91
#define MAX_CERT_READ_SIZE 260
#define OFFSET_PUBKEY 170
#define OFFSET_SKI 130
#define COMMAND_BUF_SIZE 256
/*-----------------
 * Global variables
 */
struct {
  SList     entries;
  RWLock    lock;
  uint32_t  maxSerial;
  uint32_t  minPSExpired, maxSExpired;
  uint8_t   version;

  // Values for Version 2 - see RFC8210 for more
  uint32_t  refreshInterval; // = PDU_EOD_REFRESH_DEF;
  uint32_t  retryInterval;   // = PDU_EOD_RETRY_DEF;
  uint32_t  expireInterval;  // = PDU_EOD_EXPIRE_DEF;
} cache;

struct {
  int   timer;
  bool  notify;
} service;

/** Reference to the server socket. */
ServerSocket svrSocket;
/** A list of cache clients */
CacheClient* clients   = NULL;
/** Verbose mode on or off */
bool         verbose   = false;
/** The current cache session id value */
uint16_t     sessionID = 0;

/** Used to indicate if the system is in a controlled wait loop. This allows
 *  The CTRL+C handler to not initiate a shutdown but set the ctr__c variable
 *  to true. */
bool         inWait    = false;
/** Indicates if the ctrl+c combination was pressed.  */
bool         ctrl_c    = false;
/** Location (directory) where the key files are stored. */
char keyLocation[LINE_BUF_SIZE];

/*---------------
 * Utility macros
 */

#define OPROMPT() \
  puts(USER_PROMPT); \
  fflush(stdout)

#define OUTPUTF(LAST, FMT, ...) \
  if (verbose) {                 \
    putc('\r', stdout);          \
    printf(FMT, ## __VA_ARGS__); \
    if (LAST) {                  \
      OPROMPT();                 \
    }                            \
  }

#define ERRORF(FMT, ...) \
  printf(FMT, ## __VA_ARGS__); \
  OPROMPT()

#define SKIP_IF(COND, MSG, VAR, IS_FILE) \
  if (COND)                     \
  {                             \
    if (IS_FILE)                \
    {                           \
      ERRORF("Error: " MSG " (line %d): '%s'\n", lineNo, VAR); \
    } else {                    \
      ERRORF("Error: " MSG " : '%s'\n", VAR); \
    }                           \
    continue;                   \
  }

////////////////////////////////////////////////////////////////////////////////
// CLIENT SERVER COMMUNICATION AND UTILITIES
////////////////////////////////////////////////////////////////////////////////
/**
 * This function checks the validity of the requested serial number. It might
 * be that the cache performed a serial number overflow. This means the serial
 * number reached its natural max and got rolled over to zero. In this case the
 * cache_min number could be greater than cache_max.
 *
 * @param cache_min The minimum serial number (oldest one)
 * @param cache_max The latest given serial number (newest one)
 * @param client_serial the serial number of the client.
 * @return
 */
bool checkSerial(uint32_t cache_min, uint32_t cache_max, uint32_t client_serial)
{
  bool result;
  if (cache_min < cache_max)
  {
    result = (client_serial >= cache_min) && (client_serial <= cache_max);
  }
  else
  {
    // check that the serial number is not located in the unused range
    // Valid serial numbers are 0..max and min...max(int) with max < min
    // Unused = [max+1...min-1]
    result = !(client_serial < cache_min) && (client_serial > cache_max);
  }
  return result;
}

/**
 * Drop the session to the session with the given file descriptor.
 * @param fdPtr The file descriptor
 *
 * @return true if the session could be dropped.
 */
bool dropSession(int* fdPtr)
{
  bool result = false;
  if (HASH_COUNT(clients) > 0)
  {
    CacheClient*  cl;

    acquireReadLock(&cache.lock);
    for (cl = clients; cl; cl = cl->hh.next)
    {
      if (&cl->fd == fdPtr)
      {
        OUTPUTF(true, "Close session to the given client\n");

        // TODO: Close session and remove it from list.

        result = true;
        break;
      }
    }
    unlockReadLock(&cache.lock);
  }

  return result;
}

/**
 * Send a PDU that contains the serial field. This method can be used to
 * send SERIAL_NOTIFY (4.1) or SERIAL_QUERY (4.2)
 * @param fdPtr The file descriptor to be used to send the packet.
 * @param type The PDU type.
 *
 * @return
 */
bool sendPDUWithSerial(int* fdPtr, RPKIRouterPDUType type, uint32_t serial, 
                       uint8_t version)
{
  uint8_t                pdu[sizeof(RPKISerialQueryHeader)];
  RPKISerialQueryHeader* hdr;

  // Create PDU
  hdr = (RPKISerialQueryHeader*)pdu;
  hdr->version   = version;
  hdr->type      = (uint8_t)type;
  hdr->sessionID = htons(sessionID);
  hdr->length    = htonl(sizeof(RPKISerialQueryHeader));
  hdr->serial    = htonl(serial);
  // Send
  OUTPUTF(false, "Sending an RPKI-RTR 'PDU[%u] with Serial'\n", type);
  return sendNum(fdPtr, &pdu, sizeof(RPKISerialQueryHeader));
}


/**
 * Send the end of data PDU.
 * @since 0.6.2.1
 */
bool sendEndOfData(int* fdPtr, uint32_t serial, uint8_t version, 
                     uint32_t refresh_interval, uint32_t retry_interval,
                     uint32_t expire_interval)
{
  uint8_t                pdu[sizeof(RPKIEndOfDataHeader_2)];
  RPKIEndOfDataHeader_2* hdr    = (RPKIEndOfDataHeader_2*)pdu;
  uint32_t               length = sizeof(RPKIEndOfDataHeader);
  bool                   retVal = false;

  memset(hdr, 0, sizeof(sizeof(RPKIEndOfDataHeader_2)));

  switch (version)
  {
    case 2:
      hdr->refresh_interval = htonl(refresh_interval);
      hdr->retry_interval   = htonl(retry_interval);
      hdr->expire_interval  = htonl(expire_interval);
      length                = sizeof(RPKIEndOfDataHeader_2);
    case 1:
      hdr->v1.version       = version;
      hdr->v1.type          = (uint8_t)PDU_TYPE_END_OF_DATA;
      hdr->v1.sessionID     = htons(sessionID);
      hdr->v1.length        = htonl(length);
      hdr->v1.serial        = htonl(serial);
      // Send
      OUTPUTF(false, "Sending an RPKI-RTR 'PDU[%u] with Serial'\n", 
              PDU_TYPE_END_OF_DATA);
      retVal = sendNum(fdPtr, &pdu, length);
      break;
    default:
  }
  return retVal;
}

/**
 * Send a CACHE RESET to the client.
 *
 * @param fdPtr the socket connection
 * @param version The version for this session.
 *
 * @return true id the packet was send successful.
 */
bool sendCacheReset(int* fdPtr, uint8_t version)
{
  uint8_t               pdu[sizeof(RPKICacheResetHeader)];
  RPKICacheResetHeader* hdr;

  // Create PDU
  hdr = (RPKICacheResetHeader*)pdu;
  hdr->version  = version;
  hdr->type     = (uint8_t)PDU_TYPE_CACHE_RESET;
  hdr->reserved = 0;
  hdr->length   = htonl(sizeof(RPKICacheResetHeader));

  return sendNum(fdPtr, &pdu, sizeof(RPKICacheResetHeader));
}

/**
 * Send a CACHE RESPONSE to the client.
 *
 * @param fdPtr the socket connection
 * @param version The version number of this session
 *
 * @return true id the packet was send successful.
 */
bool sendCacheResponse(int* fdPtr, uint8_t version)
{
  uint8_t                  pdu[sizeof(RPKICacheResetHeader)];
  RPKICacheResponseHeader* hdr;

  // Create PDU
  hdr = (RPKICacheResponseHeader*)pdu;
  hdr->version   = version;
  hdr->type      = (uint8_t)PDU_TYPE_CACHE_RESPONSE;
  hdr->sessionID = htons(sessionID);
  hdr->length    = htonl(sizeof(RPKICacheResetHeader));

  OUTPUTF(true, "Sending a 'Cache Response'\n");
  return sendNum(fdPtr, &pdu, sizeof(RPKICacheResetHeader));
}

/**
 * This function was previously called sendPrefixes but in the meantime not only
 * ROA prefixes are sent RFC8610, also BGPsec keys RFC8210as well as 
 * ASPA objects RFC8210-bis.
 *
 * @param fdPtr The file descriptor
 * @param clientSerial the serial the client requested.
 * @param clientSessionID the sessionID of the client request.
 * @param isReset if set to true both clientSerial nor clientSessionID is
 *                ignored.
 */
void sendCacheObjects(int* fdPtr, uint32_t clientSerial, 
                      uint16_t clientSessionID, bool isReset, uint8_t version)
{
  // No need to send the notify anymore
  service.notify = false;

  // Let no one modify the cache
  acquireReadLock(&cache.lock);

  // Send "Cache Reset" in case
  // A: The client and the cache operate on a different SESSION IDddd
  // B: The serial of the client can not be served buy the cache.
  if (!isReset && (clientSessionID != sessionID))
  { // session id is incorrect, drop this session
    dropSession(fdPtr);
  }
  else if (   !isReset
           && (checkSerial(cache.minPSExpired, cache.maxSExpired, clientSerial))
          )
  { // Serial is incorrect, send a Cache Reset
    if (!sendCacheReset(fdPtr, version))
    {
      ERRORF("Error: Failed to send a 'Cache Reset'\n");
    }
  }
  else
  { // Send the prefix
    // Send 'Cache Response'
    if (!sendCacheResponse(fdPtr, version))
    {
      ERRORF("Error: Failed to send a 'Cache Response'\n");
    }
    else
    {
      OUTPUTF(true, "Cache size = %u\n", cache.entries.size);
      if (cache.entries.size > 0) // there is always a root.
      {
        ValCacheEntry* cEntry;

        uint8_t               v4pdu[sizeof(RPKIIPv4PrefixHeader)];
        uint8_t               v6pdu[sizeof(RPKIIPv6PrefixHeader)];
        RPKIIPv4PrefixHeader* v4hdr = (RPKIIPv4PrefixHeader*)v4pdu;
        RPKIIPv6PrefixHeader* v6hdr = (RPKIIPv6PrefixHeader*)v6pdu;
        RPKIRouterKeyHeader   rkhdr;
        // This is used to allow to grow the ASPA PDU memory as needed.
        // The ASPA PDU size depends on the number of providerASes sent.
        // + 4 Because each ASPAHeader contains at least one ProviderAS which
        // in NOT part of the static Header structure because 1..N providers
        // van exist.
        uint32_t              aspaBufferSize    = sizeof(RPKIASPAHeader)+4;
        uint32_t              aspaBufferMinSize = aspaBufferSize;
        uint8_t*              aspaBuffer = malloc(aspaBufferSize);
        RPKIASPAHeader*       aspahdr = (RPKIASPAHeader*)aspaBuffer;
        memset(aspaBuffer, 0, aspaBufferSize);
        
        // Basic initialization of data that does NOT change
        // IPv4 Prefix PDU
        v4hdr->version  = version;
        v4hdr->type     = PDU_TYPE_IP_V4_PREFIX;
        v4hdr->reserved = 0;
        v4hdr->length   = htonl(sizeof(RPKIIPv4PrefixHeader));

        // IPv6 Prefix PDU
        v6hdr->version  = version;
        v6hdr->type     = PDU_TYPE_IP_V6_PREFIX;
        v6hdr->reserved = 0;
        v6hdr->length   = htonl(sizeof(RPKIIPv6PrefixHeader));

        // Router Key PDU
        rkhdr.version   = version;
        rkhdr.type      = PDU_TYPE_ROUTER_KEY;
        rkhdr.zero      = 0;
      
        // ASPA PDU
        aspahdr->version = version;
        aspahdr->type    = PDU_TYPE_ASPA;
        aspahdr->zero_1  = 0;
        aspahdr->zero_2  = 0;
        
        // helps to find the next serial number
        SListNode*  currNode;
        uint32_t    serial;

        // Go through list until next available serial is found
        FOREACH_SLIST(&cache.entries, currNode)
        {
          serial = ((ValCacheEntry*)getDataOfSListNode(currNode))->serial;
          if (isReset || (serial > clientSerial))
          {
            break;
          }
        }

        // Go over each node. currNode is not null if a serial was found.
        for (; currNode; currNode = getNextNodeOfSListNode(currNode))
        {
          cEntry = (ValCacheEntry*)getDataOfSListNode(currNode);

          // Skip entries that are already expired.
          if (isReset)
          {
            if ((cEntry->flags & PREFIX_FLAG_ANNOUNCEMENT) == 0)
            {
              // This entry is NOT an announcement. Because we send a fresh set,
              // only announcements will be send, no withdrawals.
              continue;
            }
          }

          // Skip entries that were never announced to the client
          if (   (cEntry->serial != cEntry->prevSerial)
              && (cEntry->prevSerial > clientSerial))
          {
            continue;
          }

          // Send 'Router Key'
          if( cEntry->isKey == true && cEntry->prefixLength == 0 &&
              cEntry->prefixMaxLength == 0 &&
              cEntry->ski && cEntry->pPubKeyData)

          {
            // Change from version == 1 to faster != 0
            if (version != 0)
            {
              rkhdr.flags     = cEntry->flags;
              memcpy(&rkhdr.ski, cEntry->ski, SKI_LENGTH);
              memcpy(&rkhdr.keyInfo, cEntry->pPubKeyData, KEY_BIN_SIZE);
              rkhdr.as        = cEntry->asNumber;
              rkhdr.length    = htonl(sizeof(RPKIRouterKeyHeader));

              OUTPUTF(false, "Sending an 'Router Key' (serial = %u)\n",
                  cEntry->serial);
              if (!sendNum(fdPtr, &rkhdr, sizeof(RPKIRouterKeyHeader)))
              {
                ERRORF("Error: Failed to send a 'RouterKey'\n");
                break;
              }
              continue;
            }
          }
          else if (cEntry->isASPA)
          {
            if (version > 1)
            {
              // First determine if the header buffer is large enough.
              aspaBufferMinSize = sizeof(RPKIASPAHeader) 
                                  + (ntohs(cEntry->providerCount) * 4);
              aspahdr->length            = htonl(aspaBufferMinSize);
              aspahdr->flags             = cEntry->flags;
              aspahdr->customer_asn      = cEntry->asNumber;
              if ( (cEntry->flags & PREFIX_FLAG_ANNOUNCEMENT) != 0 )
              {
              aspahdr->provider_as_count = cEntry->providerCount;
              aspahdr->provider_as_count = cEntry->providerCount;
              aspahdr->customer_asn      = cEntry->asNumber;
                aspahdr->provider_as_count = cEntry->providerCount;
              aspahdr->customer_asn      = cEntry->asNumber;
                if (aspaBufferSize < aspaBufferMinSize)
                {
                  uint8_t* newBuffer = realloc(aspaBuffer, aspaBufferMinSize);
                  if (newBuffer != NULL)
                  {
                    aspaBuffer     = newBuffer;
                    aspaBufferSize = aspaBufferMinSize;
                    aspahdr        = (RPKIASPAHeader*)aspaBuffer;
                    newBuffer      = NULL;
                  }
                  else
                  {
                    ERRORF("Error: Not enough memory for ASPA object!\n");
                    break;
                  }
                }
                // Now use a helper pointer to copy the list of provider ASs to 
                // into the PDU
                uint8_t* ptr = aspaBuffer + sizeof(RPKIASPAHeader);
                memcpy(ptr, (uint8_t*)cEntry->providerAS, 
                      ntohs(cEntry->providerCount) * 4);
              }
              else
              {
                // Withdrawal - No PRovider ASes
                aspaBufferMinSize = sizeof(RPKIASPAHeader);
                aspahdr->provider_as_count = 0;
              }

              // Here aspahdr is a pointer so we can hand is over directly.
              if (!sendNum(fdPtr, aspahdr, aspaBufferMinSize))
              {
                ERRORF("Error: Failed to send an 'ASPA' object!\n");
                break;
               }
            }
          }
          else
          {
            // Send 'Prefix'
            if (!cEntry->isV6)
            {
              v4hdr->flags     = cEntry->flags;
              v4hdr->prefixLen = cEntry->prefixLength;
              v4hdr->maxLen    = cEntry->prefixMaxLength;
              v4hdr->zero      = (uint8_t)0;
              v4hdr->addr      = cEntry->address.v4;
              v4hdr->as        = cEntry->asNumber;
              OUTPUTF(false, "Sending an 'IPv4Prefix' (serial = %u)\n",
                  cEntry->serial);
              if (!sendNum(fdPtr, &v4pdu, sizeof(RPKIIPv4PrefixHeader)))
              {
                ERRORF("Error: Failed to send a 'Prefix'\n");
                break;
              }
            }
            else
            {
              v6hdr->flags     = cEntry->flags;
              v6hdr->prefixLen = cEntry->prefixLength;
              v6hdr->maxLen    = cEntry->prefixMaxLength;
              v6hdr->zero      = (uint8_t)0;
              v6hdr->addr      = cEntry->address.v6;
              v6hdr->as        = cEntry->asNumber;
              OUTPUTF(false, "Sending an 'IPv6Prefix' (serial = %u)\n",
                  cEntry->serial);
              if (!sendNum(fdPtr, &v6pdu, sizeof(RPKIIPv6PrefixHeader)))
              {
                ERRORF("Error: Failed to send a 'Prefix'\n");
                break;
              }
            }
          }
        }
        
        if (aspaBuffer != NULL)
        {
          free (aspaBuffer);
          aspaBuffer = NULL;
        }
      }

      // Send 'End of Data'
      OUTPUTF(true, "Sending an 'End of Data (max. serial = %u)\n",
              cache.maxSerial);

      // was sending cache version, not session version.
      if (!sendEndOfData(fdPtr, cache.maxSerial, version, cache.refreshInterval,
                         cache.retryInterval, cache.expireInterval))
      {
        ERRORF("Error: Failed to send a 'End of Data'\n");
      }
    }
  }
  unlockReadLock(&cache.lock);
}

/**
 * Send a SERIAL NOTIFY to all clients of the test harness
 *
 * @return CMD_ID_NOTIFY
 */
int sendSerialNotifyToAllClients()
{
  if (HASH_COUNT(clients) > 0)
  {
    CacheClient*  client;

    OUTPUTF(true, "Sending multiple 'Serial Notify' (max. serial = %u)\n",
            cache.maxSerial);

    acquireReadLock(&cache.lock);
    for (client = clients; client; client = client->hh.next)
    {
      if (!sendPDUWithSerial(&client->fd, PDU_TYPE_SERIAL_NOTIFY,
                             cache.maxSerial, client->version))
      {
        ERRORF("Error: Failed to send a 'Serial Notify\n");
      }
    }

    unlockReadLock(&cache.lock);
  }

  return CMD_ID_NOTIFY;
}

/**
 * Sends a Cache Reset message to all clients.
 *
 * @return CMD_ID_RESET;
 */
int sendCacheResetToAllClients()
{
  if (HASH_COUNT(clients) > 0)
  {
    CacheClient*  client;

    OUTPUTF(true, "Sending 'Cache Reset' to all clients\n");

    for (client = clients; client; client = client->hh.next)
    {
      if (!sendCacheReset(&client->fd, client->version))
      {
        ERRORF("Error: Failed to send a 'Cache Reset\n");
      }
    }

    unlockReadLock(&cache.lock);
  }

  return CMD_ID_RESET;
}

/**
 * Send an error report to all clients.
 *
 * @param fdPtr the socket connection
 * @param the error number to be send
 * @param data contains the error number followed by the PDU and text. The
 *             character - as PDU or text generates a PDU / text length of zero.
 * @param version The version of this session.
 * 
 * @return true if it could be send.
 */
bool sendErrorPDU(int* fdPtr, RPKICommonHeader* pdu, char* reason, 
                  uint8_t version)
{
  // @TODO: Fix sendErrorPDU.
  printf("ERROR: [V:%u] invalid PDU because of %s\n", version, reason);
//  uint8_t                  pdu[sizeof(RPKIErrorReportHeader)];
//  RPKICacheResponseHeader* hdr;
//
//  // Create PDU
//  hdr = (RPKICacheResponseHeader*)pdu;
//  hdr->version   = RPKI_RTR_PROTOCOL_VERSION;
//  hdr->type      = (uint8_t)PDU_TYPE_CACHE_RESPONSE;
//  hdr->sessionID = htons(sessionID);
//  hdr->length    = htonl(sizeof(RPKICacheResetHeader));

//  OUTPUTF(true, "Sending a 'Cache Response'\n");
//  return sendNum(fdPtr, &pdu, sizeof(RPKICacheResetHeader));
  return false;
}

/**
 * Send an error report to all clients.
 *
 * @param fdPtr the socket connection
 * @param the error number to be send
 * @param data contains the error number followed by the PDU and text. The
 * character - as PDU or text generates a PDU / text length of zero.
 * @return true if it could be send.
 */
bool sendErrorReport(int* fdPtr, uint16_t errNo, char* data)
{
  // ERROR CODE
  RPKIErrorReportHeader* hdr;
  uint32_t     length = 16; // includes the basic 8 bytes header plus 8 bytes
                            // for length fields of encapsulated PDU and msg
                            // must be extended by length of err PDU and msg
                            // text

  // Erroneous PDU
  uint8_t      errPdu[sizeof(RPKIIPv6PrefixHeader)]; // for now MAX encaps. PDU
  char*        pduTok     = NULL;
  uint16_t     pduLen     = 0;
  memset(errPdu, 0, sizeof(RPKIIPv6PrefixHeader));  // initialize with zero

  // Error Message
  char*        msgTok     = NULL;
  uint16_t     msgLen     = 0;

  // Error Message
  bool         succ = true;
  CacheClient* cl;

  // determine the error number
  if (strlen(data) == 0)
  {
    ERRORF("Parameter missing! Can not generate Error PDU!");
    return false;
  }

  // Set the pointer for the pdu and message
//  pduTok = strtok(data, " ");
//  msgTok = strtok(NULL, " "); // Only used to figure out if a message was given
                              // If so it will not be NULL

  msgTok = data;
  pduTok = strsep(&msgTok, " ");


  if (pduTok == NULL)
  {
    ERRORF("Parameter for PDU missing, either PDU specification or - !");
    return false;
  }

  if (msgTok == NULL)
  {
    ERRORF("Parameter for message text missing; either a message or - !");
    return false;
  }

  // Check if an erroneous PDU is specified and if so generate it
  if (*pduTok != '-')
  { // A PDU is specified, add it and create the proper length
    RPKICommonHeader* chdr = (RPKICommonHeader*)errPdu;
    RPKISerialNotifyHeader* shdr;
    RPKIIPv4PrefixHeader* v4hdr;
    RPKIIPv6PrefixHeader* v6hdr;

    chdr->version = (uint8_t)atoi(strtok(pduTok, ","));
    chdr->type    = (uint8_t)atoi(strtok(NULL, ","));
    chdr->mixed   = htons((uint16_t)atoi(strtok(NULL, ",")));
    pduLen        = (uint32_t)atol(strtok(NULL, ","));
    chdr->length  = htonl(pduLen);

    switch (chdr->type)
    {
      case PDU_TYPE_RESET_QUERY:
      case PDU_TYPE_CACHE_RESPONSE:
      case PDU_TYPE_CACHE_RESET:
        break;
      case PDU_TYPE_SERIAL_NOTIFY:
      case PDU_TYPE_SERIAL_QUERY:
      case PDU_TYPE_END_OF_DATA:
        shdr = (RPKISerialNotifyHeader*)errPdu;
        shdr->serial = htonl((uint32_t)atol(strtok(NULL, ",")));
        break;
      case PDU_TYPE_IP_V4_PREFIX:
      case PDU_TYPE_IP_V6_PREFIX:
        v4hdr = (RPKIIPv4PrefixHeader*)errPdu;
        v6hdr = (RPKIIPv6PrefixHeader*)errPdu;
        IPv4Address v4addr;
        IPv6Address v6addr;
        // Here and below V4 and V6 share the same data structure. we fill v4
        // and V6 will be fileld as well.
        v4hdr->flags     = (uint8_t)atoi(strtok(NULL, ","));
        v4hdr->prefixLen = (uint8_t)atoi(strtok(NULL, ","));
        v4hdr->maxLen    = (uint8_t)atoi(strtok(NULL, ","));
        v4hdr->zero      = (uint8_t)atoi(strtok(NULL, ","));
        if (chdr->type == PDU_TYPE_IP_V4_PREFIX)
        {
          strToIPv4Address(strtok(NULL, ","), &v4addr);
          v4hdr->addr = v4addr;
          v4hdr->as   = htonl((uint32_t)atol(strtok(NULL, ",")));
        }
        else
        {
          strToIPv6Address(strtok(NULL, ","), &v6addr);
          v6hdr->addr = v6addr;
          v6hdr->as   = htonl((uint32_t)atol(strtok(NULL, ",")));
        }
        break;
      case PDU_TYPE_ERROR_REPORT:
      case PDU_TYPE_RESERVED:
        // Do Nothing here, leave as is
        break;
    }
  }

  // Now add the length of the error pdu.
  length += pduLen;

  // Check if a message is provided
  if (*msgTok != '-')
  {
    // increase the packet length by the message itself.
    msgLen = strlen(msgTok);
    length += msgLen;
  }

  uint8_t  pdu[length];
  uint32_t posPDU = 0;
  uint32_t posData = 0;
  memset(pdu, 0, length);
  hdr = (RPKIErrorReportHeader*)pdu;
  hdr->version      = RPKI_RTR_PROTOCOL_VERSION;
  hdr->type         = PDU_TYPE_ERROR_REPORT;
  hdr->error_number = htons(errNo);
  hdr->length       = htonl(length); // Length of complete PDU
  hdr->len_enc_pdu  = htonl(pduLen); // length of erroneous PDU
  // Fill the error PDU into the PDU
  for (posData = 0, posPDU=12; posData < pduLen; posPDU++, posData++)
  {
    pdu[posPDU] = errPdu[posData];
  }

  // Fill Text
  uint32_t msgLenField = htonl(msgLen);
  pdu[posPDU++] = (uint8_t)(msgLenField & 0xff);;
  pdu[posPDU++] = (uint8_t)(msgLenField >>  8 & 0xff);;
  pdu[posPDU++] = (uint8_t)(msgLenField >> 16 & 0xff);;
  pdu[posPDU++] = (uint8_t)(msgLenField >> 24 & 0xff);

  for(posData = 0; posData < msgLen; posData++)
  {
    pdu[posPDU++] = (uint8_t)msgTok[posData];
  }

  // Send
  if (HASH_COUNT(clients) > 0)
  {
    OUTPUTF(true, "Sending multiple 'Error Report' (Error = %hhu)\n", errNo);

    for (cl = clients; cl; cl = cl->hh.next)
    {
      if (!sendNum(&cl->fd, &pdu, length))
      {
        ERRORF("Error: Failed to send an 'Error Report'\n");
        succ = false;
        break;
      }
    }
  }

  return succ;
}

/**
 * This method is used to print the given error report.
 *
 * @param errNo The number of the error.
 * @param data The data of the report. (It's the pdy minus the first 8 byrtes.
 * @param dataLen the length of the report data.
 * @return
 */
bool printErrorReport(uint8_t errNo, void* data, uint32_t dataLen)
{
  uint32_t len;

  // Here the data len do mess the first 8 bytes of the PDU. They were read in
  // the common header.
  OUTPUTF(false, "Error report - error number: %hhu\n", errNo);

  // Encapsulated PDU
  len = ntohl(*((uint32_t*)data));
  if (len > 0)
  {
    if (len > dataLen)
    {
      ERRORF("Error: Not enough data (found: %u, expected: %u)\n",
                      dataLen, len);
      return false;
    }

    OUTPUTF(false, "Erroneous PDU:\n");
    dumpHex(stderr, data + 4, len);

    data += len + 4;
    dataLen -= len + 4;
  }

  // Error message
  if (dataLen < 4)
  {
    ERRORF("Error: 'Error Text Length' is missing\n");
    return false;
  }
  len = ntohl(*((uint32_t*)data));
  if (len > 0)
  {
    if (len > dataLen)
    {
      ERRORF("Error: Not enough text (found: %u, expected: %u\n",
                      dataLen, len);
      return false;
    }
    OUTPUTF(true, "Message: '%*s'\n", len, (char*)(data + 4));
  }

  return true;
}

// ClientConnectionAccepted
/**
 * Handle the data received from the client.
 *
 * @param svrSock The socket through which the data is received.
 * @param sock
 * @param user
 */
void handleClient(ServerSocket* svrSock, int sock, void* user)
{
  time_t           lastReq, diffReq;
  RPKICommonHeader hdr;
  uint32_t         remainingDataLentgh;
  void*            buf;
  CacheClient*     ccl = NULL;

  HASH_FIND_INT(clients, &sock, ccl);
  if (ccl == NULL)
  {
    ERRORF("Error: Cannot find client sessoin!\n");
    close(sock);
    return;
  }

  // Process client requests, store the current time
  lastReq = time(NULL);

  // read the beginning of the header to see how many bytes are actually needed
  while (recvNum(&sock, &hdr, sizeof(RPKICommonHeader)))
  {
    if (hdr.version > cache.version)
    {
      char buff[256];
      sprintf(buff, "Unsupported Version %u, expected %u", hdr.version, 
              RPKI_RTR_PROTOCOL_VERSION);
      printf("");
      sendErrorPDU(&ccl->fd, &hdr, buff, RPKI_RTR_PROTOCOL_VERSION);
      close(sock);
      break;          
    }
    if (ccl->version == UNDEF_VERSION)
    {
      ccl->version = hdr.version;
    } 
    else if (hdr.version != ccl->version)
    {
      // @TODO: Fix this and also close connection in this case.
      sendErrorPDU(&ccl->fd, &hdr, "Illegal switch of version number!", 
                   ccl->version);
      close(sock);
      break;
    }
    
    // determine the remaining data that needs to be received - if any
    remainingDataLentgh = ntohl(hdr.length) - sizeof(RPKICommonHeader);

    if (remainingDataLentgh > 0)
    {
      // allocate a buffer with the correct size to hold the remaining data
      buf = malloc(remainingDataLentgh);
      if (buf == NULL)
      {
        ERRORF("Error: Not enough memory to receive the data\n");
        close(sock);
        break;
      }

      // Read the remaining data
      if (!recvNum(&sock, buf, remainingDataLentgh))
      {
        ERRORF("Error: Failed to receive the data\n");
        close(sock);
        break;
      }
    }
    else
    {
      buf = NULL;
    }

    // Time since the last request
    diffReq = lastReq - time(NULL);

    OUTPUTF(true, "Received Data From Client [%x]...\n", sock);

    // Action depending on the type
    switch ((RPKIRouterPDUType)hdr.type)
    {
      case PDU_TYPE_SERIAL_QUERY:
        OUTPUTF(true, "[+%lds] Received a 'Serial Query'\n", diffReq);
        uint32_t serial;
        uint16_t sessionID;
        if (remainingDataLentgh != 4)
        {
          ERRORF("Error: Invalid 'Serial Query'\n");
          dumpHex(stderr, buf, remainingDataLentgh);
        }
        else
        {
          serial = ntohl(*((uint32_t*)buf));
          sessionID  = ntohs(hdr.mixed);
          sendCacheObjects(&sock, serial, sessionID, false, ccl->version);
        }
        break;

      case PDU_TYPE_RESET_QUERY:
        OUTPUTF(true, "[+%lds] Received a 'Reset Query'\n", diffReq);
        sendCacheObjects(&sock, 0, sessionID, true, ccl->version);
        break;

      case PDU_TYPE_ERROR_REPORT:
        printErrorReport(ntohs(hdr.mixed), buf, remainingDataLentgh);
        break;

      case PDU_TYPE_RESERVED:

      default:
        ERRORF("Error: Invalid PDU type: %hhu\n", hdr.type);
    }

    free(buf);

    // Time after processing the request
    lastReq = time(NULL);
  }
}

/**
 * Handles client session status
 *
 * @param svrSock The server socket that receives the data
 * @param client NOT USED
 * @param fd The file descriptor
 * @param connected Indicates if the connection will be established of shut down
 * @param user NOT USED
 *
 * @return false is an error occured.
 */
bool handleStatus(ServerSocket* svrSock, ServerClient* client, int fd,
                  bool connected, void* user)
{
  CacheClient* ccl = NULL;

  if (connected)
  {
    ccl = (CacheClient*)malloc(sizeof(CacheClient));
    if (ccl == NULL)
    {
      ERRORF("Error: Out of memory - rejecting client\n");
      return false;
    }
    memset(ccl, 0, sizeof(CacheClient));
    ccl->fd      = fd;
    ccl->version = UNDEF_VERSION;
    HASH_ADD_INT(clients, fd, ccl);
  }
  else
  {
    HASH_FIND_INT(clients, &fd, ccl);
    if (ccl != NULL)
    {
      HASH_DEL(clients, ccl);
      memset(ccl, 0, sizeof(CacheClient));
      ccl = NULL;
    }
    else
    {
      ERRORF("Error: Unknown client\n");
    }
  }
  return true;
}


void* handleServerRunLoop(void* _unused)
{
  LOG (LEVEL_DEBUG, "([0x%08X]) > RPKI Server Thread started!", pthread_self());

  runServerLoop (&svrSocket, MODE_CUSTOM_CALLBACK,
                 handleClient, handleStatus, NULL);

  LOG (LEVEL_DEBUG, "([0x%08X]) < RPKI Server Thread stopped!", pthread_self());

  pthread_exit(0);
}

////////////////////////////////////////////////////////////////////////////////
// Read the data from the file.
////////////////////////////////////////////////////////////////////////////////

/*----------------------
 * Prefix file functions
 */

int stripLineBreak(char* str)
{
  int pos = strlen(str) - 1;

  while (pos >= 0)
  {
    if ((str[pos] != 0xA) && (str[pos] != 0xD))
    {
      break;
    }
    str[pos--] = '\0';
  }
  return pos + 1;
}

/**
 * Read prefix data from a given file or from command line. An error while
 * reading from command line will result in skipping the line and posting a
 * WARNING. An error from command line results in abort of the operation.
 *
 * @param arg The filename or the data provided via command line.
 * @param dest
 * @param serial The serial number of the prefix announcement(s).
 * @param isFile determine if the argument given specifies a file or input data.
 *
 * @return true if the prefix(es) could be send.
 */
bool readPrefixData(const char* arg, SList* dest, uint32_t serial, bool isFile)
{
  #define NUM_FIELDS    3  // prefix max_len as

  FILE*          fh;
  int            lineNo;
  char           buf[LINE_BUF_SIZE];
  char*          bptr;
  int            idx;
  char*          fields[NUM_FIELDS];
  IPPrefix       prefix;
  uint32_t       maxLen;
  uint32_t       oas;
  ValCacheEntry* cEntry;
  bool           goOn=true;

  if (isFile)
  {
    fh = fopen(arg, "rt");
    if (fh == NULL)
    {
      ERRORF("Error: Failed to open '%s'\n", arg);
      return false;
    }
  }
  else
  {
    if (arg == NULL)
    {
      ERRORF("Error: Data missing: <prefix> <maxlen> <as>\n");
      return false;
    }
  }

  // Read line by line
  for (; goOn; lineNo++)
  {
    if (isFile)
    {
      goOn = fgets(buf, LINE_BUF_SIZE, fh);
      if (!goOn)
      {
        continue;
      }
    }
    else
    {
      // Stop after the one line.
      goOn = false;
      // here filename is not the name of the file, it contains the one and only
      // line of data. (Called by addPrefix);
      strncpy(buf, arg, LINE_BUF_SIZE);
    }

    // Skip comments
    if (buf[0] == '#')
    {
        continue;
    }

    // Make sure the line is not empty
    if (stripLineBreak(buf) == 0)
    {
        continue;
    }

    // Put into fields for later processing
    bptr = buf;
    idx  = 0;
    // FIX BZ164
    bool fieldIsNull = false;
    do
    {
      fields[idx] = strsep(&bptr, " \t");
      fieldIsNull = fields[idx] == NULL;

      if (fieldIsNull)
      {
        if (idx < NUM_FIELDS)
        {
          if (isFile)
          {
            ERRORF("ERROR: Line[%d] Parameters missing : '%s'\n", lineNo, buf);
          }
          else
          {
            ERRORF("ERROR: Parameters missing : '%s'\n"
                   "try Help for more information\n", buf);
          }
          return false;
        }
      }
      else if (fields[idx][0] == 0)
      {
        // To the else if block above:
        // It can happen that the buffer contains "      a.b.c.d/d   x   y   "
        // in this case the read field "fields[idx][0]" is zero for each list of
        // blanks. In this case read the next element in the buffer and don't
        // increase the idx, the field has to be refilled.
        continue;
      }

      idx++;
    } while (idx < NUM_FIELDS && !fieldIsNull);

    // Parse fields
    SKIP_IF(!strToIPPrefix(fields[0], &prefix),
            "Invalid IP Prefix", fields[0], isFile);

    maxLen = strtoul(fields[1], NULL, 10);
    SKIP_IF(!BETWEEN(maxLen, 0, GET_MAX_PREFIX_LEN(prefix.ip)),
            "Invalid max. length", fields[1], isFile);

    oas = strtoul(fields[2], NULL, 10);
    SKIP_IF(oas == 0,
            "Invalid origin AS", fields[2], isFile);

    // Append
    cEntry = (ValCacheEntry*)appendToSList(dest, sizeof(ValCacheEntry));
    if (cEntry == NULL)
    {
        fclose(fh);
      return false;
    }
    memset(cEntry, 0, sizeof(ValCacheEntry));
    cEntry->serial  = cEntry->prevSerial = serial++;
    cEntry->expires = 0;

    cEntry->flags           = PREFIX_FLAG_ANNOUNCEMENT;
    cEntry->prefixLength    = prefix.length;
    cEntry->prefixMaxLength = (uint8_t)maxLen;
    cEntry->isKey           = false;

    if (prefix.ip.version == 4)
    {
      cEntry->isV6 = false;
      memcpy(&cEntry->address.v4.in_addr, &prefix.ip.addr, 4);
      cEntry->asNumber = htonl(oas);

    }
    else
    {
      cEntry->isV6 = true;
      memcpy(&cEntry->address.v6.in_addr, &prefix.ip.addr, 16);
      cEntry->asNumber = htonl(oas);
    }
  }

  if (isFile)
  {
    fclose(fh);
  }
  return true;
}

/**
 * This helper function scans through the string and counts the number of tokens
 * separated by ' ' or '\t'
 * 
 * @param line the String to process
 * 
 * @return number of tokens found in the string.
 * 
 * @since 0.6.0.0
 */
int _countTokens(char* line)
{
  int  tokens = 0;
  bool isLastToken = false;
  int  idx = 0;
  int  length = 0;
  
  if (line != NULL)
  {
    length = strlen(line);
    for (; idx < length; idx++, line++)
    {
      switch (*line)
      {
        case ' ':
        case '\t':
          isLastToken = false;
          break;
        default:
          if (!isLastToken)
            tokens++;
          isLastToken = true;
      }
    }
  }
  return tokens;
}

/** 
 * Find the ASPA entry for the given customerAS
 * 
 * @param customerAS The ASN of the customer for whom the cache entry is
 *                   requested.
 * 
 * @return Pointer to the cache entry (ValCacheEntry) or NULL is not found.
 * 
 * @since 0.6.2.1
 */
ValCacheEntry* findASPA(uint32_t customerAS)
{
  ValCacheEntry* retVal = NULL;
  ValCacheEntry* cEntry = NULL;
  SListNode*     lnode  = NULL;
  // it is faster to translate this into network representation once rather than
  // translating each stored value into host representation.
  uint32_t       cAS    = htonl(customerAS);
  
  if (sizeOfSList(&cache.entries) != 0)
  {
    FOREACH_SLIST(&cache.entries, lnode)
    {
      cEntry = (ValCacheEntry*)getDataOfSListNode(lnode);
      if (cEntry->isASPA)
      {
        if (cAS == cEntry->asNumber)
        {
          retVal = cEntry;
          break;
        }
      }
    }
  }

  return retVal;
}

/**
 * Read ASPA data from a given file or from command line. An error while
 * reading from command line will result in skipping the line and posting a
 * WARNING. An error from command line results in abort of the operation.
 *
 * @param arg The filename or the data provided via command line.
 * @param dest The list where the data will be stored in.
 * @param serial The serial number of the ASPA announcement(s).
 * @param isFile determine if the argument given specifies a file or input data.
 *
 * @return true if the ASPA data is added.
 */
bool readASPAData(const char* arg, SList* dest, uint32_t serial, bool isFile)
{
//  #define NUM_FIELDS    3  // prefix max_len as
  int num_fields = (arg != NULL) ? _countTokens((char*)arg) : 0;

  FILE*          fh;
  int            lineNo;
  char           buf[LINE_BUF_SIZE];
  char*          bptr;
  int            idx;
  char*          fields[num_fields];
  
  uint32_t       previousSerial;
  uint32_t       numAdded = 0;

  uint32_t       customerAS, pAS;
  uint16_t       providerCount = 0;
  uint8_t*       providerBuff  = NULL;
  uint32_t*      providerAS    = NULL;
  int providerCounter;
  providerBuff = NULL;
  providerAS   = NULL;
  
  ValCacheEntry* cEntry;
  bool           goOn=true;

  if (isFile)
  {
    fh = fopen(arg, "rt");
    if (fh == NULL)
    {
      ERRORF("Error: Failed to open '%s'\n", arg);
      return false;
    }
  }
  else
  {
    if (arg == NULL)
    {
      ERRORF("Error: Data missing: <customer-as> <provider-as> [<provider-as>*]\n");
      return false;
    }
  }

  // Read line by line
  for (; goOn; lineNo++)
  {
    if (isFile)
    {
      goOn = fgets(buf, LINE_BUF_SIZE, fh);
      if (!goOn)
      {
        continue;
      }
    }
    else
    {
      // Stop after the one line.
      goOn = false;
      // here filename is not the name of the file, it contains the one and only
      // line of data. (Called by addPrefix);
      strncpy(buf, arg, LINE_BUF_SIZE);
    }

    // Skip comments
    if (buf[0] == '#')
    {
        continue;
    }

    // Make sure the line is not empty
    if (stripLineBreak(buf) == 0)
    {
        continue;
    }

    // Put into fields for later processing
    bptr = buf;
    idx  = 0;
    // FIX BZ164
    bool fieldIsNull = false;
    do
    {
      fields[idx] = strsep(&bptr, " \t");
      fieldIsNull = fields[idx] == NULL;

      if (fieldIsNull)
      {
        if (idx < NUM_FIELDS)
        {
          if (isFile)
          {
            ERRORF("ERROR: Line[%d] Parameters missing : '%s'\n", lineNo, buf);
          }
          else
          {
            ERRORF("ERROR: Parameters missing : '%s'\n"
                   "try Help for more information\n", buf);
          }
          return false;
        }
      }
      else if (fields[idx][0] == 0)
      {
        // To the else if block above:
        // It can happen that the buffer contains "      a.b.c.d/d   x   y   "
        // in this case the read field "fields[idx][0]" is zero for each list of
        // blanks. In this case read the next element in the buffer and don't
        // increase the idx, the field has to be refilled.
        continue;
      }

      idx++;
    } while (idx < num_fields && !fieldIsNull);

    // Parse fields
    customerAS = strtoul(fields[0], NULL, 10);
    SKIP_IF(customerAS == 0,
            "Invalid Customer AS", fields[0], isFile);

    if(providerBuff != NULL)
    {
      // RElease the memory from the last round.
      free(providerBuff);
      providerBuff = NULL;
      providerAS   = NULL; 
    }
    
    int errorCode = (idx > 1) ? 0 : 1;
    // 1 : NO provider specified
    // 2 : CUstomer also specified as provider
    if (errorCode == 0)
    {
      providerBuff = malloc((idx-1) * 4);
      memset(providerBuff, 0, (idx-1) * 4);
      providerAS = (uint32_t*)providerBuff;
      providerCount=idx-1;
      if (providerCount != 0)
      {
        for (providerCounter = 1; providerCounter < idx; 
             providerCounter++, providerAS++)
        {
          pAS = strtoul(fields[providerCounter], NULL, 10);
          *providerAS = htonl(pAS);
          if (customerAS == pAS)
          {
            errorCode = 2;
            break;
          }
        }
      }
      else
      {
        errorCode = 1;
      }
    }

    // 
    if (errorCode != 0)
    {
      free(providerBuff);
      providerBuff = NULL;
      providerAS   = NULL;
      switch(errorCode)
      {
        case 1: SKIP_IF(true, "At least one provider is required!", 
                              "Use 'remove #' to delete ASPA!", isFile);
                break;
        case 2: SKIP_IF(true, "Provider and Customer must not be same!", 
                              "Customer cannot be it's own provider!", isFile);
                break;
        default: SKIP_IF(true, "Unknown Error!", 
                              "UNKNOWN ERROR WHILE PROCESSING addASPA!",isFile);
                break;
      }
    }
    
    // Check if the cache already contains an ASPA object for this particular
    // clientAS. In this case remove it directly from the cache, do not call 
    // removeASPA.
    previousSerial = 0;
    cEntry = findASPA(customerAS);
    //cEntry = NULL;
    if (cEntry != NULL)
    {
      LOG(LEVEL_INFO, "Found previous ASPA object with ASN %u, remove object"
                      " from cache to install replacement!", customerAS);
      previousSerial = cEntry->serial;
      deleteFromSList(&cache.entries, cEntry);
      free(cEntry->providerAS);
      cEntry->providerCount = 0;
      cEntry->providerAS    = NULL;
      free(cEntry);
      cEntry = NULL;
    }

    // Append
    cEntry = (ValCacheEntry*)appendToSList(dest, sizeof(ValCacheEntry));
    if (cEntry == NULL)
    {
      fclose(fh);
      if (providerBuff != NULL)
      {
        free(providerBuff);
      }
      providerBuff = NULL;
      providerAS = NULL;
      return false;
    }
    
    // Reset cEntry value to all zero
    memset(cEntry, 0, sizeof(ValCacheEntry));
            
    cEntry->serial  = serial++;
    cEntry->prevSerial = previousSerial != 0 ? previousSerial : cEntry->serial;
    cEntry->expires = 0; // Not needed , it is 0 already from above

    cEntry->flags           = PREFIX_FLAG_ANNOUNCEMENT;
    cEntry->prefixLength    = 0;
    cEntry->prefixMaxLength = 0;
    cEntry->isASPA          = true;
    cEntry->asNumber        = htonl(customerAS);
    cEntry->providerCount   = htons(providerCount);
    cEntry->providerAS      = providerBuff;
    numAdded++;
    providerBuff = NULL;
    providerAS   = NULL;
  }

  if (isFile)
  {
    fclose(fh);
  }
  return numAdded;
}

/**
 * Display or generate a session id.
 *
 * @param argument if NULL display the current cache session id otherwise use
 *                 value to generate new once.
 *
 * @return CMD_ID_SESSID
 */
int processSessionID(char* argument)
{
  if (argument == NULL)
  {
    printf("Current SESSION ID: %d (0x%04X)\n", sessionID, sessionID);
  }
  else
  {
    uint16_t newSessionID = atoi(argument);
    if (newSessionID == 0) // it is zero for both, 0 as well as text
    {
      if (strcmp(argument, "reset") == 0)
      {
        printf ("Reset session id to 0\n");
        processSessionID(NULL);
        sendSerialNotifyToAllClients();
      }
      else
      {
        printf("ERROR: New SESSION ID '%s' is not a number!\n", argument);
      }
    }
    else
    {
      if (newSessionID < sessionID)
      {
        printf("ERROR: New SESSION ID %d must be greater than current SESSION "
               "IS %d!\n", newSessionID, sessionID);
      }
      else
      {
        // initiate a serial request from the clients. This will result in a
        // session id error.
        sessionID = newSessionID;
        processSessionID(NULL);
        sendSerialNotifyToAllClients();
      }
    }
  }

  return CMD_ID_SESSID;
}

/*----------------
 * Single commands
 */

/**
 * Display the version information.
 *
 * @return CMD_ID_VERSION
 */
int showVersion()
{
#ifndef BRIO
  printf("%s Version %s\n", RPKI_RTR_SRV_NAME, RPKI_RTR_SRV_VER);
#else
  printf("%s Version %s\n", BRIO_RPKI_CACHE_NAME, BRIO_RPKI_CACHE_VER);
#endif

  return CMD_ID_VERSION;
}


/**
 * Display the command help
 *
 * @return CMD_ID_HELP
 */
int showHelp(char* command)
{
  if (command == NULL)
  {
    showVersion();
    printf("\nDisplay Commands:\n"
           "-----------------\n"
           "  - verbose\n"
           "                 Turns verbose output on or off\n"
           "  - cache\n"
           "                 Lists the current cache's content\n"
           "  - version\n"
           "                 Displays the version of this tool!\n"
           "  - sessionID\n"
           "                 Display the current session id\n"
           "  - help [command]\n"
           "                 Display this screen or detailed help for the\n"
           "                 given command!\n"
           "  - credits\n"
           "                 Display credits information!\n"
           "\n"
           "Cache Commands:\n"
           "-----------------\n"
           "  - keyLoc <location>\n"
           "                 The key volt location.\n"
           "  - empty\n"
           "                 Empties the cache\n"
           "  - sessionID <number>\n"
           "                 Generates a new session id.\n"
           "  - append <filename>\n"
           "                 Appends a prefix file's content to the cache\n"
           "  - add <prefix> <maxlen> <as>\n"
           "                 Manually add a whitelist entry\n"
           "  - addNow <prefix> <maxlen> <as>\n"
           "                 Manually add a whitelist entry without any \n"
           "                 delay!\n"
           "  - addKey <as> <cert file>\n"
           "                 Manually add a RPKI Router Certificate\n"
           "  - addASPA <customer-as> <provider-as> [<provider-as>*]\n"
           "                 Manually add an ASPA object to the cache\n"
           "  - addASPANow <customer-as> <provider-as> [<provider-as>*]\n"
           "                 Manually add an ASPA object to the cache without\n"
           "                 any delay!\n"
           "  - remove <index> [end-index]\n"
           "                 Remove one or more cache entries\n"
           "  - removeNow <index> [end-index]\n"
           "                 Remove one or more cache entries without any\n"
           "                 delay!\n"
           "  - error <code> <pdu|-> <message|->\n"
           "                 Issues an error report. The pdu contains all\n"
           "                 real fields comma separated.\n"
           "  - notify\n"
           "                 Send a SERIAL NOTIFY to all clients.\n"
           "  - reset\n"
           "                 Send a CACHE RESET to all clients.\n"
           "  - echo [text]\n"
           "                 Print the given text on the console window.\n"
           "  - waitFor <client-IP>\n"
           "                 Wait until the client with the given IP connects.\n"
           "                 This function times out after 60 seconds.\n"
           "  - pause [prompt]\n"
           "                 Wait until any key is pressed. This is mainly\n"
           "                 for scripting scenarios. If no prompt is used,\n"
           "                 the default prompt will be applied!\n"

           "\n"
           "Program Commands:\n"
           "-----------------\n"
           "  - quit, exit, \\q\n"
           "                 Quits the loop and terminates the server.\n"
           "                 This command is allowed within scripts but only\n"
           "                 as the very last command, otherwise it will be\n"
           "                 ignored!\n"
           "  - clients\n"
           "                 Lists all clients\n"
           "  - run <filename>\n"
           "                 Executes a file line-by-line\n"
           "  - sleep <seconds>\n"
           "                 Pauses execution\n"
           "\n\n");
  }
  else
  {
    #define SHOW_CMD_HLP(CMD, TXT)      \
        printf ("\nCommand: " CMD ":\n"); \
        printf ("-----------------------------------------------------\n"); \
        printf (TXT "\n\n");

    if (strcmp(command, "empty")==0)
    {
      SHOW_CMD_HLP("empty",
                   "This command cleans the complete cache. No message will be"
                   " send to the attached clients."
        );
    }
    else if (strcmp(command, "sessionID")==0)
    {
      SHOW_CMD_HLP("sessionID <number>",
                   "Depending if a number is provided or not the function "
                   "performs a different function.\n"
                   "- If no number is provided, the current cache sessionID "
                   "value will be displayed.\n"
                   "- If a number os provided the cache changes its internal "
                   "cache session id to the given number. In this "
                   "implementation the number can only grow. Once the number "
                   "is changed, a SERIAL NOTIFY message will be send to all "
                   "clients attached.\n"
                   "This might result in an earlier SERIAL REQUEST than the "
                   "client would otherwise do. As result a CACHE SESSION ID "
                   "error will occur on the client side. This SHOULD result in "
                   "a RESET QUERY from the client."
        );
    }
    else if (strcmp(command, "notify")==0)
    {
      SHOW_CMD_HLP("notify",
                   "Send a SERIAL NOTIFY to all clients right away."
      );
    }
    else if (strcmp(command, "error")==0)
    {
      SHOW_CMD_HLP("error <code> <pdu|-> <message|->",
                   "  code    ::= an error code according to the draft "
                                  "\"10. Error Codes\"\n"
                   "              0: Corrupt Data (fatal)\n"
                   "              1: Internal Error (fatal)\n"
                   "              2: No data Available.\n"
                   "              3: Invalid Request (fatal)\n"
                   "              4: Unsupported Protocol Version (fatal)\n"
                   "              5: Unsupported PDU type (fatal)\n"
                   "              6: Withdrawal of Unknown Record (fatal)\n"
                   "  pdu     ::= The pdu to encapsulate in the error message.\n"
                   "              Each field is comma separated and will be parsed "
                                  "according to its type.\n"
                   "              Use \"-\" to not include a pdu.\n"
                   "  message ::= A text message wrapped in quote marks.\n"
                   "              Use \"-\" to not include a message.\n"
      );
    }
    else if (strcmp(command, "echo")==0)
    {
      SHOW_CMD_HLP("echo [text]",
                   "This command allows to display a given text on the console "
                   "window. It is mainly useful for scripted scenarios, where "
                   "follow up actions by the user is needed or where the "
                   "it makes sense to inform about the script process.\n"
      );
    }
    else if (strcmp(command, "waitFor")==0)
    {
      SHOW_CMD_HLP("waitFor <client-IP>",
                   "This command waits for a client to connect but will time "
                   "out after 60 seconds and writes a timeout statement on the "
                   "console.\n"
      );
    }
    else if (strcmp(command, "pause")==0)
    {
      SHOW_CMD_HLP("pause [prompt]",
                   "This command is for scripting scenarios the have user "
                   "interactions to continue. For instance CTRL+C will "
                   "further loading or deleting of cache entries. In case "
                   "no prompt is provided the default prompt is used.\n"
      );
    }
    else
    {
      printf ("No detailed help for '%s' available - use standard help!\n",
              command);
    }
  }
  return CMD_ID_HELP;
}

/**
 * Display the credits of the program
 *
 * @return CMD_ID_CREDITS
 */
int showCredits()
{
  showVersion();
  printf(SRX_CREDITS);

  return CMD_ID_CREDITS;
}

/**
 * Turn Verbose mode on or off.
 *
 * @return CMD_ID_VERBOSE
 */
int toggleVerboseMode()
{
  verbose ^= true;
  printf("Verbose output: %s\n", verbose ? "on" : "off");
  return CMD_ID_VERBOSE;
}

/**
 * This function does the real work of adding the prefix to the cache test
 * harness. It will be called in both modes, file and console.
 *
 * @param arg Can be a filename (file) or cache entry (console)
 * @param fromFile specified the type of "arg"
 *
 * @return true if the cache entrie(s) could be added.
 */
bool appendPrefixData(char* arg, bool fromFile)
{
  size_t  numBefore, numAdded;
  bool    succ;

  acquireReadLock(&cache.lock);
  numBefore = sizeOfSList(&cache.entries);

  changeReadToWriteLock(&cache.lock);
  succ = readPrefixData(arg, &cache.entries, cache.maxSerial + 1, fromFile);
  changeWriteToReadLock(&cache.lock);

  // Check how many entries were added
  numAdded = succ ? (sizeOfSList(&cache.entries) - numBefore) : 0;
  cache.maxSerial += numAdded;
  unlockReadLock(&cache.lock);

  OUTPUTF(false, "Read %d Prefix entr%s\n", (int)numAdded,
          (numAdded != 1 ? "ies" : "y"));

  // Send notify at least one entry was added
  if (numAdded > 0)
  {
    service.notify = true;
  }

  return succ;
}


#define CHAR_CONV_CONST     0x37
#define DIGIT_CONV_CONST    0x30
#define LEN_BYTE_NIBBLE     0x02

unsigned char hex2bin_byte(char* in)
{
  unsigned char result=0;
  int i=0;
  for(i=0; i < LEN_BYTE_NIBBLE; i++)
  {
    if(in[i] > 0x40)
      result |= ((in[i] - CHAR_CONV_CONST) & 0x0f) << (4-(i*4));
    else if(in[i] > 0x30 && in[i] < 0x40)
      result |= (in[i] - DIGIT_CONV_CONST) << (4-(i*4));
  }
  return result;
}

/**
 * This function loops through the list of clients and checks if one client
 * matches the given IP address.
 * 
 * @param clientIP The IP address the clients will be compared too.
 * 
 * @return true if one client exists with the given IP address.
 * 
 * @since 0.5.1.0 
 */
bool hasClient(char* clientIP)
{
  #define BUF_SIZE (MAX_IP_V6_STR_LEN + 6)
  char*        buf = malloc(BUF_SIZE);
  char*        ipStr1 = malloc(BUF_SIZE);
  char*        ipStr2 = malloc(BUF_SIZE);
  CacheClient* cl;
  bool         retVal = false;

  if ((clientIP != NULL) && (HASH_COUNT(clients) != 0))
  {
    cl = clients;
    snprintf(ipStr1, BUF_SIZE, "%s:", clientIP);
    while (cl != NULL)
    {
      socketToStr(cl->fd, true, buf, BUF_SIZE);
      snprintf(ipStr2, strlen(ipStr1)+1, "%s:", buf);
      // ==0 because the client also has the port number attached.
      retVal = strcmp(ipStr1, ipStr2) == 0;
      cl = retVal ? cl=NULL : cl->hh.next;
    }
  }

  free (buf);
  free (ipStr1);
  free (ipStr2);
  return retVal;
}

/**
 * Read the router key certificate file.
 *
 * @param arg the arguments (asn algoid certFile)
 * @param dest The list where to store it in
 * @param serial The serial number
 *
 * @return true if the cert could be read or not.s
 */
bool readRouterKeyData(const char* arg, SList* dest, uint32_t serial)
{

  char  buffKey[KEY_BIN_SIZE];
  char  buffSKI_asc[SKI_LENGTH * 2];
  char  buffSKI_bin[SKI_LENGTH];
  uint16_t keyLength, skiLength;
  FILE*   fpKey;
  ValCacheEntry* cEntry;

  char           streamBuf[COMMAND_BUF_SIZE];
  char*          bptr;

  if (arg == NULL)
  {
    ERRORF("Error: Data missing: <as> <cert file>\n");
    return false;
  }
  strncpy(streamBuf, arg, COMMAND_BUF_SIZE);
  stripLineBreak(streamBuf);
  bptr = streamBuf;

  char* asnStr     = strsep(&bptr, " \t");
  char* _certFile  = strsep(&bptr, " \t");

  if (_certFile == NULL)
  {
    ERRORF("Error: Data missing: <as> <cert file>\n");
    return false;
  }

  char certFile[512];
  snprintf(certFile, 512, "%s/%s", keyLocation, _certFile);

  // to read certificate file
  fpKey = fopen (certFile, "rb");
  if (fpKey == NULL)
  {
    ERRORF("Error: Failed to open '%s'\n", certFile);
    return false;
  }
  // to read a certificate and
  // parsing pubkey part and SKI
  //
  fseek(fpKey, OFFSET_PUBKEY, SEEK_SET);
  keyLength = (uint16_t)fread (&buffKey, sizeof(char), KEY_BIN_SIZE, fpKey);

  if (keyLength != KEY_BIN_SIZE)
  {
    ERRORF("Error: Failed to read, key size mismatch\n");
    return false;
  }

  fseek(fpKey, OFFSET_SKI, SEEK_SET);
  // read two times of SKI_SIZE(20 bytes) because of being written in a way of ASCII
  skiLength = (uint16_t)fread(&buffSKI_asc, sizeof(char), SKI_LENGTH * 2,
                               fpKey);

  int idx;
  for(idx = 0; idx < SKI_LENGTH; idx++)
  {
    buffSKI_bin[idx] = hex2bin_byte(buffSKI_asc+(idx*2));
  }

  // new instance to append
  cEntry = (ValCacheEntry*)appendToSList(dest, sizeof(ValCacheEntry));

  if (cEntry == NULL)
  {
    fclose(fpKey);
    return false;
  }
  // Make sure the entry object is properly initialized.
  memset(cEntry, 0, sizeof(ValCacheEntry));

  cEntry->serial          = cEntry->prevSerial = serial++;
  cEntry->flags           = PREFIX_FLAG_ANNOUNCEMENT;
  cEntry->isKey           = true;

  cEntry->asNumber    = htonl(strtoul(asnStr, NULL, 10));
  cEntry->ski         = (char*) calloc(1, SKI_LENGTH);
  cEntry->pPubKeyData = (char*) calloc(1, KEY_BIN_SIZE);

  memcpy(cEntry->ski, buffSKI_bin, SKI_LENGTH);
  memcpy(cEntry->pPubKeyData, buffKey, KEY_BIN_SIZE);

  fclose(fpKey);

  return true;
}

/**
 * Append the given public router key to the cache.
 *
 * @param line the arguments line.
 *
 * @return true if the key could be added.
 */
bool appendRouterKeyData(char* line)
{

  size_t  numBefore, numAdded;
  bool    succ;

  acquireReadLock(&cache.lock);
  numBefore = sizeOfSList(&cache.entries);

  changeReadToWriteLock(&cache.lock);

  // function for certificate reading
  succ = readRouterKeyData(line, &cache.entries, cache.maxSerial+1);

  changeWriteToReadLock(&cache.lock);

  numAdded = succ ? (sizeOfSList(&cache.entries) - numBefore) : 0;
  cache.maxSerial += numAdded;
  unlockReadLock(&cache.lock);

  OUTPUTF(false, "Read %d Router Key%s entry\n", (int)numAdded,
          numAdded != 1 ? "s" : "");

  if (numAdded > 0)
  {
    service.notify = true;
  }
  return succ;
}

/**
 * This function does the real work of adding the ASPA data to the cache test
 * harness. It will be called in both modes, file and console.
 *
 * @param arg Can be a filename (file) or cache entry (console)
 * @param fromFile specified the type of "arg"
 *
 * @return true if the cache entrie(s) could be added.
 */
bool appendASPAData(char* arg, bool fromFile)
{
  size_t  numAdded;

  acquireReadLock(&cache.lock);

  changeReadToWriteLock(&cache.lock);
  numAdded = readASPAData(arg, &cache.entries, cache.maxSerial + 1, fromFile);
  cache.maxSerial += numAdded;
  changeWriteToReadLock(&cache.lock);

  unlockReadLock(&cache.lock);

  OUTPUTF(false, "Read %d ASPA object%s\n", (int)numAdded,
          (numAdded != 1 ? "s" : "y"));

  // Send notify at least one entry was added
  if (numAdded > 0)
  {
    service.notify = true;
  }

  return numAdded > 0;
}

/**
 * This is a helper function for the append functions. It does create a proper
 * error printout.
 * 
 * @param type The append function type
 * @param line The command line passed to the append function
 * 
 * @since 0.6.0.0
 */
void _printAppendError(char* type, char* line)
{
  if (line != NULL)
  {
    printf ("Error: The %s '%s' could not be added to the "
            "cache\n", type, line);
  }
  else
  {
    printf ("Error: Cache not modified!!\n", line);      
  }  
}

/**
 * This method adds the RPKI cache entry into the test harness. The format
 * is IP/len max AS. 
 * 
 * The serial notify notification will be send out to all attached clients right
 * away if now==true
 *
 * @param line The command line
 * @param now If true a serialNotify will be send immediately
 *
 * @return CMD_ID_ADDNOW or CMD_ID_ADD or CMD_ERROR
 * 
 * @since 0.6.0.0
 */
int _appendPrefix(char* line, bool now)
{
  if (!appendPrefixData(line, false))
  {
    _printAppendError("prefix information", line);
  }
  else if (now)
  {
    sendSerialNotifyToAllClients();    
  }
  return line != NULL ? (now ? CMD_ID_ADDNOW : CMD_ID_ADD) : CMD_ERROR;
}

/**
 * This method adds the RPKI cache entry into the test harness. The format
 * is IP/len max AS
 *
 * @param line the cache entry.
 *
 * @return CMD_ID_ADD
 */
int appendPrefix(char* line)
{
  return _appendPrefix(line, false);
}

/**
 * This method adds the RPKI cache entry into the test harness. The format
 * is IP/len max AS. this method does not wait for the notification timer to
 * expire. The notification will be send out to all attached clients right away.
 *
 * @param line the cache entry.
 *
 * @return CMD_ID_ADDNOW
 */
int appendPrefixNow(char* line)
{
  return _appendPrefix(line, true);
}

/**
 * Append the key cert to the cache.
 *
 * The serial notify notification will be send out to all attached clients right
 * away if now==true
 * 
 * @param line The command line
 * @param now If true a serialNotify will be send immediately
 *
 * @return CMD_ID_ADDNOW or CMD_ID_add or CMD_ERROR
 * 
 * @since 0.6.0.0
 */
int _appendRouterKey(char* line, bool now)
{
  if (!appendRouterKeyData(line))
  {
    _printAppendError("key cert", line);
  }
  else if (now)
  {
    sendSerialNotifyToAllClients();
  }

  return line != NULL ? (now ? CMD_ID_ADDNOW : CMD_ID_ADD) : CMD_ERROR;
}

/**
 * Append the key cert to the cache.
 *
 * @param line The command line
 *
 * @return CMD_ID_ADD
 */
int appendRouterKey(char* line)
{
  return _appendRouterKey(line, false);
}

/**
 * Append the key cert to the cache.
 *
 * @param line The command line
 *
 * @return CMD_ID_ADDNOW
 */
int appendRouterKeyNow(char* line)
{
  return _appendRouterKey(line, true);
}

/*
 * This method adds the ASPA cache entry into the test harness. The format
 * is <customer-AS> <provider-AS> [ <provider-AS>*] 
 * 
 * The serial notify notification will be send out to all attached clients right
 * away if now==true
 *
 * @param line The command line
 * @param now If true a serialNotify will be send immediately
 *
 * @return CMD_ID_ADD or CMD_ID_ADDNOW or CMD_ERROR
 * 
 * @since 0.6.0.0
 */
int _appendASPA(char* line, bool now)
{
  if (!appendASPAData(line, false))
  {
    _printAppendError("ASPA object", line);
  }
  else if (now)
  {
    sendSerialNotifyToAllClients();
  }

  return line != NULL ? (now ? CMD_ID_ADDNOW : CMD_ID_ADD) : CMD_ERROR;
}

/**
 * This method adds the ASPA cache entry into the test harness. The format
 * is <customer-AS> <provider-AS> [ <provider-AS>*] 
 *
 * @param line The command line
 *
 * @return CMD_ID_ADD
 */
int appendASPA(char* line)
{
  return _appendASPA(line, false);
}

/**
 * This method adds the ASPA cache entry into the test harness. The format
 * is <customer-AS> <provider-AS> [ <provider-AS>*] 
 * The notification will be send out to all attached clients right away.
 *
 * @param line The command line
 *
 * @return CMD_ID_ADDNOW
 */
int appendASPANow(char* line)
{
  return _appendASPA(line, true);
}

/**
 * Set the key location
 *
 * @param line the location where the keys are stored (if null the key location
 *             will be removed.)
 *
 * @return CMD_ID_KEY_LOC
 */
int setKeyLocation(char* line)
{
  if (line == NULL)
  {
    line = ".\0";
  }
  snprintf(keyLocation, LINE_BUF_SIZE, "%s", line);

  return CMD_ID_KEY_LOC;
}

/**
 * Append the given prefix information in the given file.
 *
 * @param fileName the filename containing the prefix information
 *
 * @return CMD_ID_AD or CMD_ERROR
 */
int appendPrefixFile(char* fileName)
{
  int retVal = CMD_ID_ADD;
  
  if (!appendPrefixData(fileName, true))
  {
    printf("Error appending prefix information of '%s'\n", fileName);
    retVal = CMD_ERROR;
  }

  return retVal;
}

/**
 * Append the given ASPA information in the given file.
 *
 * @param fileName the filename containing the prefix information
 *
 * @return CMD_ID_AD or CMD_ERROR
 * 
 * @since 0.6.0.0
 */
int appendASPAFile(char* fileName)
{
  int retVal = CMD_ID_ADD;

  if (!appendASPAData(fileName, true))
  {
    printf("Error appending ASPA information of '%s'\n", fileName);
    retVal = CMD_ERROR;
  }

  return retVal;
}

/**
 * Clear the cache without sending a notify.
 *
 * @return CMD_ID_EMPTY
 */
int emptyCache()
{
  acquireWriteLock(&cache.lock);
  emptySList(&cache.entries);
  unlockWriteLock(&cache.lock);

  OUTPUTF(true, "Emptied the cache\n");

  return CMD_ID_EMPTY;
}

/**
 * Print the content of the cache test harness to the console.
 *
 * @return CMD_ID_CACHE
 */
int printCache()
{
  #define IPBUF_SIZE   MAX_IP_V6_STR_LEN

  time_t      now;
  SListNode*  lnode;
  unsigned    pos = 1;
  ValCacheEntry* cEntry;
  char        ipBuf[IPBUF_SIZE];
  int         idx=0;
  uint32_t*   ptr32;

  now = time(NULL);

  acquireReadLock(&cache.lock);
  printf("Session ID: %u (0x%04X)\n", sessionID, sessionID);
  if (sizeOfSList(&cache.entries) == 0)
  {
    printf("Cache is empty\n");
  }
  else
  {
    FOREACH_SLIST(&cache.entries, lnode)
    {
      cEntry = (ValCacheEntry*)getDataOfSListNode(lnode);

      printf("%c %4u: ",
             ((cEntry->flags & PREFIX_FLAG_ANNOUNCEMENT) ? ' ' : '*'), pos++);

      if (cEntry->isKey)
      {
        printf("[Key]:  SKI=");
        for (idx = 0; idx < SKI_LENGTH; idx++)
        {
          printf ("%02X", (uint8_t)cEntry->ski[idx]);
        }
        printf (", OAS=%u", ntohl(cEntry->asNumber));
      }
      else if (cEntry->isASPA)
      {
        printf ("[ASPA]: CAS=%u", ntohl(cEntry->asNumber));
        
        ptr32 = (uint32_t*)cEntry->providerAS;
        for (idx = 0; idx < ntohs(cEntry->providerCount); idx++, ptr32++)
        {
          printf (", PAS=%u", ntohl(*ptr32));
        }
      }
      else
      {
        printf("[ROA]:  ");
        if (cEntry->isV6)
        {
          printf("%s/%hhu, OAS=%u",
                 ipV6AddressToStr(&cEntry->address.v6, ipBuf, IPBUF_SIZE),
                 cEntry->prefixLength, ntohl(cEntry->asNumber));
        }
        else
        {
          printf("%s/%hhu, OAS=%u",
                 ipV4AddressToStr(&cEntry->address.v4, ipBuf, IPBUF_SIZE),
                 cEntry->prefixLength, ntohl(cEntry->asNumber));
        }
        printf(", Max.Len=%hhu", cEntry->prefixMaxLength);
      }
      
      printf(", Serial=%u, Prev.Serial=%u", cEntry->serial, cEntry->prevSerial);

      if (cEntry->expires > 0)
      {
        printf(" - Expires=%lds", (cEntry->expires - now));
      }

      printf("\n");
    }
  }
  unlockReadLock(&cache.lock);

  return CMD_ID_CACHE;
}

/**
 * Remove the specified entries. Format: "start [end]"
 *
 * @param arg list of entry-id's to be deleted from the cache.
 *
 * @return true if the entries could be removed.
 */
bool processEntryRemoval(char* arg)
{
  int            startIndex, endIndex, currPos;
  char*          aptr;
  ValCacheEntry* currEntry;
  SListNode*     prevNode, *currIndex;
  time_t         tsExp;
  int            removed = 0;

  if (arg == NULL)
  {
    ERRORF("Error: No indexes given\n");
    return false;
  }

  // Parse start and end-index string
  startIndex = strtoul(arg, &aptr, 10);
  if (aptr == arg)
  {
    ERRORF("Error: Index is not a number: '%s'\n", arg);
    return false;
  }
  if (*aptr != '\0')
  {
    endIndex = strtoul(aptr, NULL, 10);
  }
  else
  {
    endIndex = startIndex;
  }

  // Within bounds
  acquireReadLock(&cache.lock);
  if (   !BETWEEN(startIndex, 1, sizeOfSList(&cache.entries))
      || !BETWEEN(endIndex, startIndex, sizeOfSList(&cache.entries)))
  {
    unlockReadLock(&cache.lock);
    ERRORF("Error: Invalid index(es): '%s'\n", arg);
    return false;
  }

  // When removed entries expire
  tsExp = time(NULL) + CACHE_EXPIRATION_INTERVAL;

  // Go over the list
  changeReadToWriteLock(&cache.lock);

  prevNode  = (startIndex == 1)
              ? NULL
              : getNodeFromSList(&cache.entries, startIndex - 2);

  for (currPos = startIndex; currPos <= endIndex; currPos++)
  {
    currIndex = (prevNode == NULL)
                ? getNodeFromSList(&cache.entries, startIndex - 1)
                : getNextNodeOfSListNode(prevNode);

    currEntry = (ValCacheEntry*)getDataOfSListNode(currIndex);

    if (currEntry->serial == currEntry->prevSerial)
    {
      removed++;
      currEntry->flags &= ~PREFIX_FLAG_ANNOUNCEMENT;
      currEntry->serial = ++cache.maxSerial;
      currEntry->expires = tsExp;

      if (currEntry->isASPA)
      {
        currEntry->providerCount = 0;
        free(currEntry->providerAS);
        currEntry->providerAS = NULL;
      }

      // Move to end
      moveSListNode(&cache.entries, &cache.entries, currIndex, prevNode);
    }
    else
    {
      prevNode = currIndex;
    }
  }

  unlockWriteLock(&cache.lock);
  OUTPUTF(true, "Removed %d entries\n", removed);

  // Notify the clients so that they can query the withdrawals
  if (removed > 0)
  {
    service.notify = true;
  }

  return true;
}

/**
 * This method is a wrapper for processEntryRemoval(char* arg) which does
 * the actual work. This wrapper is necessary to return the correct integer
 * value.
 *
 * @param arg list of entry-id's to be deleted from the cache.
 *
 * @return CMD_ID_REMOVE or CMD_ERORR
 *
 * @since 0.3.0.2
 */
int removeEntries(char* arg)
{
  return processEntryRemoval(arg) ? CMD_ID_REMOVE : CMD_ERROR;
}

/**
 * Same as removeEntries except the entries are removed this very second.
 * This method overwrites the sleeping period of the cache. It sends the
 * notification right away to the connected client(s) without waiting.
 *
 * @param arg The list of entries to be removed.
 *
 * @return CMD_ID_REMOVENOW or CDM_ERROR
 */
int removeEntriesNow(char* arg)
{
  int retVal = removeEntries(arg);
  if (retVal == CMD_ID_REMOVE)
  {
    sendSerialNotifyToAllClients();
    retVal = CMD_ID_REMOVENOW;
  }
  return retVal;
}

/**
 * Prepare the generation of the error report.
 *
 * @param arg <errorNo> <pdu | "-"> <msg | "-">
 *
 * @return CMD_ID_ERROR or CMD_ERROR
 */
int issueErrorReport(char* arg)
{
  uint16_t  errNo;
  char*     msg;

  if (arg == NULL)
  {
    ERRORF("Error: No error-code and/or message given\n");
    return CMD_ERROR;
  }

  // Parse code and point to message
  errNo = (uint16_t)strtoul(arg, &msg, 10);
  if (msg == arg)
  {
    ERRORF("Error: Invalid error-code: %s\n", arg);
    return CMD_ERROR;
  }
  if (*msg != '\0')
  {
    msg++;
  }

  // Send
  CacheClient* ccl = NULL;
  if (HASH_COUNT(clients) > 0)
  {
    OUTPUTF(true, "Sending multiple 'Error Report' (Error = %hhu)\n", errNo);

    for (ccl = clients; ccl; ccl = ccl->hh.next)
    {
      sendErrorReport(&ccl->fd ,errNo, msg);
    }
  }

  return CMD_ID_ERROR;
}

/**
 * Print the list of clients to the console.
 *
 * @return CMD_ID_CLIENTS
 */
int listClients()
{
  #define BUF_SIZE (MAX_IP_V6_STR_LEN + 6)
  char          buf[BUF_SIZE];
  CacheClient*  cl;
  unsigned      idx = 1;

  if (HASH_COUNT(clients) == 0)
  {
    printf("No clients\n");
  }
  else
  {
    for (cl = clients; cl; cl = cl->hh.next, idx++)
    {
      printf("%i: %s\n", cl->fd, socketToStr(cl->fd, true, buf, BUF_SIZE));
    }
  }

  return CMD_ID_CLIENTS;
}

// Necessary forward declaration
int handleLine(char* line);

/**
 * Load the file given and execute line by line.
 *
 * @param arg The name of the file
 *
 * @return The last comment in the script or CMD_ID_RUN if unknown or CMD_ERROR
 */
int executeScript(char* fileName)
{
  FILE* fh;
  char  cbuf[LINE_BUF_SIZE];
  char* cpos;
  int last_command = CMD_ID_UNKNOWN;

  fh = fopen(fileName, "rt");
  if (fh == NULL)
  {
    ERRORF("Error: Failed to open the script '%s'\n", fileName);
    return CMD_ERROR;
  }

  while (fgets(cbuf, LINE_BUF_SIZE, fh))
  {
    // Strip comment
    cpos = strchr(cbuf, '#');
    if (cpos != NULL)
    {
      *cpos = '\0';
    }

    // Remove white-space(s)
    chomp(cbuf);

    // Empty line?
    if (*cbuf == '\0')
    {
      continue;
    }

    last_command = handleLine(cbuf);
  }

  fclose(fh);

  return (last_command == CMD_ID_UNKNOWN) ? CMD_ID_RUN : last_command;
}

/**
 * Pauses the application for a given number of seconds
 *
 * @param noSeconds The time in seconds the program has to pause.
 *
 * @return CMD_ID_SLEEP or CMD_ERROR.
 */
int pauseExecution(char* noSeconds)
{
  int sec;
  int retVal = CMD_ID_SLEEP;

  sec = strtol(noSeconds, NULL, 10);
  if (sec <= 0)
  {
    ERRORF("Error: Invalid number of seconds: %s\n", noSeconds);
    retVal = CMD_ERROR;
  }
  else
  {
    sleep(sec);
  }
  return retVal;
}

/**
 * Display the given text on the screen. A new line will be added at the end.
 * 
 * @param text the text to be printed.
 * 
 * @return CMD_ID_ECHO
 * 
 * @since 0.5.1.0
 */
int printText(char* text)
{
  if (text == NULL)
  {
    text = "";
  }
  printf ("%s\n", text);
  
  return CMD_ID_ECHO;
}

/**
 * Wait for a client to connect but no longer than 60 seconds
 * 
 * @param clientIP The IP of the client
 * 
 * @return CMD_ID_WAIT_CLIENT 
 * 
 * @since 0.5.1.0
 */
int waitForClient(char* clientIP)
{
  int  timeout = 60;
  bool found = hasClient(clientIP);
  
  // initialize flags
  inWait = true;
  ctrl_c = false;
  char* space = " ";
  
  if (clientIP != NULL)
  {
    printf("Waiting for client (%s)", clientIP);
    while (!found && (timeout != 0) && !ctrl_c)
    {
      space = "";
      found = hasClient(clientIP);
      found = false;
      if (!found && !ctrl_c)
      {
        printf(".");
        sleep(1);
        timeout--;
      }
    }
    printf("%s%s!\n", space, found ? "connected" 
                                   : ctrl_c ? "stopped" : "timeout");
  }
  else
  {
    printf("No IP provided!\n");
  }
  
  // clear flags
  ctrl_c = false;
  inWait = false;
    
  return CMD_ID_WAIT_CLIENT;
}

/**
 * This function waits until any key was pressed.
 * 
 * @param prompt Contains the prompt. If not provided the default prompt will 
 *               be used.
 * 
 * @return CMD_ID_PAUSE 
 */
int doPause(char* text)
{
  printf("%s ", text != NULL ? text : "Press any key to continue!");
  fgetc(stdin);   
  return CMD_ID_PAUSE;
}

/**
 * Doesn't really do anything
 *
 * @return CMD_ID_QUIT
 *
 * @since 0.3.0.2
 */
int processQuit()
{
  return CMD_ID_QUIT;
}

////////////////////////////////////////////////////////////////////////////////
// CONSOLE INPUT
////////////////////////////////////////////////////////////////////////////////
/** List of commands for auto completion. */
char* commands[] = {
  "verbose",
  "cache",
  "version",
  "help",
  "credits",
  "sessionID",
  "empty",
  "append",
  "add",
  "addNow",
  "keyLoc",
  "addKey",
  "addKeyNow",
  "addASPA",
  "addASPANow",
  "remove",
  "removeNow",
  "error",
  "notify",
  "reset",
  "clients",
  "run",
  "sleep",
  "quit",
  "exit",
  "echo",
  "waitFor",
  "pause",
  "*",
  NULL};

char** command_completion(const char *, int, int);
char*  command_generator(const char *, int);

char** command_completion(const char *text, int start, int end)
{
  rl_attempted_completion_over = 1;
  return rl_completion_matches(text, command_generator);
}

char* command_generator(const char *text, int state)
{
  static int list_index, len;
  char *name;

  if (!state)
  {
    list_index = 0;
    len = strlen(text);
  }

  while ((name = commands[list_index++]))
  {
    if (strncmp(name, text, len) == 0)
    {
      return strdup(name);
    }
  }

  return NULL;
}

/**
 * This method parses the line by splitting it into command and argument
 * separated by a blank. Depending on the command the appropriate method is
 * called. With version 0.3.0.2 this method will return true if the parser can
 * continue and false if not. (false does not necessarily mean an error.
 *
 * @param line The line to be parsed
 *
 * @return the integer value of the executed command.
 */
int handleLine(char* line)
{
  char* cmd, *arg;
  int retVal = CMD_ID_UNKNOWN;

  // Split into command and argument
  arg = line;
  cmd = strsep(&arg, " ");

  // Call function that is going to handle the command
  #define CMD_CASE(STR, FUNC) \
    if (!strcmp(cmd, STR)) { return FUNC(arg); }

  CMD_CASE("verbose",   toggleVerboseMode);
  CMD_CASE("cache",     printCache);
  CMD_CASE("version",   showVersion);
  CMD_CASE("\\h",       showHelp);
  CMD_CASE("help",      showHelp);
  CMD_CASE("credits",   showCredits);

  CMD_CASE("sessionID", processSessionID);

  CMD_CASE("empty",     emptyCache);
  CMD_CASE("append",    appendPrefixFile);
  CMD_CASE("add",       appendPrefix);
  CMD_CASE("addNow",    appendPrefixNow);
  CMD_CASE("keyLoc",    setKeyLocation);
  CMD_CASE("addKey",    appendRouterKey);
  CMD_CASE("addKeyNow", appendRouterKeyNow);
  CMD_CASE("addASPA",    appendASPA);
  CMD_CASE("addASPANow", appendASPANow);
  CMD_CASE("remove",    removeEntries);
  CMD_CASE("removeNow", removeEntriesNow);
  CMD_CASE("error",     issueErrorReport);
  CMD_CASE("notify",    sendSerialNotifyToAllClients);
  CMD_CASE("reset",     sendCacheResetToAllClients);

  CMD_CASE("clients",   listClients);
  CMD_CASE("run",       executeScript);
  CMD_CASE("sleep",     pauseExecution);
  CMD_CASE("waitFor",   waitForClient);
  CMD_CASE("pause",     doPause);
  
  CMD_CASE("echo",      printText);

  CMD_CASE("quit",      processQuit);
  CMD_CASE("exit",      processQuit);
  CMD_CASE("\\q",       processQuit);

  // Unknown
  printf("Error: Unknown command '%s'\n", cmd);
  return retVal;
}

/**
 * Processes the input from the command line.
 */
void handleUserInput()
{
  char* line;

  using_history();
  read_history(HISTORY_FILENAME);
  int cmd = CMD_ID_UNKNOWN;
  char cmdLine[255];
  char historyLine[255];
  // Add auto completion
  rl_attempted_completion_function = command_completion;
  OUTPUTF(false, "Enable command auto completion - switch to file browser "
                 "using '*'\n");
  // Added trim = M0000713
  while((line = readline(USER_PROMPT)) != NULL)
  {
    line = trim(line);
    if (strcmp(line, "*")==0)
    {
      // Toggle auto completion
      if (rl_attempted_completion_function != 0)
      {
        OUTPUTF(false, "Enable file browser - switch using '*'\n");
        rl_attempted_completion_function = 0;
      }
      else
      {
        OUTPUTF(false, "Enable command auto completion - switch using '*'\n");
        rl_attempted_completion_function = command_completion;
      }
      free(line);
      continue;
    }
    snprintf(cmdLine, 255, line);
    snprintf(historyLine, 255, line);
    free(line);

    // Empty line - ignore
    if (strlen(cmdLine) == 0)
    {
      continue;
    }

    // Execute the line
    cmd = handleLine(cmdLine);
    if (cmd == CMD_ID_QUIT)
    {
      break;
    }
    else if (cmd != CMD_ID_UNKNOWN && cmd != CMD_ERROR)
    {
      // Store so that the user does not have to type it again
      add_history(historyLine);
    }
  }

  if (write_history(HISTORY_FILENAME) != 0)
  {
    printf("Failed writing history file '%s'\n", HISTORY_FILENAME);
  }
}

/*----------
 * Callbacks
 */
void deleteExpiredEntriesFromCache(time_t now)
{
  SListNode*  currNode, *nextNode;
  ValCacheEntry* cEntry;
  uint32_t    removed = 0;

  acquireWriteLock(&cache.lock);
  currNode = getRootNodeOfSList(&cache.entries);
  while (currNode)
  {
    cEntry   = (ValCacheEntry*)getDataOfSListNode(currNode);
    nextNode = getNextNodeOfSListNode(currNode);

    // Entry expired
    if ((cEntry->expires > 0) && (cEntry->expires <= now))
    {
      cache.minPSExpired = MIN(cache.minPSExpired, cEntry->prevSerial);
      cache.maxSExpired  = MAX(cache.maxSExpired, cEntry->serial);

      // free ski and key allocations
      if(cEntry->isKey)
      {
        if (cEntry->ski)
        {
          free(cEntry->ski);
          cEntry->ski = NULL;
        }
        if (cEntry->pPubKeyData)
        {
          free(cEntry->pPubKeyData);
          cEntry->pPubKeyData = NULL;
        }
      }
      
      if(cEntry->isASPA)
      {
        if(cEntry->providerAS)
        {
          free(cEntry->providerAS);
          cEntry->providerAS = NULL;
        }
      }

      deleteFromSList(&cache.entries, cEntry);
      removed++;
    }

    currNode = nextNode;
  }
  unlockWriteLock(&cache.lock);

  if (removed > 0)
  {
    OUTPUTF(true, "Deleted %d expired entries\n", removed);
  }
}

void serviceTimerExpired(int id, time_t now)
{
  deleteExpiredEntriesFromCache(now);

  // A change occurred that requires a serial notify
  if (service.notify)
  {
    service.notify = false;
    sendSerialNotifyToAllClients();
  }
}

/** 
 * Use the log level specified or if verbose is enabled.
 * 
 * @param level The log level of the message/
 * @param fmt The format string
 * @param args The argument list matching the format string.
 */
void printLogMessage(LogLevel level, const char* fmt, va_list args)
{
  if ((level == getLogLevel()) || verbose)
  {
    putc('\r', stdout);
    vprintf(fmt, args);
    printf("\n");
    OPROMPT();
  }
}

/**
 * This handler deals with SIGINT signals and relaces the old handler.
 *
 * @param signal
 */
void handleSigInt(int signal)
{
  if (!inWait)
  {
    printf ("\nUse command 'exit'\n");
  }
  ctrl_c = true;
}


////////////////////////////////////////////////////////////////////////////////
// MAIN PROGRAM
////////////////////////////////////////////////////////////////////////////////

bool setupCache()
{
  initSList(&cache.entries);
  if (!createRWLock(&cache.lock))
  {
    ERRORF("Error: Failed to create the cache R/W lock");
    return false;
  }
  cache.maxSerial     = 0;
  cache.minPSExpired  = UINT32_MAX;
  cache.maxSExpired   = 0;
  cache.version       = RPKI_RTR_PROTOCOL_VERSION; // cache version

  cache.refreshInterval = RPKI_RTR_REFRESH_DEF;
  cache.retryInterval   = RPKI_RTR_RETRY_DEF;
  cache.expireInterval  = RPKI_RTR_EXPIRE_DEF;

  return true;
}

bool setupService()
{
  service.timer = setupTimer(serviceTimerExpired);
  if (service.timer == -1)
  {
    ERRORF("Error: Failed to create the service timer");
    return false;
  }
  service.notify = false;
  startIntervalTimer(service.timer, SERVICE_TIMER_INTERVAL, false);

  return true;
}

/**
 * Print the program syntax on stdio
 *
 * @param prgName The program name.
 *
 * @since 0.5.0.0
 */
static void syntax(const char* prgName)
{
  printf ("Syntax: %s [options] [port [script]]\n", prgName);
  printf ("  options:\n");
  printf ("    -f <script>  A script that has to be executed as soon as\n");
  printf ("                 the server is started.\n");
  printf ("    -D <level>   Set the logging level ERROR(%i) to DEBUG(%i)\n\n",
                            LEVEL_ERROR, LEVEL_DEBUG);
  printf ("  For backward compatibility, a script also can be added after a\n");
  printf ("  port is specified.! - For future usage, use -f <script> to \n");
  printf ("  specify a script!\n");
  printf ("  If no port is specified the default port %u is used.\n",
          DEF_RPKI_PORT);
  printf ("\n");
  showVersion();
}

/**
 * Parses the program parameters and set the configuration. This function
 * returns true if the program can continue and the exit Value.
 *
 * @param argc    The argument count
 * @param argv    The Argument array
 * @param cfg     The program configuration
 * @param exitVal The exit value pointer if needed
 *
 * @return true if the program can continue, false if it should be ended.
 *
 * @since 0.5.0.0
 */
static bool parseParams(int argc, const char* argv[],
                        RPKI_SRV_Configuration* cfg, int* exitVal)
{
  bool retVal = true;
  int  eVal   = 0;
  bool doHelp = false;
  char* arg   = NULL;
  int idx     = 0;
  
  #define HMSG " - try '-?' for more info"

  for (idx = 1; (idx < argc) && !doHelp && retVal; idx++)
  {
    arg = (char*)argv[idx];
    if (arg[0] == '-')
    {
      arg++;
      if (strcmp(arg, "-help") == 0)
      {
        doHelp = true;
      }
      else switch(arg[0])
      {
        case 'h':
        case 'H':
        case '?':
          doHelp = true;
          break;
        case 'D':
          idx++;
          if (idx < argc)
          {
            int logLevel = atoi(argv[idx]);
            if ((logLevel >= LEVEL_ERROR) && (logLevel <= LEVEL_DEBUG))
            {
              setLogLevel(logLevel);
            }
            else
            {
              printf ("ERROR: Invalid log level!\n");
              printf ("  Accepted values range from ERROR(%i) to DEBUG(%i)\n",
                      LEVEL_ERROR, LEVEL_DEBUG);
              retVal = false;
              eVal   = 1;
            }
          }
          else
          {
            printf ("ERROR: Log level missing%s!\n", HMSG);
            retVal = false;
            eVal   = 1;
          }
          break;
          break;
        case 'f':
          if (cfg->script == NULL)
          {
            idx++;
            if (idx < argc)
            {
              cfg->script = (char*)argv[idx];
            }
            else
            {
              printf ("ERROR: filename missing%s!\n", HMSG);
              retVal = false;
              eVal   = 1;
            }
          }
          else
          {
            printf ("ERROR: Script already added%s!\n", HMSG);
            retVal = false;            
            eVal   = 1;
          }    
          break;
        default:
          printf ("ERROR: Invalid parameter '%s'%s!\n", arg, HMSG);
          retVal = false;
          eVal   = 1;
          break;
      }
    }
    else if ((strcmp(arg, "help") == 0) || (arg[0] == '?'))
    {
      doHelp = true;
    }
    else if (cfg->port == 0)
    {
      cfg->port = strtol(arg, NULL, 10);
    }
    else
    {
      if (cfg->script == NULL)
      {
        cfg->script = arg;
        printf ("WARNING: Script added but use -f <script> to add scripts in "
                "the future.\n");
      }
      else
      {
        printf ("ERROR: Script already added%s!\n", HMSG);
        retVal = false;
      }
    }
  }

  if (doHelp)
  {
    syntax(argv[0]);
    retVal = false;
  }

  // Configure the default port if not specified otherwise.
  if (cfg->port == 0)
  {
    cfg->port = DEF_RPKI_PORT;
  }

  if (exitVal != NULL)
  {
    *exitVal = eVal;
  }

  return retVal;
}

/**
 * Start the RPKI Test Harness.
 *
 * @param argc The number of arguments passed to the program
 * @param argv The arguments passed to the program
 *
 * @return The program exit level.
 */
int main(int argc, const char* argv[])
{
  pthread_t rlthread;
  int       ret = 0;
  
  // Disable printout buffering.
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);
  
  RPKI_SRV_Configuration config;
  memset(&config, 0, sizeof(RPKI_SRV_Configuration));
  // Initialize keyLocation
  setKeyLocation(NULL);

  setLogLevel(LEVEL_WARNING);

  if (!parseParams(argc, argv, &config, &ret))
  {
    return ret;
  }

#ifndef BRIO
  printf("Start %s using port %u\n", RPKI_RTR_SRV_NAME, config.port);
#else
  printf("Start %s using port %u\n", BRIO_RPKI_CACHE_NAME, config.port);
#endif

  // Output all log messages to stderr
  setLogMethodToCallback(printLogMessage);

  // Initialize the cache
  if (!setupCache())
  {
    return -2;
  }

  // Bind to the port
  if (!createServerSocket(&svrSocket, config.port, true))
  {
    releaseRWLock(&cache.lock);
    return -3;
  }

  // Service (= maintenance)
  if (!setupService())
  {
    stopServerLoop(&svrSocket);
    releaseRWLock(&cache.lock);
    return -4;
  }

  showVersion();

  // Start run loop and handle user input
  if (pthread_create(&rlthread, NULL, handleServerRunLoop, NULL) == 0)
  {
    // Handle Ctrl-C
    struct sigaction new_sigaction, old_sigaction;
    new_sigaction.sa_handler = handleSigInt;
    sigemptyset(&new_sigaction.sa_mask);
    new_sigaction.sa_flags = 0;

    sigaction (SIGINT, NULL, &old_sigaction);
    if (old_sigaction.sa_handler != SIG_IGN)
    {
      sigaction(SIGINT, &new_sigaction, NULL);
    }
    sigaction (SIGHUP, NULL, &old_sigaction);

    if (old_sigaction.sa_handler != SIG_IGN)
    {
      sigaction (SIGHUP, &new_sigaction, NULL);
    }
    sigaction (SIGTERM, NULL, &old_sigaction);

    if (old_sigaction.sa_handler != SIG_IGN)
    {
      sigaction (SIGTERM, &new_sigaction, NULL);
    }

    //signal(SIGINT, handleSigInt);

    ret = 0;
    bool doContiunue = true;
    if (config.script != NULL)
    {
      doContiunue = executeScript(config.script) != CMD_ID_QUIT;
    }
    if (doContiunue)
    {
      handleUserInput();
    }
  }
  else
  {
    ret = -5;
    ERRORF("Error: Failed to start server run-loop\n");
  }

  // Stop all timers
  deleteAllTimers();

  // Release port
  stopServerLoop(&svrSocket);

  // Cleanup
  releaseRWLock(&cache.lock);
  releaseSList(&cache.entries);
  memset(keyLocation, 0, LINE_BUF_SIZE);

  return ret;
}
