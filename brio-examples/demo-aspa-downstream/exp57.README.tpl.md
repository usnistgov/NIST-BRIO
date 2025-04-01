# ASPA Upststeam Verification Example #57
This example contains examples #5, #6, and #7 bundled in a single example.

## Introduction
This demo uses one BGP router instance under test (IUT) that must support ASPA 
processing, two BGP RPKI-IO (BRIO) traffic generators (brio_tg) and one BRIO 
RPKI Validation Cache Test Harness (brio_rc). 

All required files for this experiment have the prefix ```exp57.```

Three BGP UPDATEs are received from a customer or lateral peer. AS 65060 (G)
announces one prefix, 10.65.60.0/22 and AS 65010 (B) announces two prefixes, 
10.65.10.0/23 and 10.65.12.0/23.  
```
65000 (A) <-- 65020 (C) <-- 65050 (F) <-- 65030 (D) <-- 65060 (G), 10.65.60.0/22
65000 (A) <-- 65030 (D) <-- 65060 (G) <-- 65040 (E) <-- 65010 (B), 10.65.10.0/23
65000 (A) <-- 65020 (C) <-- 65030 (D) <-- 65060 (G) <-- 65040 (E) <-- 65010 (B), 10.65.12.0/23
```  

## System Requirements
For this example, three IP addresses are required. All three can be assigned to 
alias interfaces.  
IP-Address-1: {IP_AS_65000-57} (A) Used for IUT 
IP-Address-2: {IP_AS_65020-57} (C) Used for BRIO-1 Traffic Generator (brio_tg) and 
              BRIO RPKI Cache (brio_rc)
IP-Address-3: {IP_AS_65030-57} (D) Used for BRIO-2 Traffic Generator (brio_tg)

## Topology
```                 
                 [F].......(G) {AS0 ASPA}
       /|\       / \       / \
   C2P  |       /   \     /   \
        |      /     \   /     \
              /<-p2p->\ /       \
            (C)-------(D).......[E]
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
* B is the receiving/verifying AS (Implementation Under Test - IUT)
* B receives BGP updates from peer E with AS paths: 
  * E G F C A
  * E G D A
  * E D C A
  * E G D C A
  (simplified to contain only unique ASes)
* AS path lengths = 4 and 5 respectively
* Using the given ASPAs and the method in Section 6.1 and procedure in 
  Section 6.3 of [[ASPA verification](https://datatracker.ietf.org/doc/draft-ietf-sidrops-aspa-verification/)] ...
  * ... the max_up_ramp, min_up_ramp, max_down_ramp, and min_down_ramp values 
        can be retrieved from the table below.
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

See the [Cache File](exp57.brio_rc.script)


## UPDATE and Upstream Path verification result
BGP update received from a customer or lateral peers

| Exp | Receiving / Verifying AS | AS path (Unique ASes) | AS path length | max_up_ramp | max_down_ramp | min_up_ramp | min_down_ramp | Upstream Path Verification Result |
| :-: | ------------------------ | --------------------- | -------------- | ----------- | ------------- | ----------- | ------------- | --------------------------------- |
|  5  |           A              |    C F D G            |    4           |     1       |     4         |    1        |     2         |     Unknow                        |
|  6  |           A              |    D G E B            |    4           |     3       |     2         |    2        |     2         |     Valid                         |
|  7  |           A              |    C D G E B          |    5           |     3       |     1         |    2        |     1         |     Invalid                       |

## Configuration 
The table below contains information about the configuration of brio_tg and IUT.
| Router                              | AS number (ID) | Update announcement                     |
| :---------------------------------- | :------------- | :-------------------------------------- |
| [IUT](exp57.router.tpl.md)          | 65000 (A)    | --                                      | 
| [BRIO-1](exp57.brio_tg.as65020.tpl) | 65020 (C)    | 65050 (F) 65030 (D) 65060 (G)           |
|                                     |              | 65030 (D) 65060 (G) 65040 (E) 65010 (B) |
| [BRIO-2](exp57.brio_tg.as65030.tpl) | 65030 (D)    | 65060 (G) 65040 (E) 65010 (B)           |

The IUT must be configured in passive mode. What this means is the router does 
not initiate the BGP connection request, the BRIO traffic generator ```brio_tg``` 
is responsible for session establishment. 

[Back](README.tpl.md)