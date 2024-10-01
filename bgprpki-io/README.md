# BGP-RPKI-IO (BRIO)
BGP-RPKI-IO (BRIO) is a collection of tools that allow to generate synthetic 
BGP, BGPsec, and RPKI payloads such as ROA, BGPSec Key, and ASPA via the RPKI 
to Router protocol RFC8210 up to RFC 8210bis-15 to be send to router clients. 

The BRIO Cache (brio_cache) simulates an RPKI cache that allows to script RPKI 
payloads for route origin validation, bgpsec path validation and aspa vaidation. 


## Traffic Generator "brio"
The BGP traffic generator (brio) can produce BGP-4 traffic as well as fully 
end to end signed BGPSec traffic. Furthermore it allows to test BGPsec crypto
algorithms as long as they implement the SRxCryproAPI interface. This allows to 
create performance tests. 

In addition BRIO allows to receive BGP and BGPSec UPDATES and dump them into
text files.

The following Modes are supported in this version:

1) BGP  : This mode generates BGP updates containing the BGPsec path attribute

2) GEN-x: This mode allows to pre-generate bgpsec signed updates for performance
          testing.

3) CAPI : This mode allows to make direct calls into the SRxCryptoAPI to perform
          performance tests of API plugins.

The software can be controlled using a configuration file as well as program
parameters. The traffic is generated by passing scripted updates of the following
form either via command line parameters, piping, or scripted within the 
configuration file.

**Update Format:   "\<prefix> [ , ( \<ASN>[ [ p | P ] \<pCount> ] )\* ]"**

|    |    |    |   |
| -- | -- | -- | --|
| Examples: | BGP-RPKI-IO as ASN 10
| | "10.0.10.0/24"          | -> | path: 10  
| | "10.0.10.0/24, 20 30"   | -> | path: 10 20 30  
| | "10.0.10.0/24, 20p3 30" | -> | path: 10 20 20 20 30  

### Order of loading updates
Updates are loaded in the following manner:  
1. Session Updates
2. Global Updates  
3. binary File Updates  
4. STDIO Updates  
5. Piped Updates  

### Private and Public Keys:  
Private and public keys are generated using the key generation functions of the
SRxCryptoAPI. Important here is the key-list.txt file. BGPsec-IO reads this file 
to pre-load all keys for processing. For more info on keys see the SRxCryptoAPI

For all different settings, please call **``brio -?``** or check the 
configuration file.

### BRIO as traffic sink
It is possible to use ```brio``` as traffic receiver. All BGP / BGPSec traffic 
can be printed out on receive in two forms, simple and regular. The regular form 
follows the Wireshark format where as the simple form presents one PDU per line.

## BRIO_CACHE
BRIO Cache is the RPKI Cache Test Harness. This software allows to produce 
synthetic RPKI data and timed / event driven data flow. This means that RPKI
payloads can be pre-scripted announcements and withdrawals. 

## Requirements
The BRIO Framework is tested on Linux Rocky 9 installations. 

## Installation
To compile and build BRIO just call the included build shell script. This 
script allows to specify the folder where the software package will be installed
in. ./build... -i sandbox  will build and install the framework inthe folder
sandbox. The installed code will be in the embedded local filder.

## Contact &  ???
For Questions and/or BUG reports please email to bgpsec-dev@nist.gov