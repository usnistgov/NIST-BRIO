## BGP Router Configuration for Upstream Path Verification - Example #79
---
The following table shows the minimal configuration of the router. The example
requires the router to be capable of processing ASPA.

| Setting    | Value | Description                                             |
| :--------- | :---- | :------------------------------------------------------ | 
| AS Number  | 65030 |  The AS Number of the Implementation Under Test.        |
| IP Address | {IP_AS_65030-79} | The IP Address the router is listening on.   |
|            |       |                                                         |
| Peer AS    | 65000 |                                                         |
| Peer IP    | {IP_AS_65000-79} | The IP Address or the BGP peer               | 
| Peer Relation | Customer | The Peering Relation (1)                          |
| Peer BGPsec   |          | The BGPsec mode (2) - no value disabled!          |
|            |             |                                                   |
| Peer AS    | 65040   |                                                       |
| Peer IP    | {IP_AS_65040-79} | The IP Address or the BGP peer               | 
| Peer Relation | Lateral | The Peering Relation (1)                           |
| Peer BGPsec   |         | The BGPsec mode (2) - no value disabled!           |
|            |         |                                                       |
| Mode       | Passive | The router must not initiate the BGP session.         |
|            |         |                                                       |
| Validation | Origin  | Perform Route Origin Validation                       |
| Validation | ASPA    | Perform ASPA Validation                               |
|            |         |                                                       |
| RPKI Cache IP | {IP_AS_65000-79} | The IP Address of the RPKI Validation Cache.|
| RPKI Cache Port | 50000 | The Port Address of the RPKI Validation Cache.     |
| RPKI Cache Protocol | TCP | Regular TCP session.                             |

(1) The `Peer Relation` is  needed for ASPA and can contain the following values:
* Customer  The peer is a topologically downstream (customer AS)
* Lateral   The peering only includes prefixes of customers.
* Provider  The peer is a topologically upstream neighbor (transit provider)
* Sibling   The peer is transit provider and transit customer.

(2) The `Peer BGPsec` setting specifies a possible BGPsec configuration between the 
peers.
* Both     Send BGPSEC and receive BGPSEC
* Receive  Receive BGPSEC but send BGP4 only
* Send     Send BGPSEC but receive BGP4 only

[Back](exp79.README.tpl.md)
