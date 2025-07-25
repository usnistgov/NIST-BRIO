
#!/bin/bash
VERSION="b0.7.1.2-0015"

# This is needed for the configuration sourcing
BRIO_ROUTER_WRAPPER="$0 V$VERSION"

echo
echo "BRIO ROUTER WRAPPER: $BRIO_ROUTER_WRAPPER"
echo 

################################################################################
# The command to use
CMD=""
# The experiment to be run - it is assumed that start is not given, then the 
# first parameter is start
EXPERIMENT=""
# The wrapper configuration
WRAPPER_CFG=""

# BGP Router Port - Depends on the router if it accepts it 
#                   overwrites setting of rtr-wrapper.cnf
P_CP=
# BGP Cache Port - Depends on the router if it accepts it
#                  overwrites setting of "rtr-wrapper.cnf"
P_RP=
# Router Wrapper cfg 
P_CFG=
# BGP Parameters
P_BGP=( )

## Check if parameter $1 has an additional value $2
function checkParam()
{
  if [ "$2" == "" ] ; then
    echo "Parameter '$1' needs an additional value!"
    exit 1
  fi
}

# {SECTION: Parameters}
while [ "$1" != "" ] ; do
  case "$1" in 
    "start" | "stop" | "viewTable")
        checkParam $1 $2
        CMD="$1"
        if [ "$1" != "viewTable" ] ; then
          CMD+="Router"
        fi
        shift
        EXPERIMENT="$1"
        ;;
    "-?")
        CMD="help"
        break
        ;;
    "-rp")
        checkParam $1 $2
        shift
        P_RP="$1"
        ;;
    "-cp")
        checkParam $1 $2
        shift
        P_CP="$1"
        ;;
    "-cfg")
        checkParam $1 $2
        shift
        P_CFG=$1
        ;;
    *) 
       # Add as router parameter
       P_BGP+=( $1 )
       ;;
  esac
  shift
done

##############################################################################
#{SEGMENT: Helper functions}
OK="OK"
FAILED="FAILED"

# Print $OK or $FAILED and returns $1
function match()
{
  local _retVal=1
  if [ "$1" == "$2" ] ; then
    echo $OK
  else
    echo $FAILED
    shift
    shift
    $@
  fi

  return $_retVal
}

################################################################################
# {SECTION: Detect etc Folders}

# attempt to identidy the etc folder containing experiments
LOCAL_ETC_FOLDER=$(pwd | sed -E "s#^(.*)/opt/brio-examples/.*#\1/etc#g")
DEMO_ETC_FOLDER=$(pwd | sed -E "s#^(.*)(/opt/brio-examples)/.*#\1\2/etc#g")

#{SEGMENT: Configuration}
################################################################################
##                                                                            ##
##  This section wil be overwritten by the configuration file                 ##

# Set to configurred!!
CONFIGURED=0

# Timeout for shutdown
TIMEOUT=5

# This is used in case the router out-sourced validation 
# (e.g. NIST-BGP-SRx: router: QuaggaSRx and validator: SRx-Server)
VALIDATOR_BIN=
## Additional parameters for the validator.
VALIDATOR_PARAMS=

# The validator's program parameter to set the configuration file. 
VALIDATOR_CFG_PARAM=
# The validator configuration file
VALIDATOR_CFG=
# The validato's program parameter to set the validator port. 'srx-server -> 17900' 
VALIDATOR_PORT_PARAM=
# The default router BGP Port (RFC 4271)
VALIDATOR_PORT=
# The validator's program parameter to set the rpki validation cache port. 
VALIDATOR_RPKI_CACHE_PARAM=
# The port the RPKI validation cache listens on
VALIDATOR_RPKI_CACHE_PORT=

# The router binary - here as example just netcat to have something to listen on
# the router port.
BGP_RTR_BIN="nc "

# The router's program parameter to set the routers configuration file. 'often -f'
BGP_RTR_CFG_PARAM=
# The router configuration file
BGP_RTR_CFG=
# The router's program parameter to set the BGP port. 'often -p' 
BGP_RTR_PORT_PARAM="l "
# The default router BGP Port (RFC 4271)
BGP_RTR_PORT=179
# The router's program parameter to set the rpki validation cache port. 
BGP_RTR_RPKI_CACHE_PARAM=
# The port the RPKI validation cache listens on
BGP_RTR_RPKI_CACHE_PORT=

# The command to show the bgp table (use quagga for a quagga command)
VIEW_TABLE_COMMAND=

# Try to find a router configuration if non is provided
if [ "$P_CFG" == "" ] ; then 
  scan_folder=( "." "$LOCAL_ETC_FOLDER" "$DEMO_ETC_FOLDER" )
  for _folder in ${scan_folder[@]} ; do
    _wrapper_cfg="$_folder/rtr-wrapper.cfg"
    echo -n "Scan for '$_wrapper_cfg'..."
    if [ -e "$_folder/rtr-wrapper.cfg" ] ; then
      WRAPPER_CFG="$_wrapper_cfg"
      echo "found!"
      break
    else
      echo "not found"
    fi
  done
else
  WRAPPER_CFG="$P_CFG"
fi

if [ "$WRAPPER_CFG" != "" ] ; then
  if [ ! -e $WRAPPER_CFG ] ; then
    echo "Provided rtr-wrapper.cfg not found!"
    exit 1
  else
    echo -n "Load configuration [$WRAPPER_CFG]..."
    source $WRAPPER_CFG &>/dev/null
    match $? 0
  fi
else
  echo "Note: No configuration file found!"
fi
    
if [ $CONFIGURED -eq 0 ] ; then
  echo "This is a demo wrapper for the BGP router. It needs to be"
  echo "configured/rewritten to be correctly used with the BGP router!"
  echo "Each function will have the experiment name as 1st parameter"
  echo
fi

if [ "$P_RP" != "" ] ; then
  if [ "$BGP_RTR_PORT_PARAM" == "" ] ; then
    echo "WARNING: BGP router port provided via command line without scripting"
    echo "         the port flag in $WRAPPER_CFG!"
  fi
  BGP_RTR_PORT=$P_RP
fi

if [ "$P_CP" != "" ] ; then
  if [ "$BGP_RTR_RPKI_CACHE_PARAM" == "" ] ; then
    echo "WARNING: BGP router's rpki cache port provided via command line"
    echo "         without scripting the port flag in $WRAPPER_CFG!"
  fi
  BGP_RTR_PORT=$P_RP
fi

# Fill the BGP router params
BGP_RTR_PARAMS="$( echo ${P_BGP[@]} )"

#                                                                              #
##                                                                            ##
##                                                                            ##
################################################################################


#{SEGMENT: Router Control}

## This function needs to include the router start command.
## $1..$n Router parameters provided in original start file
## Return 1 on failure and 0 on success
function startRouter()
{
  echo "#################################################################"
  echo "Start the router for experiment: '$EXPERIMENT'"
  echo "#################################################################"
  echo "##                                                             ##"
  echo "## Add specific router start code here!                        ##"
  echo "##                                                             ##"
  echo "#################################################################"
  echo "### Start netcat to listen on port $BGP_RTR_PORT - blocking call         ##"
  echo "#################################################################"

  ##############################################################################
  ##                                                                          ##
  ##                                                                          ##
  #  This code below can be replaced using other starter code.                 #
  #                                                                            #
  local _pid
  local _pid_val=( )
  local _ret_val=( )

  # Start a validator {SEGMENT: Start validator}
  if [ "$VALIDATROR_BIN" != "" ] ; then
    CALL=(  $VALIDATOR_BIN $VALIDATOR_CFG_PARAM $VALIDATOR_CFG
            $VALIDATOR_PORT_PARAM $VALIDATOR_PORT 
            $VALIDATOR_RPKI_CACHE_PARAM $VALIDATOR_RPKI_CACHE_PORT 
            $VALIDATOR_PARAMS
    )
    echo "CALL: ${CALL[@]}"
    echo -n "Start validator '$VALIDATROR_BIN' "
    ${CALL[@]} & &>/dev/null &

    _pid=$!
    _pid_val+=( $_pid )
    echo -n "PID($_pid)..."
    match 0 0
  fi

  # Start a router {SEGMENT: Start BGP router}
  if [ "$BGP_RTR_BIN" != "" ] ; then
    CALL=(  $BGP_RTR_BIN $BGP_RTR_CFG_PARAM $BGP_RTR_CFG
            $BGP_RTR_PORT_PARAM $BGP_RTR_PORT
            $BGP_RTR_RPKI_CACHE_PARAM $BGP_RTR_RPKI_CACHE_PORT
            $BGP_RTR_PARAMS
    )
    echo "CALL: ${CALL[@]}"
    echo -n "Start router...$BGP_RTR_BIN' "
    ${CALL[@]} & &>/dev/null &

    _pid=$!
    _pid_val+=( $_pid )
    echo -n "PID($_pid)..."
    match 0 0
  fi

  # Prevent disfunct processes
  trap '
    for _pid in "${_pid_val[@]}"; do
      if kill -o "$pid" 2>/dev/null; then
        echo "Killing $_pid"
        kill "$_pid"
        sleep 2
        kill -9 "$_pid" 2>/dev/null
      fi
    done
  ' EXIT

  for _pid in "${_pid_val[@]}"; do
    if wait "$_pid"; then
      _ret_val+=( 0 )   # Success
    else
      _ret_val+=( $? )  # Capture actual exit code
    fi
  done

  # Print results
  local _retVal=0
  for _idx in "${!_pid_val[@]}"; do
    echo -n "Process ${_pid_val[$_idx]} returned"
    echo " with ${_ret_val[$_idx]}"
    if [ ${_ret_val[$_idx]} -ne 0 ] ; then
      _retVal=1
    fi
  done

  return $_retVal
}

## This function needs to include the routers display table command
## Return 1 on failure and 0 on success
function viewTable()
{
  echo "#################################################################"
  echo "View table of experiment: '$EXPERIMENT'"
  echo "#################################################################"
  echo "##                                                             ##"
  echo "## Add the view table command here!                            ##"
  echo "##                                                             ##"
  echo "#################################################################"

  #echo "Display the routing table:"
  #echo "=========================="
  
  if [ "$VIEW_TABLE_COMMAND" == "quagga" ] ; then
    { sleep 1; echo "zebra"; sleep 1; echo "enable"; sleep 1; \
      echo "show ip bgp"; sleep 3; } | telnet localhost 2605
  else
    # Just execute what is stored
    $VIEW_TABLE_COMMAND
  fi

  return 1
}

## This function needs to include the routers shutdown command
## Return 1 on failure and 0 on success
function stopRouter()
{
  echo "#################################################################"
  echo "Shutdown router of experiment: '$EXPERIMENT'"
  echo "#################################################################"
  echo "##                                                             ##"
  echo "## Add shutdown of the router!                                 ##"
  echo "##                                                             ##"
  echo "#################################################################"

  # SAMPLE CODE
  local _retVal=0

  if [ "$BGP_RTR_BIN $BGP_RTR_PORT" != " " ] ; then
    echo -n "Stopping Router..."
    _timeout=$TIMEOUT
    while [ $_timeout -gt 0 ] ; do
      PID=$(pidof -f "$BGP_RTR_BIN $BGP_RTR_PORT")
      if [ $? -eq 0 ] ; then
        kill -9 $PID
        echo -n "."
        sleep 1
        _timeout=$(($_timeout-1))      
      fi
    done
    if [ $_timeout -gt 0 ] ; then _timeout=0 ; else _timeout=1 ; fi
    match $_timeout 0
    if [ $? -eq 1 ] ; then
      echo "Timed out!"
      _retVal=1
    fi
  fi

  if [ "$VALIDATROR_BIN $CACHE_PORT" != " " ] ; then
    echo -n "Stopping Validator..."
    _timeout=$TIMEOUT
    while [ $_timeout -gt 0 ] ; do
      PID=$(pidof -f "$BGP_RTR_BIN $BGP_RTR_PORT")
      if [ $? -eq 0 ] ; then
        kill -9 $PID
        echo -n "."
        sleep 1
        _timeout=$(($_timeout-1))      
      fi
    done
    if [ $_timeout -gt 0 ] ; then _timeout=0 ; else _timeout=1 ; fi
    match $_timeout 0
    if [ $? -eq 1 ] ; then
      echo "Timed out!"
      _retVal=1
    fi
  fi

  return $_retVal
}

#{SEGMENT: Help}

# Just show the syntax, does not exit!
function syntax()
{
  echo "Syntax: $0 (start|stop|viewTable) <experiment> [-rp <router port>] " \
                   "[-cp <cache port>] [-conf <config>] [-?] [params]"
  echo
  echo "Parameters:"
  echo "  start <experiment> [params]"
  echo "       Start the router with router specific parameters. *"
  echo
  echo "  stop <experiment> [params]"
  echo "       Start the router with router specific parameters. *"
  echo
  echo "  viewTable <experiment> [params]"
  echo "       View the router's table with router specific parameters. *"
  echo
  echo "  * The router port (-rp) and cache port (-cp) parameters might need to"
  echo "    be specified in the routers configuration file."
  echo
  echo "  -rp    Specify the routers port (default  179)"
  echo "  -cp    Specify the cache port (default  50000)"
  echo "  -conf <config>"
  echo "         Specify the configuration for the router starter script!"
  echo
  echo "  params A list of router specific parameters"
  echo
  echo "  If no parameter (start|stop|viewTable) is provied 'start' is assumed!"
  echo
}

#{SEGMENT: Main}

if [ "$EXPERIMENT" == "" ] ; then
  echo "WARNING: No experiment specified!"
  EXPERIMENT="unknown"
fi

echo "Provided Router Port...: $BGP_RTR_PORT"
echo "Provided Cache Port....: $CACHE_PORT"
echo

# {SEGMENT: Main}
_retVal=0
case "$CMD" in
  "startRouter")
    $CMD ${P_BGP[@]}
    _retVal=$?
    ;;
  "stopRouter" | "viewTable" )
    $CMD
    ;;
  "help")
    syntax
    ;;
  *) 
    echo "Nothing to do - use -? for more information!"
    exit 1
    ;;     
esac
echo
