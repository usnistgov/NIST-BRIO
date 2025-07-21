#!/bin/bash

VERSION="b0.7.1.2-0009"

CONFIGURED=0
# attempt to identidy the etc folder containing experiments
ETC_FOLDER=$(pwd | sed -e "s#\(.*\)/opt/brio-examples/bin#\1#g")

if [ $CONFIGURED -eq 0 ] ; then
  echo "This is a demo file for the router to be started. It needs to be"
  echo "configured/rewritten for the used router."
  echo "Each function will have the experiment name as 1st parameter"
  echo
fi

# Will be set by the first parameter
EXPERIMENT=
# The command, what to do
CMD=

#{SEGMENT: Configuration}

if [ -e router.cfg ] ; then
  source router.cfg
fi

# If the validator is not part of the router (normally not needed)
if [ "$VALIDATROR_BIN" == "" ] ; then VALIDATROR_BIN="nc -l " ; fi
# the router binary (netcat mok to listen on port)
if [ "$ROUTER_BIN"  == "" ] ; then ROUTER_BIN="nc -l " ; fi
# The default router BGP Port (RFC 4271)
if [ "$ROUTER_PORT" == "" ] ; then ROUTER_PORT=179 ; fi
# The default cache port (RFC 8210)
if [ "$CACHE_PORT" == "" ] ; then CACHE_PORT=323 ; fi

# The default cache port (RFC 8210)
if [ "$TIMEOUT" == "" ] ; then TIMEOUT=5 ; fi

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

# This function prints out all given parametes and ends with exit 2
function error()
{
  echo "ERROR: $@"
  exit 1
}

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
  echo "### Start netcat to listen on port $ROUTER_PORT - blocking call         ##"
  echo "#################################################################"

  local _config=""
  local _found=0

  echo -n "Lookup configuration file...["
  if [ -e $ETC_FOLDER ] ; then
    _config="$ETC_FOLDER/bgpd-$(echo $EXPERIMENT).conf"
    if [ -e $_config ] ; then
      found=1
      echo "not "
    fi
  fi
  echo "found]"

  ##############################################################################
  ## This code below can be replaced using other starter code.

  local _pid_val=
  local _pid_rtr=
  local _val_ret=0
  local _rtr_ret=0

  # Start a validator
  if [ "$VALIDATROR_BIN" != "" ] ; then
    echo -n "Start validator...'$VALIDATROR_BIN $CACHE_PORT'"
    $VALIDATROR_BIN $CACHE_PORT &>/dev/null &
    _pid_val=$!
    match 0 0
  fi

  if [ "$ROUTER_BIN" != "" ] ; then
    echo -n "Start router...'$ROUTER_BIN $ROUTER_PORT'"
    $ROUTER_BIN $ROUTER_PORT &>/dev/null &
    _pid_rtr=$!
    match 0 0
  fi

  # Prefvent disfunct processes
  trap "kill $_pid_val $_pid_rtr" EXIT

  wait $_pid_rtr
  _val_ret=$?

  wait $_pid_val
  _rtr_ret=$?

  return $(($_val_ret | $_rtr_ret))
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
  #{ sleep 1; echo "zebra"; sleep 1; echo "enable"; sleep 1; echo "show ip bgp"; sleep 3; } | telnet localhost 2605

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

  if [ "$ROUTER_BIN $ROUTER_PORT" != " " ] ; then
    echo -n "Stopping Router..."
    _timeout=$TIMEOUT
    while [ $_timeout -gt 0 ] ; do
      PID=$(pidof -f "$ROUTER_BIN $ROUTER_PORT")
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
      PID=$(pidof -f "$ROUTER_BIN $ROUTER_PORT")
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
                   "[-cp <cache port>] [-?] [params]"
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
  echo "  -rp    Specify the routers port (defaulrt  179)"
  echo "  -cp    Specify the cache port (defaulrt  50000)"
  echo
  echo "  params A list of router specific parameters"
  echo
}

#{SEGMENT: Parameters}
PARAMS=()
while [ "$1" != "" ] ; do
  case "$1" in
    "start" | "stop")
      CMD=$1"Router"
      shift 
      if [ "$1" != "" ] ; then
        EXPERIMENT="$1"
      else
        echo "Experiment name is missing"
        exit 1
      fi
      ;;
    "viewTable")       
      CMD=$1
      shift 
      if [ "$1" != "" ] ; then
        EXPERIMENT="$1"
      else
        error "Experiment name is missing"
      fi
      ;;
    "-rp")
      shift
      if [ "$1" != "" ] ; then
        ROUTER_PORT=$1
      else
        error "Router Port expected"
      fi
      ;;
    "-cp")
      shift
      if [ "$1" != "" ] ; then
        CACHE_PORT=$1
      else
        error "RPKI Cache Port expected"
      fi
      ;;
    "-?") 
      syntax 
      exit 0
      ;;
    *) 
      if [ "$EXPERIMENT" == "" ] ; then
        EXPERIMENT="$1"
      else
        PARAMS+=( $1 )
      fi
      ;;
  esac
  shift
done

#{SEGMENT: Main}

if [ "$EXPERIMENT" == "" ] ; then
  echo "WARNING: No experiment specified!"
  EXPERIMENT="unknown"
fi

echo "Provided Router Port...: $ROUTER_PORT"
echo "Provided Cache Port....: $CACHE_PORT"
echo

_retVal=0
case "$CMD" in
  "startRouter" | "stopRouter" | "viewTable" )
    $CMD ${PARAMS[@]}
    _retVal=$?
    ;;
  *) 
    echo "Nothing to do - use -? for more information!"
    exit 1
    ;;     
esac
echo
