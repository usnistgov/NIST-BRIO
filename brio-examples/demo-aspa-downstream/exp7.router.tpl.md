## BGP Router Configuration for Downstream Path Verification - Example #7
---
The following table shows the minimal configuration of the router. The example
requires the router to be capable of processing ASPA.

| Setting    | Value | Description                                           |
| :--------- | :---- | :---------------------------------------------------- | 
| AS Number  | 65000 |  The AS Number of the Implementation Under Test.      |
| IP Address | {IP_AS_65000-7} | The IP Address the router is listening on.  |
|            |       |                                                       |
| Peer AS    | 65020 |                                                       |
| Peer IP    | {IP_AS_65020-7} | The IP Address or the BGP peer              | 
|            |       |                                                       |
| Mode       | Passive | The router must not initiate the BGP session.       |
| Protocol   | BGP-4 | Regular BGP-4 with ASPA enabled                       |
|            |       |                                                       |
| RPKI Cache IP | {IP_AS_65020-7} | The IP Address of the RPKI Validation Cache. |
| RPKI Cache Port | 50000 | The Port Address of the RPKI Validation Cache.   |
| RPKI Cache Protocol | TCP | Regular TCP session.                           |

[Back](exp7.README.tpl.md)
