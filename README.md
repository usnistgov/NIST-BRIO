# BGP-RPKI-IO (BRIO)


NIST BGP-RPKI-IO (BRIO) is a collection of tools that allows the generation of
synthetic BGP and BGPsec traffic as well as RPKI payloads such as ROA, BGPSec
Key, and ASPA via the RPKI to Router protocol RFC8210 up to RFC 8210bis-15 to be
send to BGP router clients. 

The BRIO RPKI Validation Cache (brio_rc) simulates an RPKI cache that allows to
script RPKI payloads for route origin validation, bgpsec path validation and 
aspa vaidation. The BRIO Traffic Generator (brio_tg) simulates a BGP/BGPsec
capable router that can generate multi hop BGPsec traffic. In addition to 
function as traffic generator brio_tg can test BGPsec algorithm API's for 
performance measurements. Furthermore brio_tg can be used as a BGP Monitor that
displays incoming traffic and can redirect it into text files.

## BRIO Traffic Generator "brio_tg"
The BGP traffic generator brio_tg can produce BGP-4 traffic as well as fully end 
to end signed BGPsec traffic [RFC8205](https://datatracker.ietf.org/doc/html/rfc8205). 
In addition brio_tg allows to test BGPsec crypto algorithms as long as they 
implement the SRxCryproAPI interface. This allows to create performance tests. 

BRIO's brio_tg allows to receive BGP and BGPsec UPDATES and dump them into text 
files for further processing.

The following Modes are supported in this version:

1) **BGP**  : This mode generates BGP updates containing the BGPsec path
              attribute.
2) **GEN-x**: This mode allows to pre-generate bgpsec signed updates for 
              performance testing.
3) **CAPI** : This mode allows to make direct calls into the SRxCryptoAPI (SCA) 
              to perform performance tests of SCA plugins.

The software can be controlled using a configuration file as well as program
parameters. The traffic is generated by passing scripted updates of the following
form either via command line parameters, piping, or scripted within the 
configuration file.

> For More informationon brio_tg, **see brio_tg [README](README.brio_tg.md)**

## BRIO RPKI Validation Cache Test Harness "brio_rc"
The BRIO RPKI Cache Test Harness (brio_rc) allows to produce synthetic RPKI data
and timed / event driven data flow. This means that RPKI payloads can be 
pre-scripted announcements and withdrawals. 

> For More informationon brio_tg, **see brio_rc [README](README.brio_rc.md)**

--------------------------------------------------------------------------------

## Requirements
BRIO is tested on **Rocky 9** Linux and **Ubuntu 22** Linux distributions. The
OS can be installed as minimal install, the provided install shell script will
install all required libraries using the **dnf** or **apt** package managers.  

This package does not provide a BGP router implementation, only the tools to 
test a BGP router implementation that provides the tested capability (ASPA, ROA,
BGPsec). 

## Examples
This project provides a set of examples to illustrate how to use BRIO as well as
test implementations by providing re-producable test runs with pre-defined
outcomes. The audience of BRIO are developers of BGP, BGPsec and RPKI related 
tools as well as users who want to experient with routers that have these RPKI
related secutiry features enabled.

| Examples      | Overview   |
| ------------- | ---------- |
| [ASPA upstream](brio-examples/demo-aspa-upstream/README.tpl.md) | The set of examples provide scenarios for ASPA upstream validation |
| [ASPA downstream](brio-examples/demo-aspa-downstream/README.tpl.md) | The set of examples provide scenarios for ASPA downstream validation |
| BGPsec        | BGPsec examples are not yet transferred, see [NIST BGP SRx](https://github.com/usnistgov/NIST-BGP-SRx) for more information. |
| ROA Validation | ROA Validation examples are not yet transferred, see [NIST BGP SRx](https://github.com/usnistgov/NIST-BGP-SRx) for more information. |

The examples in the above tables are in their Template Form and will be installed
with the examples in the _sandbox/local/opt/brio-examples_ folder. Once installed 
there the template tokens _{TOKEN}_ will be replaced with the actual values for 
the given example.

## Implemented Standards

| Standard  | Title |
| ---  | ----- |
| [RFC 4271](https://datatracker.ietf.org/doc/html/rfc4271) | A Border Gateway Protocol 4 (BGP-4) |
| [RFC 8205](https://datatracker.ietf.org/doc/html/rfc8205) | BGPsec Protocol Specification |
| [RFC 8208](https://datatracker.ietf.org/doc/html/rfc8205) | BGPsec Algorithms, Key Formats, and Signature Formats |
| [RFC 8210](https://datatracker.ietf.org/doc/html/rfc8205) | The Resource Public Key Infrastructure (RPKI) to Router Protocol, Version 1 |
| [draft-ietf-sidrops-8210bis-17](draft-ietf-sidrops-8210bis-17) | The Resource Public Key Infrastructure (RPKI) to Router Protocol, Version 2, draft-ietf-sidrops-8210bis-17 |
| [draft-ietf-sidrops-aspa-verification-22](https://datatracker.ietf.org/doc/draft-ietf-sidrops-aspa-verification/22/) | BGP AS_PATH Verification Based on Autonomous System Provider Authorization (ASPA) Objects |

## BRIO Installation
To compile and build BRIO just call the included build shell script. This 
script allows to specify the folder where the software package will be installed
in. ```./buildBRIO.sh -i <sandbox name>``` will build and install the framework 
in the provided <sandbox> folder.  
The installed code will be installed in the ```<sandbox name>/local``` folder.

### Errors
In most case errors occur out of threee reasons:
1. **Missing Libraries:** The CONTENT file contains a list of required libraries.
   Also the provided shell script ```install_dependencies.sh``` can be used to 
   install the necessary libraries. We want to caution because this script will
   modify the Linux install. 
2. **Automake Framework Outdated:** To solve this you need to install the 
   automake framework. Then enter the package that could failed and call
   ```autoreconf -i -f```. This forces the automake framework to be refreshed 
   and the install should pass.
3. **Examples Fail due to missing IP addresses:** Restart the build file and
   pass the parameter -EI. Also it is possible to go into the brio-examples
   folder and call ```./configure.sh -I``` followed by ```./install.sh -I```.
   Alias interfaces can be identifies by the brio label (e.g.: ```ens18:brio0:```) 

Please familiarize yourself with the shell scripts by using the parameter **-?**

If the above steps are unsuccessful, then drop us an email. (see below)

## Known Issues
* At this time the brio_rc cache does not use encrypted traffic. 
* The auto install of IP addresses on docker instances fails.  

## Contributing
Please read [How To Contribute](CONTRIBUTING.md) for details on how to contribute to the project.

## Authors & Main Contributors
Oliver Borchert (Lead)  
Kotikalapudi Sriram  
Kyehwan Lee  
Doug Montgomery  

## Copyright
For license information see the [LICENSE](LICENSE) file.

## Contacts
For information, questions, or comments, contact us by sending an email to itrg-contact@list.nist.gov