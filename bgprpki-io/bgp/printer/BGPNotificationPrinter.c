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
 * Provides functionality to print a BGP Update
 * 
 * @version 0.2.0.19
 * 
 * Changelog:
 * -----------------------------------------------------------------------------
 *  0.2.0.19- 2018/04/19 - oborchert
 *            * Added clear text for notification sub-code in simple mode.
 *  0.2.0.12- 2018/04/12 - oborchert
 *            * Added simple to printNotificationData to allow a more simplistic 
 *              printing.
 *  0.2.0.7 - 2017/03/10 - oborchert
 *            * Fixed wrong printout in ERR2.
 *            * Enhanced Notification printer by printing the capabilities data
 *              for 'Unsupported Capability' notification.
 *  0.2.0.5 - 2017/01/31 - oborchert
 *            * Added text for "Bad Message Length" notification.
 *            * Fixed data printout in Notification printer. (BZ1095)
 *  0.2.0.4 - 2016/07/01 - oborchert
 *            * BZ1007 - speller in Notification output.
 *  0.2.0.0 - 2016/05/12 - oborchert
 *            * Fixed some invalid formatting in output.
 *          - 2016/05/10 - oborchert
 *            * Compiler warning exposed error in printNotificationData where 
 *              the subcode string was never printed. Fixed now.
 *  0.1.1.0 - 2016/03/21 - oborchert
 *            * Created File.
 */

#include <stdio.h>
#include <string.h>
#include "bgp/BGPHeader.h"
#include "bgp/printer/BGPHeaderPrinter.h"
#include "bgp/printer/BGPPrinterUtil.h"
#include "BGPOpenPrinter.h"


/**
 * Print the BGP Notification Message
 * 
 * @param update The notification message as complete BGP packet. 
 * @param simple If true, do not use the tree format as in wireshark
 * 
 */
void printNotificationData(BGP_NotificationMessage* notification, bool simple)
{
  u_int16_t notificationLength = ntohs(notification->messageHeader.length);
  u_int8_t* start = (u_int8_t*)notification;
  u_int8_t* data = start + sizeof(BGP_NotificationMessage);  
  u_int8_t* end = start + notificationLength;
  
  char* errStr = NULL;
  char* subCodeStr = NULL;
  switch (notification->error_code)
  {
    case BGP_ERR1_MESSAGE_HEADER:
      errStr = "Message Header Error\0";
      switch (notification->sub_code)
      {
        case BGP_ERR1_SUB_NOT_SYNC:
          subCodeStr = "\0"; break;
        case BGP_ERR1_SUB_BAD_LENGTH:
          subCodeStr = "Bad Message Length\0"; break;
        case BGP_ERR1_SUB_BAD_TYPE:
          subCodeStr = "\0"; break;
        default:
          subCodeStr = "UNKNOWN\0";    
      }
      break;
      
    case BGP_ERR2_OPEN_MESSAGE:
      errStr = "OPEN Message Error\0"; 
      switch (notification->sub_code)
      {
        case BGP_ERR2_SUB_UNSUPPORTED_BGPSEC_VER:
          subCodeStr = "Unsupported BGPSec Version Number\0"; break;
        case BGP_ERR2_SUB_VERSION:
          subCodeStr = "Unsupported Version Number\0"; break;
        case BGP_ERR2_SUB_BAD_PEERAS   :
          subCodeStr = "Bad Peer AS\0"; break;
        case BGP_ERR2_SUB_BAD_BGPIDENT:
          subCodeStr = "Bad BGP Identifier\0"; break;
        case BGP_ERR2_SUB_UNSUPP_OPT_PARAM:
          subCodeStr = "Unsupported Optional Parameter\0"; break;
        case BGP_ERR2_SUB_DEPRECATED:
          subCodeStr = "Deprecated (See RFC4271 - Appendix A)\0"; break;
        case BGP_ERR2_SUB_UNACCEPTED_HOLDTIME:
          subCodeStr = "Unacceptable Hold Time\0"; break;
        case BGP_ERR2_SUB_UNSUPPORTED_CAPABILITY:
          subCodeStr = "Unsupported Capability\0"; break;
        default:
          subCodeStr = "UNKNOWN\0";    
      }
      break;
      
    case BGP_ERR3_UPDATE_MESSAGE:
      errStr = "UPDATE Message Error\0"; 
      switch (notification->sub_code)
      {
        case BGP_ERR3_SUB_MALFORMED_ATTR_LIST:
          subCodeStr = "Malformed Attribute List\0"; break;
        case BGP_ERR3_SUB_UNRECOG_WELLKNOWN_ATTR:
          subCodeStr = "Unrecognized Well-known Attribtue\0"; break;
        case BGP_ERR3_SUB_MISSING_WELLKNOWN_ATTR:
          subCodeStr = "Missing Well-known Attribtue\0"; break;
        case BGP_ERR3_SUB_ATTR_FLAG_ERR:
          subCodeStr = "Attribute Flags Error\0"; break;
        case BGP_ERR3_SUB_ATTR_LEN_ERR:
          subCodeStr = "Attribtue Length Error\0"; break;
        case BGP_ERR3_SUB_INVALID_ORIGIN_ATTR:
          subCodeStr = "Invalid ORIGIN Attribute\0"; break;
        case BGP_ERR3_SUB_DEPRECATED:
          subCodeStr = "Deprecated (See RFC4271 - Appendix A)\0"; break;
        case BGP_ERR3_SUB_INVLAID_NEXT_HOP:
          subCodeStr = "Invalid NEXT_HOP Attribute\0"; break;
        case BGP_ERR3_SUB_OPTIONAL_ATTR_ERR:
          subCodeStr = "Optional Attribute Error\0"; break;
        case BGP_ERR3_SUB_INVALID_NETWORK_FIELD:
          subCodeStr = "Invalid Network Field\0"; break;
        case BGP_ERR3_SUB_MALFORMED_AS_PATH:
          subCodeStr = "Malformed AS_PATH\0"; break;
        default:
          subCodeStr = "UNKNOWN\0";    
      }
      break;
      
    case BGP_ERR4_HOLD_TIMER_EXPIRED:
      errStr = "Hold Timer Expired\0"; 
      subCodeStr = "UNKNOWN\0";
      break;
      
    case BGP_ERR5_FSM:
      errStr = "Finite State Machine Error\0"; 
      subCodeStr = "UNKNOWN\0";
      break;
      
    case BGP_ERR6_CEASE:
      errStr = "Cease\0"; 
      switch (notification->sub_code)
      {
        case BGP_ERR6_SUB_MAX_NUM_PREFIXES:
          subCodeStr = "Maximum Number of Prefixes Reached\0";
          break;
        case BGP_ERR6_SUB_ADMIN_SHUTDOWN:
          subCodeStr = "Administrative Shutdown\0";
          break;
        case BGP_ERR6_SUB_PEER_DE_CONFIGURED:          
          subCodeStr = "Peer De-configured\0";
          break;
        case BGP_ERR6_SUB_ADMIN_RESET:
          subCodeStr = "Administrative Reset\0";
          break;
        case BGP_ERR6_SUB_CONNECTION_REJECTED:
          subCodeStr = "Connection Rejected\0";
          break;
        case BGP_ERR6_SUB_OTHER_CONFIG_CHANGE:
          subCodeStr = "Other Configuration Change\0";
          break;
        case BGP_ERR6_SUB_CONN_COLL_RESOLUTION:
          subCodeStr = "Connection Collision Resolution\0";
          break;
        case BGP_ERR6_SUB_OUT_OF_RESOURCES:          
          subCodeStr = "Out of Resources\0";
          break;
        default:
          subCodeStr = "UNKNOWN\0";          
      }
      break;      
    default:
      errStr = "UNKNOWN\0";
      subCodeStr = "UNKNOWN\0";
  }
  
  if (!simple)
  {
    printf("%s+--Error code: %u (%s)\n", TAB_2, notification->error_code, errStr);
    printf("%s+--Error subcode: %u (%s)\n", TAB_2, notification->sub_code, 
           subCodeStr);

    char useTab[STR_MAX];
    snprintf(useTab, STR_MAX, "%s", TAB_2);

    // If the notification contains Capabilities in the data block, print them.
    if (notification->sub_code == BGP_ERR2_SUB_UNSUPPORTED_CAPABILITY)
    {
      BGP_Capabilities* cap = (BGP_Capabilities*)data;
      int minSize   = sizeof(BGP_Capabilities);    
      int remainder = end - data;
      int length    = minSize + cap->cap_length;

      printf("%s+--Unsupported Capabilities\n", useTab);
      snprintf(useTab, STR_MAX, "%s   ", TAB_2);

      while ((remainder >= minSize) && (length <= remainder))
      {
        data += printCapability(cap, useTab, (length < remainder));
        remainder = end - data;
        if (remainder >= minSize)
        {
          cap = (BGP_Capabilities*)data;
          length = sizeof(BGP_Capabilities) + cap->cap_length;
        }
      }
    }

    if (data < end)
    {
      int dataLength = (int)(end - data);
      char dataStr[STR_MAX];
      // write the text first in the variable to see how long it becomes. This 
      // will be the tab for the final print in case data is very large.

      snprintf(dataStr, STR_MAX, "%s+--data: ", useTab);
      // Now print the tree leaf name
      printf("%s", dataStr);
      // Now generate the tab
      memset(dataStr, ' ', strlen(dataStr));
      // Now write the hex data (formatted)
      printHex(data, dataLength, dataStr);
    }
  }
  else
  {
    // Do print only one simple string.
    printf("NOTIFICATION: '%s' subcode %u (%s)\n", errStr, 
                                                   notification->sub_code, 
                                                   subCodeStr);
  }
}
