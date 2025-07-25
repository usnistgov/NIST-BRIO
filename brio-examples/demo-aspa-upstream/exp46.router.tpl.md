## BGP Router Configuration for Upstream Path Verification - Example #46
---
The following table shows the minimal configuration of the router. The example
requires the router to be capable of processing ASPA.

| Setting    | Value | Description                                             |
| :--------- | :---- | :------------------------------------------------------ | 
| AS Number  | 65020 |  The AS Number of the Implementation Under Test.        |
| IP Address | {IP_AS_65020-46} | The IP Address the router is listening on.   |
|            |       |                                                         |
| Peer AS    | 65030 |                                                         |
| Peer IP    | {IP_AS_65030-46} | The IP Address or the BGP peer               | 
|            |       |                                                         |
| Peer AS    | 65000 |                                                         |
| Peer IP    | {IP_AS_65000-46} | The IP Address or the BGP peer               | 
|            |       |                                                         |
| Mode       | Passive | The router must not initiate the BGP session.         |
|            |         |                                                       |
| Validation | Origin  | Perform Route Origin Validation                       |
| Validation | ASPA    | Perform ASPA Validation                               |
|            |         |                                                       |
| RPKI Cache IP | {IP_AS_65030-46} | The IP Address of the RPKI Validation Cache.|
| RPKI Cache Port | 50000 | The Port Address of the RPKI Validation Cache.     |
| RPKI Cache Protocol | TCP | Regular TCP session.                             |

[Back](exp46.README.tpl.md)
