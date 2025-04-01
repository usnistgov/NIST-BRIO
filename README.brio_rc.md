# BGP-RPKI-IO Validation Cacne Test Harness
The BGP-RPKI-IO (BRIO) RPKI Cache Test Harness (brio_rc) allows to produce 
synthetic RPKI data and timed / event driven data flow. This means that RPKI 
payloads can be pre-scripted announcements and withdrawals. **It is multi-client capable**

#### What it is not...
It is important to understand that **brio_rc** is only a cache test harness. The 
brio_rc test tool does **NOT** validate ASPA Objects, ROA Objects, BGPsec Keys, 
or perform any validation and data gathering function a Validation Cache does.
The brio_rc test tool allows to script RPKI data that is send from a validation
cache to the client using the roiter to cache protocol [RFC8210](https://datatracker.ietf.org/doc/html/rfc8210)
or its future replacement [8210bis-17](https://datatracker.ietf.org/doc/draft-ietf-sidrops-8210bis/).

### Requirements
BRIO is tested on **Rocky 9** Linux and **Ubuntu 22** Linux distributions. The
OS can be installed as minimal install, the provided install shell script will
install all required libraries using the **dnf** or **apt** package managers.  

### Installation
The installation instructions are provided in the main [README](README.md) file.

## Users Guide (brio_rc)
Tis section explains on how to use the tool, its parameters, as well as s simple
example.

#### Command Line Interface
This section expands on the usage of the Command Line Interface (CLI)
##### Auto Completion vs File Search
Auto completion and File Search can be toggled. The inition key for either modes 
is the **[TAB]** key. 

| Mode | Description |
| ---- | ----------- |
| Auto Completion | Auto completion allows brio_rc to complete the current entered command.|
| File Search     | File search allows to browse through the local drive to select a file. |

To activate either of the modes use the **[*]** key.
Unfortunately it is not possible to switch between the modes while entering a command.


##### Program Parameters
```
Syntax: ./brio_rc [options] [port [script]]
  options:
    -f <script>  A script that has to be executed as soon as
                 the server is started.
    -D <level>   Set the logging level ERROR(3) to DEBUG(7)

  For backward compatibility, a script also can be added after a
  port is specified.! - For future usage, use -f <script> to 
  specify a script!
  If no port is specified the default port 323 is used.

BRIO RPKI Cache Test Harness Version b0.7.0.1
```

##### CLI Commands
The command line interface of brio_rc allows to add and remove RPKI validated 
payloads using add and delete commands. Each command can be stored in a test
file and used as a script.

```
BRIO RPKI Cache Test Harness Version b0.7.0.1

Display Commands:
-----------------
  - verbose
                 Turns verbose output on or off
  - cache
                 Lists the current cache's content
  - version
                 Displays the version of this tool!
  - sessionID
                 Display the current session id
  - help [command]
                 Display this screen or detailed help for the
                 given command!
  - credits
                 Display credits information!

Cache Commands:
-----------------
  - keyLoc <location>
                 The key volt location.
  - empty
                 Empties the cache
  - sessionID <number>
                 Generates a new session id.
  - append <filename>
                 Appends a prefix file's content to the cache
  - add <prefix> <maxlen> <as>
                 Manually add a whitelist entry
  - addNow <prefix> <maxlen> <as>
                 Manually add a whitelist entry without any 
                 delay!
  - addKey <as> <cert file>
                 Manually add a RPKI Router Certificate
  - addASPA <customer-as> <provider-as> [<provider-as>*]
                 Manually add an ASPA object to the cache
  - addASPANow <customer-as> <provider-as> [<provider-as>*]
                 Manually add an ASPA object to the cache without
                 any delay!
  - remove <index> [end-index]
                 Remove one or more cache entries
  - removeNow <index> [end-index]
                 Remove one or more cache entries without any
                 delay!
  - error <code> <pdu|-> <message|->
                 Issues an error report. The pdu contains all
                 real fields comma separated.
  - notify
                 Send a SERIAL NOTIFY to all clients.
  - reset
                 Send a CACHE RESET to all clients.
  - echo [text]
                 Print the given text on the console window.
  - waitFor <client-IP>
                 Wait until the client with the given IP connects.
                 This function times out after 60 seconds.
  - pause [prompt]
                 Wait until any key is pressed. This is mainly
                 for scripting scenarios. If no prompt is used,
                 the default prompt will be applied!

Program Commands:
-----------------
  - quit, exit, \q
                 Quits the loop and terminates the server.
                 This command is allowed within scripts but only
                 as the very last command, otherwise it will be
                 ignored!
  - clients
                 Lists all clients
  - run <filename>
                 Executes a file line-by-line
  - sleep <seconds>
                 Pauses execution
```

##### Run in the Background
It is possible to run brio_rc in the background using a script. Unfortunately 
there is no other way than to stop a brio_rc thread running in the background
using the linu ```kill -9``` comand.  

To send the brio_rc into the background use the following call:
```./brio_rc [port] -f <script>```. The linux command ```fuser``` allows to 
retrieve the process ID (PID) of the running brio_rc instance.  
```fuser -n tcp <selectedPort>``` will return the PID if it is still runnong.

### Example Script
```
echo "This is the BGP-SRx RPKI Cache Test Harness"
echo "==========================================="

echo "Add ROA configurations"

add 10.5.0.0/20  24 65005
add 10.10.0.0/18 20 65010

echo "Add Key Configuration"
keyLoc ../opt/bgp-srx-examples/bgpsec-keys/raw-keys
addKey 65005 65005.cert
addKey 65010 65010.cert

echo "Add ASPA configuration (AS 65030 does not participate in ASPA)"
addASPA 65000 65005
addASPA 65005 65015
addASPA 65015 65005
notify

echo "Current Cache Contents with ROA, Key, and ASPA information:"
cache
```
---
[back](README.md)