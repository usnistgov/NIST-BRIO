## BGP Router Configuration for Downstream Path Verification - Example #8
---
The following table shows the minimal configuration of the router. The example
requires the router to be capable of processing ASPA.

| Setting    | Value | Description                                             |
| :--------- | :---- | :------------------------------------------------------ | 
| AS Number  | 65030 |  The AS Number of the Implementation Under Test.        |
| IP Address | {IP_AS_65030-8} | The IP Address the router is listening on.    |
|            |       |                                                         |
| Peer AS    | 65050 |                                                         |
| Peer IP    | {IP_AS_65050-8} | The IP Address or the BGP peer                | 
|            |       |                                                         |
| Mode       | Passive | The router must not initiate the BGP session.         |
|            |         |                                                       |
| Validation | Origin  | Perform Route Origin Validation                       |
| Validation | ASPA    | Perform ASPA Validation                               |
|            |         |                                                       |
| RPKI Cache IP | {IP_AS_65050-8} | The IP Address of the RPKI Validation Cache.|
| RPKI Cache Port | 50000 | The Port Address of the RPKI Validation Cache.     |
| RPKI Cache Protocol | TCP | Regular TCP session.                             |

[Back](exp8.README.tpl.md)
