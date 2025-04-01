# ASPA Upstream Verification Example #7

## Introduction
This demo uses one BGP router instance under test (IUT) that must support ASPA 
processing, one BGP RPKI-IO (BRIO) traffic generator (brio_tg) and one BRIO RPKI 
Validation Cache Test Harness (brio_rc). 

All required files for this experiment have the prefix ```exp7.```

A BGP UPDATE is received from a customer or lateral peer. AS 65010 announces the
prefix 10.65.10.0/22 which traverses the following path:  
```
65000 (A) <-- 65020 (C) <-- 65030 (D) <-- 65060 (G) <-- 65040 (E) <-- 65010 (B), 10.65.10.0/22
```  
## System Requirements
For this example, two IP addresses are required. Both can be assigned to alias
interfaces.  
IP-Address-1: {IP_AS_65000-7} (A) Used for IUT 
IP-Address-2: {IP_AS_65020-7} (C) Used for BRIO Traffic Generator (brio_tg) and 
              BRIO RPKI Cache (brio_rc)

## Topology
```
                 [F].......(G) {AS 0 ASPA}
       /|\       . .       / \ 
   C2P  |       .   .     /   \ 
        |      .     .   /     \ 
              .<-p2p->. /       \
            (C)-------(D).......[E]
       /|\    \       .           \
   C2P  |      \     .             \  
        |       \   .               \ 
                 \ .                 \ 
                 (A)                 (B)

[E] and [F] have no ASPAs  
--/--\-- Updates are sent across this path
........ Path available but no update traverse for this example
```
### Legend:

| Label | Description                  | \| | Node | AS Number | Prefix        |
| ----- | ---------------------------- | -- | :--: | :-------: | :-----------  |
| C2P   | Customer to Peer             | \| |  A   | AS 65000  | 10.65.0.0/22  |
| p2p   | Peer to Peer (lateral peers) | \| |  B   | AS 65010  | 10.65.10.0/22 |
| [X]   | Do not have ASPAs            | \| |  C   | AS 65020  | 10.65.20.0/22 |
|       |                              | \| |  D   | AS 65030  | 10.65.30.0/22 |
|       |                              | \| |  E   | AS 65040  | 10.65.40.0/22 |
|       |                              | \| |  F   | AS 65050  | 10.65.52.0/22 |
|       |                              | \| |  G   | AS 65060  | 10.65.60.0/22 |

## Example Scenario
* A is the receiving/verifying AS (Implementation Under Test - IUT)
* A receives BGP update from C with AS path: C D G E B
  (simplified to contain only unique ASes)
* AS path length = 5
* Using the given ASPAs and the method in Section 6.1 of [[ASPA verification](https://datatracker.ietf.org/doc/draft-ietf-sidrops-aspa-verification/)]:
  * max_up_ramp = 3 ( B E G )
  * max_down_ramp = 1 ( C )
  * min_up_ramp = 2 ( B E )
  * min_down_ramp = 1 ( C )
* Using the procedure in Section 6.3 of [[ASPA verification](https://datatracker.ietf.org/doc/draft-ietf-sidrops-aspa-verification/)]:
  * max_up_ramp + max_down_ramp is less than AS path length;  
    hence, AS verification outcome: **Invalid**

## ASPA information send by BRIO rpki cache
This table only contains the ASPA information for routers in this experiment
| Customer     | Provider                  |
| :----------- | :------------------------ |
| AS 65000 (A) | AS 65020 (C) AS 65030 (D) |
| AS 65010 (B) | AS 65040 (E)              |
| AS 65020 (C) | AS 65050 (F)              |
| AS 65030 (D) | AS 65050 (F) AS 65060 (G) |
| AS 65060 (G) | AS 0                      |

See the [Cache File](exp7.brio_rc.script)

## UPDATE and Downstream Path verification result
BGP update received from a customer or lateral peers

| Exp | Receiving / Verifying AS | AS path (Unique ASes) | AS path length | max_up_ramp | max_down_ramp | min_up_ramp | min_down_ramp | Upstream Path Verification Result |
| :-: | ------------------------ | --------------------- | -------------- | ----------- | ------------- | ----------- | ------------- | --------------------------------- |
|  7  |    A                     |    C D G E B          |    5           |     3       |     1         |    2        |     1         |     Invalid                       |

## Configuration 
The table below contains information about the configuration of brio_tg and IUT.
| Router                           | AS number (ID) | Update announcement                                 |
| :------------------------------- | :------------- | :-------------------------------------------------- |
| [IUT](exp7.router.tpl.md)        | 65000 (A)      | --                                                  | 
| [BRIO](exp7.brio_tg.as65020.tpl) | 65020 (C)      | 65030 (D) <-- 65060 (G) <-- 65040 (E) <-- 65010 (B) |


The IUT must be configured in passive mode. What this means is the router does 
not initiate the BGP connection request, the BRIO traffic generator ```brio_tg``` 
is responsible for session establishment. 

[Back](README.tpl.md)