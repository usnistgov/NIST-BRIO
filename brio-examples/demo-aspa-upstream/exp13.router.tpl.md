## BGP Router Configuration for Upstream Path Verification - Example #13
---
The following table shows the minimal configuration of the router. The example
requires the router to be capable of processing ASPA.

| Setting    | Value | Description                                             |
| :--------- | :---- | :------------------------------------------------------ | 
| AS Number  | 65060 |  The AS Number of the Implementation Under Test.        |
| IP Address | {IP_AS_65060-13} | The IP Address the router is listening on.   |
|            |       |                                                         |
| Peer AS    | 65030 |                                                         |
| Peer IP    | {IP_AS_65030-13} | The IP Address or the BGP peer               | 
|            |       |                                                         |
| Peer AS    | 65050 |                                                         |
| Peer IP    | {IP_AS_65050-13} | The IP Address or the BGP peer               | 
|            |       |                                                         |
| Mode       | Passive | The router must not initiate the BGP session.         |
| Protocol   | BGP-4 | Regular BGP-4 with ASPA enabled                         |
|            |       |                                                         |
| RPKI Cache IP | {IP_AS_65030-13} | The IP Address of the RPKI Validation Cache.|
| RPKI Cache Port | 50000 | The Port Address of the RPKI Validation Cache.     |
| RPKI Cache Protocol | TCP | Regular TCP session.                             |

[Back](exp13.README.tpl.md)
