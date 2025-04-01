# ASPA Upststeam Verification Example #46
This example contains examples #7, #8, and #9 bundled in a single example.
## Introduction
This demo uses one BGP router instance under test (IUT) that must support ASPA 
processing, two BGP RPKI-IO (BRIO) traffic generators (brio_tg) and one BRIO 
RPKI Validation Cache Test Harness (brio_rc). 

All required files for this experiment have the prefix ```exp79.```

Three BGP UPDATEs are received from a customer or lateral peer. AS 65050 (F)
announces 10.65.52.0/22, AS 65060 (G) announces 10.65.60.0/22, 
and AS 65010 (B) announces 10.65.10.0/22.  
```
65030 (D) <-- 65000 (A) <-- 65020 (C) <-- 65050 (F), 10.65.52.0/22
65030 (D) <-- 65000 (A) <-- 65020 (C) <-- 65050 (F) <-- 65060 (G), 10.65.60.0/22
65030 (D) <-- 65040 (E) <-- 65010 (B), 10.65.10.0/22
```  

## System Requirements
For this example, two IP addresses are required. Both can be assigned to alias
interfaces.  
IP-Address-1: {IP_AS_65030-79} (D) Used for IUT 
IP-Address-2: {IP_AS_65000-79} (A) Used for BRIO-1 Traffic Generator (brio_tg) and 
              BRIO RPKI Cache (brio_rc)
IP-Address-3: {IP_AS_65040-79} (E) Used for BRIO-2 Traffic Generator (brio_tg)

## Topology
```
                    <-p2p->
                 [F]-------(G) {AS0 ASPA}
       /|\       / .       . .
   C2P  |       /   .     .   .
        |      /     .   .     .
              /<-p2p->. .<-p2p->.
            (C).......(D)-------[E]
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
* D is the receiving/verifying AS (Implementation Under Test - IUT)
* D receives BGP updates from peers A and E with AS paths: 
  * A C F
  * A C F G
  * E B
  (simplified to contain only unique ASes)
* AS path lengths = 2, 3 and 4 respectively
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

See the [Cache File](exp79.brio_rc.script)

## UPDATE and Upstream Path verification result
BGP update received from a customer or lateral peers

| Exp | Receiving / Verifying AS | AS path (Unique ASes) | AS path length | max_up_ramp | min_up_ramp | Upstream Path Verification Result |
| :-: | ------------------------ | --------------------- | -------------- | ----------- | ----------- | --------------------------------- |
|  7  |           D              |    A C F              |    3           |     2       |     1       |    Invalid                        |
|  8  |           D              |    A C F G            |    4           |     1       |     1       |    Invalid                        |
|  9  |           D              |    E B                |    2           |     2       |     2       |    Valid                          |

## Configuration 
The table below contains information about the configuration of brio_tg and IUT.
| Router                              | AS number (ID) | Update announcement                     |
| :---------------------------------- | :------------- | :-------------------------------------- |
| [IUT](exp79.router.tpl.md)          | 65030 (D)      | --                                      | 
| [BRIO-1](exp79.brio_tg.as65000.tpl) | 65000 (A)      | 65010 (A) 65020 (C) 65050 (F)           |
|                                     |                | 65010 (A) 65020 (C) 65050 (F) 65060 (G) |
| [BRIO-2](exp79.brio_tg.as65040.tpl) | 65040 (E)      | 65040 (E) 65010 (B)                     |

The IUT must be configured in passive mode. What this means is the router does 
not initiate the BGP connection request, the BRIO traffic generator ```brio_tg``` 
is responsible for session establishment. 

[Back](README.tpl.md)