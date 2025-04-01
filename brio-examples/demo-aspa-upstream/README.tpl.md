# ASPA-based AS path verification (UPSTREAM)

## Introduction
This set of examples will in detail ASPA upstream verification. Five separate 
experiments are scripted that will show the upstream verification and results.
Two examples are scripted with a detailed explanation on the verification using 
a single update each. Three examples are scripted with each containing a set of 
updates being received from different peers within the topology. 
  
This set of examples requires one BGP implmentation under test (IUT) that must 
support ASPA processing, two BGP RPKI-IO (BRIO) traffic generators (brio_tg) and 
one BRIO RPKI Validation Cache Test Harness (brio_rc).  
  
The pre-requisite of this example topology and configuration is that the host 
system needs to have three IP addresses configured, one for each BRIO instance 
and one for one for the router implementation under test (IUT). The BRIO router 
cache will use the same IP address as as one of the brio_tg instances.
  
## System Requirements

In case communication problems arrise, please open the ports 179 (BGP) and 50000
(brio_rc) in the firewall. The examples are configured with the assumption that
all components are running on the same system. If this is not the case, then
reachablility must be assured within the systems (e.g. ping and firewall).

## Topology
  
The topology shown is ground truth, not deducted from ASPAs. If ASPA exists for 
an AS, it only asserts if another AS is its Provider or Not Provider.  
```
                    <-p2p->
                 [F]-------(G) {AS0 ASPA}
       /|\       / \       / \
   C2P  |       /   \     /   \
        |      /     \   /     \
              /<-p2p->\ /<-p2p->\
            (C)-------(D)-------[E]
       /|\    \       /           \
   C2P  |      \     /             \
        |       \   /               \
                 \ /                 \
                 (A)                 (B)

E and F have no ASPAs
```

### Legend:

| Label | Description                  | \| | Node | AS Number | Prefix*       |
| ----- | ---------------------------- | -- | :--: | :-------: | :-----------  |
| C2P   | Customer to Peer             | \| |  A   | AS 65000  | 10.0.0.0/22   |
| p2p   | Peer to Peer (lateral peers) | \| |  B   | AS 65010  | 10.65.10.0/22 |
| [X]   | Do not have ASPAs            | \| |  C   | AS 65020  | 10.65.20.0/22 |
|       |                              | \| |  D   | AS 65030  | 10.65.30.0/22 |
|       |                              | \| |  E   | AS 65040  | 10.65.40.0/22 |
|       |                              | \| |  F   | AS 65050  | 10.65.52.0/22 |
|       |                              | \| |  G   | AS 65060  | 10.65.60.0/22 |

\* Each prefix is covered by a ROA with max length of 22

## ASPA Configuration
| Customer     | Provider                  |
| :----------- | :------------------------ |
| AS 65000 (A) | AS 65020 (C) AS 65030 (D) |
| AS 65010 (B) | AS 65040 (E)              |
| AS 65020 (C) | AS 65050 (F)              |
| AS 65030 (D) | AS 65050 (F) AS 65060 (G) |
| AS 65060 (G) | AS 0                      |

## Examples of Upstream Path verification
BGP update received from a customer or lateral peers.

| Exp | Receiving / Verifying AS | AS path (Unique ASes) | AS path length | max_up_ramp | min_up_ramp | Upstream Path Verification Result |
| :-: | ------------------------ | --------------------- | -------------- | ----------- | ----------- | --------------------------------- |
| [[13](exp13.README.tpl.md)]    |                       |                |             |             |                                   |
|  1  |           G              |    F C A              |    3           |     3       |     3       |    Valid                          |
| [[2](exp2.README.tpl.md)]* | G |    D C A              |    3           |     2       |     2       |    Invalid                        |
|  3  |           G              |    D F C A            |    4           |     4       |     3       |    Unknown                        |
| [[46](exp46.README.tpl.md)]    |                       |                |             |             |                                   |
| [[4](exp4.README.tpl.md)]* | C |    D E B              |    3           |     3       |     2       |    Unknown                        |
|  5  |           C              |    A D E B            |    4           |     3       |     2       |    Invalid                        |
|  6  |           C              |    A D G E B          |    5           |     3       |     2       |    Invalid                        |
| [[79](exp79.README.tpl.md)]    |                       |                |             |             |                                   |
|  7  |           D              |    A C F              |    3           |     2       |     1       |    Invalid                        |
|  8  |           D              |    A C F G            |    4           |     1       |     1       |    Invalid                        |
|  9  |           D              |    E B                |    2           |     2       |     2       |    Valid                          |

\* These examples are explained in more detail.

---
[Back](../README.md)