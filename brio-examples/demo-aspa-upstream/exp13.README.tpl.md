# ASPA Upststeam Verification Example #13
This example contains examples #1, #2, and #3 bundled in a single example.
## Introduction
This demo uses one BGP router instance under test (IUT) that must support ASPA 
processing, two BGP RPKI-IO (BRIO) traffic generators (brio_tg) and one BRIO 
RPKI Validation Cache Test Harness (brio_rc). 

All required files for this experiment have the prefix ```exp13.```

Three BGP UPDATEs are received from a customer or lateral peer. AS 65000 (A)
announces three prefixes, 10.65.0.0/24, 10.65.1.0/24 and 10.65.2.0/24.  
```
65060 (G) <-- 65050 (F) <-- 65020 (C) <-- 65000 (A), 10.65.0.0/24
65060 (G) <-- 65030 (D) <-- 65020 (C) <-- 65000 (A), 10.65.1.0/24
65060 (G) <-- 65030 (D) <-- 65050 (F) <-- 65020 (C) <-- 65000 (A), 10.65.2.0/24
```  

## System Requirements
For this example, two IP addresses are required. Both can be assigned to alias
interfaces.  
IP-Address-1: {IP_AS_65060-13} (G) Used for IUT 
IP-Address-2: {IP_AS_65030-13} (D) Used for BRIO-1 Traffic Generator (brio_tg) and 
              BRIO RPKI Cache (brio_rc)
IP-Address-3: {IP_AS_65050-13} (F) Used for BRIO-2 Traffic Generator (brio_tg)

## Topology
```
                    <-p2p->
                 [F]-------(G) {AS0 ASPA}
       /|\       / \       / .
   C2P  |       /   \     /   .
        |      /     \   /     .
              /<-p2p->\ /<-p2p->.
            (C)-------(D).......[E]
       /|\    \       .           .
   C2P  |      \     .             .
        |       \   .               .
                 \ .                 .
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
* G is the receiving/verifying AS (Implementation Under Test - IUT)
* G receives BGP updates from peers F and D with AS paths: 
  * F C A
  * D C A
  * D F C A
  (simplified to contain only unique ASes)
* AS path lengths = 3 and 4 respectively
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
| AS 65020 (C) | AS 65050 (F)              |
| AS 65030 (D) | AS 65050 (F) AS 65060 (G) |
| AS 65030 (D) | AS 0                      |

See the [Cache File](exp13.brio_rc.script)


## UPDATE and Upstream Path verification result
BGP update received from a customer or lateral peers

| Exp | Receiving / Verifying AS | AS path (Unique ASes) | AS path length | max_up_ramp | min_up_ramp | Upstream Path Verification Result |
| :-: | ------------------------ | --------------------- | -------------- | ----------- | ----------- | --------------------------------- |
|  1  |           G              |    F C A              |    3           |     3       |     3       |    Valid                          |
|  2  |           G              |    D C A              |    3           |     2       |     2       |    Invalid                        |
|  3  |           G              |    D F C A            |    4           |     4       |     3       |    Unknown                        |

## Configuration 
The table below contains information about the configuration of brio_tg and IUT.
| Router                           | AS number (ID) | Update announcement      |
| :------------------------------- | :-------- | :---------------------------- |
| [IUT](exp13.router.tpl.md)          | 65060 (G) | --                         | 
| [BRIO-1](exp13.brio_tg.as65030.tpl) | 65030 (D) | 65030 (D) 65020 (C) 65000 (A) |
|                                     |           | 65030 (D) 65050 (F) 65020 (C) 65000 (A) |
| [BRIO-2](exp13.brio_tg.as65050.tpl) | 65050 (F) | 65050 (F) 65020 (C) 65000 (A) |

The IUT must be configured in passive mode. What this means is the router does 
not initiate the BGP connection request, the BRIO traffic generator ```brio_tg``` 
is responsible for session establishment. 

[Back](README.tpl.md)