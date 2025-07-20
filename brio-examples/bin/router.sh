#!/bin/bash

CONFIGURED=0

if [ $CONFIGURED -eq 0 ] ; then
  echo "This is a demo file for the router to be started. It needs to be"
  echo "configured/rewritten for the used router."
  echo "Each function will have the experiment name as 1st parameter"
  echo
fi

# Will be overwritten using -e <experiment>
EXPERIMENT="unknown"
# The command, what to do
CMD=

# This function prints out all given parametes and ends with exit 2
function error()
{
  echo "ERROR: $@"
  exit 1
}

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
  echo "### Start netcat to listen on port 179 - blocking call         ##"
  echo "#################################################################"
  nc -l 179

  return $?
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

  # Most likely the router will be stopped using the OS kill -9 command!
  echo "## Kill the netcat call from earlier!                          ##"
  echo "#################################################################"
  PID=$(pidof -f "nc -1 179")
  kill -9 $PID

  return $?
}

# Just show the syntax, does not exit!
function syntax()
{
  echo "Syntax: $0 (start|stop|viewTable) <experiment> -rp <router port> " \
                   "-cp <cache port> [-?] [params]"
  echo
}

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
        ROTUER_PORT=$1
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
      PARAMS+=( $1 )
      ;;
  esac
  shift
done

_retVal=0
case "$CMD" in
  "startRouter" | "stopRouter" | "viewTable" )
    $CMD $PARAMS
    _retVal=$?
    ;;
  *) 
    echo "Nothing to do - use -? for more information!"
    exit 1
    ;;     
esac
echo
