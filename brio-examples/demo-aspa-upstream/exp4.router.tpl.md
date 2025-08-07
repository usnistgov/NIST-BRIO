## BGP Router Configuration for Upstream Path Verification - Example #4
---
The following table shows the minimal configuration of the router. The example
requires the router to be capable of processing ASPA.

| Setting    | Value | Description                                             |
| :--------- | :---- | :------------------------------------------------------ | 
| AS Number  | 65020 |  The AS Number of the Implementation Under Test.        |
| IP Address | {IP_AS_65020-4} | The IP Address the router is listening on.    |
|            |       |                                                         |
| Peer AS    | 65030 |                                                         |
| Peer IP    | {IP_AS_65030-4} | The IP Address or the BGP peer                | 
| Peer Relation | Lateral | The Peering Relation (1)                           |
| Peer BGPsec   |         | The BGPsec mode (2) - no value disabled!           |
|            |            |                                                    |
| Mode       | Passive | The router must not initiate the BGP session.         |
|            |         |                                                       |
| Validation | Origin  | Perform Route Origin Validation                       |
| Validation | ASPA    | Perform ASPA Validation                               |
|            |         |                                                       |
| RPKI Cache IP | {IP_AS_65030-4} | The IP Address of the RPKI Validation Cache.|
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

[Back](exp4.README.tpl.md)
