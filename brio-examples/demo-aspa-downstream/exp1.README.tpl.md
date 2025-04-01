# ASPA Upstream Verification Example #1

## Introduction
This demo uses one BGP router instance under test (IUT) that must support ASPA 
processing, one BGP RPKI-IO (BRIO) traffic generator (brio_tg) and one BRIO RPKI 
Validation Cache Test Harness (brio_rc). 

All required files for this experiment have the prefix ```exp1.```

A BGP UPDATE is received from a customer or lateral peer. AS 65000 announces the
prefix 10.65.0.0/22 which traverses the following path:  
```
65010 (B) <-- 65040 (E) <-- 65060 (G) <-- 65050 (F) <-- 65020 (C) <-- 65000 (A), 10.65.0.0/22
```  
## System Requirements
For this example, two IP addresses are required. Both can be assigned to alias
interfaces.  
IP-Address-1: {IP_AS_65010-1} (B) Used for IUT 
IP-Address-2: {IP_AS_65040-1} (E) Used for BRIO Traffic Generator (brio_tg) and 
              BRIO RPKI Cache (brio_rc)

## Topology
```
                    <-p2p->
                 [F]-------(G) {AS 0 ASPA}
       /|\       / .       . \ 
   C2P  |       /   .     .   \ 
        |      /     .   .     \ 
              /       . .       \
            (C).......(D).......[E]
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
* B is the receiving/verifying AS (Implementation Under Test - IUT)
* B receives BGP update from E with AS path: E G F C A
  (simplified to contain only unique ASes)
* AS path length = 5
* Using the given ASPAs and the method in Section 6.1 of [[ASPA verification](https://datatracker.ietf.org/doc/draft-ietf-sidrops-aspa-verification/)]:
  * max_up_ramp = 4 ( A C F G )
  * max_down_ramp = 2 ( E G )
  * min_up_ramp = 3 ( A C F )
  * min_down_ramp = 1 ( E )
* Using the procedure in Section 6.3 of [[ASPA verification](https://datatracker.ietf.org/doc/draft-ietf-sidrops-aspa-verification/)]:
  * max_up_ramp + max_down_ramp is not less than AS path length;  
    hence, AS path is not Invalid
  * min_up_ramp + min_down_ramp is less than AS path length;  
    hence, verification outcome: **Unknown**

## ASPA information send by BRIO rpki cache
This table only contains the ASPA information for routers in this experiment
| Customer     | Provider                  |
| :----------- | :------------------------ |
| AS 65000 (A) | AS 65020 (C) AS 65030 (D) |
| AS 65010 (B) | AS 65040 (E)              |
| AS 65020 (C) | AS 65050 (F)              |
| AS 65030 (D) | AS 65050 (F) AS 65060 (G) |
| AS 65060 (G) | AS 0                      |

See the [Cache File](exp1.brio_rc.script)

## UPDATE and Downstream Path verification result
BGP update received from a customer or lateral peers

| Exp | Receiving / Verifying AS | AS path (Unique ASes) | AS path length | max_up_ramp | max_down_ramp | min_up_ramp | min_down_ramp | Upstream Path Verification Result |
| :-: | ------------------------ | --------------------- | -------------- | ----------- | ------------- | ----------- | ------------- | --------------------------------- |
|  1  |    B                     |    E G F C A          |    5           |     4       |     2         |    3        |     1         |     Unknown                       |

## Configuration 
The table below contains information about the configuration of brio_tg and IUT.
| Router                           | AS number (ID) | Update announcement                                 |
| :------------------------------- | :------------- | :-------------------------------------------------- |
| [IUT](exp1.router.tpl.md)        | 65010 (B)      | --                                                  | 
| [BRIO](exp1.brio_tg.as65040.tpl) | 65040 (E)      | 65060 (G) <-- 65050 (F) <-- 65020 (C) <-- 65000 (A) |


The IUT must be configured in passive mode. What this means is the router does 
not initiate the BGP connection request, the BRIO traffic generator ```brio_tg``` 
is responsible for session establishment. 

[Back](README.tpl.md)