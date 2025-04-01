# ASPA Upststeam Verification Example #46
This example contains examples #4, #5, and #6 bundled in a single example.
## Introduction
This demo uses one BGP router instance under test (IUT) that must support ASPA 
processing, two BGP RPKI-IO (BRIO) traffic generators (brio_tg) and one BRIO 
RPKI Validation Cache Test Harness (brio_rc). 

All required files for this experiment have the prefix ```exp46.```

Three BGP UPDATEs are received from a customer or lateral peer. AS 65010 (B)
announces 3 prefixes, 10.65.10.0/24, 10.65.11.0/24, and 10.65.12.0/24.  
```
65020 (C) <-- 65030 (D) <-- 65040 (E) <-- 65010 (B), 10.65.10.0/24
65020 (C) <-- 65000 (A) <-- 65030 (D) <-- 65040 (E) <-- 65010 (B), 10.65.11.0/24
65020 (C) <-- 65000 (A) <-- 65030 (D) <-- 65060 (G) <-- 65040 (E) <-- 65010 (B), 10.65.12.0/24
```  

## System Requirements
For this example, two IP addresses are required. Both can be assigned to alias
interfaces.  
IP-Address-1: {IP_AS_65020-46} (C) Used for IUT 
IP-Address-2: {IP_AS_65030-46} (D) Used for BRIO-1 Traffic Generator (brio_tg) and 
              BRIO RPKI Cache (brio_rc)
IP-Address-3: {IP_AS_65000-46} (A) Used for BRIO-2 Traffic Generator (brio_tg)

## Topology
```
                    <-p2p->
                 [F].......(G) {AS0 ASPA}
       /|\       . .       / \
   C2P  |       .   .     /   \
        |      .     .   /     \
              .<-p2p->. /<-p2p->\
            (C)-------(D)-------[E]
       /|\    \       /           \
   C2P  |      \     /             \
        |       \   /               \
                 \ /                 \
                 (A)                 (B)

[E] and [F] have no ASPAs  
--/--\-- Updates are sent across this path
........ Path available but no update traverse for this example
```
### Legend:

| Label | Description                  | \| | Node | AS Number | Prefix*        |
| ----- | ---------------------------- | -- | :--: | :-------: | :-----------  |
| C2P   | Customer to Peer             | \| |  A   | AS 65000  | 10.65.0.0/22  |
| p2p   | Peer to Peer (lateral peers) | \| |  B   | AS 65010  | 10.65.10.0/22 |
| [X]   | Do not have ASPAs            | \| |  C   | AS 65020  | 10.65.20.0/22 |
|       |                              | \| |  D   | AS 65030  | 10.65.30.0/22 |
|       |                              | \| |  E   | AS 65040  | 10.65.40.0/22 |
|       |                              | \| |  F   | AS 65050  | 10.65.52.0/22 |
|       |                              | \| |  G   | AS 65060  | 10.65.60.0/22 |

\* The prefixes above reference the range assigned to each AS and can be announced
as as full prefix and or sub-prefixes.
(E.g.: 10.65.0.0/22 can be announced as 10.65.0.0/23 and 10.65.2.0/23)

## Example Scenario
* C is the receiving/verifying AS (Implementation Under Test - IUT)
* C receives BGP updates from peers D and A with AS paths: 
  * D E B
  * A D E B
  * A D G E B
  (simplified to contain only unique ASes)
* AS path lengths = 3, 4 and 5 respectively
* Using the given ASPAs and the method in Section 6.1 and procedure in 
  Section 6.2 of [[ASPA verification](https://datatracker.ietf.org/doc/draft-ietf-sidrops-aspa-verification/)] ...
  * ... the max_up_ramp and min_up_ramp values can be retrieved from the table 
        below.
  * ... the verification outcomes can be retrieved from the table below. 

## ASPA information send by BRIO rpki cache
This table only contains the ASPA information for routers in this experiment
| Customer     | Provider                  |
| :----------- | :------------------------ |
| AS 65000 (A) | AS 65020 (C) AS 65030 (D) |
| AS 65010 (B) | AS 65040 (E)              |
| AS 65020 (C) | AS 65050 (F)              |
| AS 65030 (D) | AS 65050 (F) AS 65060 (G) |
| AS 65060 (G) | AS 0                      |

See the [Cache File](exp46.brio_rc.script)

## UPDATE and Upstream Path verification result
BGP update received from a customer or lateral peers

| Exp | Receiving / Verifying AS | AS path (Unique ASes) | AS path length | max_up_ramp | min_up_ramp | Upstream Path Verification Result |
| :-: | ------------------------ | --------------------- | -------------- | ----------- | ----------- | --------------------------------- |
|  4  |           C              |    D E B              |    3           |     3       |     2       |    Unknown                        |
|  5  |           C              |    A D E B            |    4           |     3       |     2       |    Invalid                        |
|  6  |           C              |    A D G E B          |    5           |     3       |     2       |    Invalid                        |

## Configuration 
The table below contains information about the configuration of brio_tg and IUT.
| Router                              | AS number (ID) | Update announcement                               |
| :---------------------------------- | :------------- | :------------------------------------------------ |
| [IUT](exp46.router.tpl.md)          | 65020 (C)      | --                                                | 
| [BRIO-1](exp46.brio_tg.as65030.tpl) | 65030 (D)      | 65030 (D) 65040 (E) 65010 (B)                     |
| [BRIO-2](exp46.brio_tg.as65000.tpl) | 65000 (A)      | 65010 (A) 65030 (D) 65040 (E) 65010 (B)           |
|                                     |                | 65010 (A) 65030 (D) 65060 (G) 65040 (E) 65010 (B) |

The IUT must be configured in passive mode. What this means is the router does 
not initiate the BGP connection request, the BRIO traffic generator ```brio_tg``` 
is responsible for session establishment. 

[Back](README.tpl.md)