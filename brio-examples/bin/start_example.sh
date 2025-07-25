#!/bin/bash
# Make sure we start in the demo folder
#
#
# For BRIO the following VARIABLES can be handed down to the call
#
# BRIO1_PARAMS=[Additional command line parameter]* 
# BRIO2_PARAMS=[Additional command line parameter]*
#
if [ "$_BRIO_CALLER" == "" ] ; then
  echo "This starter script cannot be called directly!"
  exit 1
fi

PACKAGE_NAME="NIST-BRIO"
VERSION="b0.7.1.2-0015"
DEBUG=0

if [ "$1" == "version" ] || [ "$1" == "-v" ] ; then
  echo "Version $VERSION"
  exit 
fi

echo "Start $_BRIO_CALLER $@..."

# DEMO_CURR_FLDR is used to come back to when script exits
DEMO_CURR_FLDR=$(pwd)
DEMO_FLDR=$(readlink -f $0 | xargs dirname)
DEMO_PREFIX_FLDR=$(echo $DEMO_FLDR | sed -e "s/\(.*\)\/opt\/.*/\1/g")
LOCAL_BIN_FLDR=$DEMO_PREFIX_FLDR/bin
LOCAL_SBIN_FLDR=$DEMO_PREFIX_FLDR/sbin
LOCAL_ETC_FLDR=$DEMO_PREFIX_FLDR/etc
DEMO_EXAMPLES_FLDR=$DEMO_PREFIX_FLDR/opt/brio-examples
DEMO_EX_BIN_FLDR=$DEMO_EXAMPLES_FLDR/bin
DEMO_EX_ETC_FLDR=$DEMO_EXAMPLES_FLDR/etc
DEMO_EX_LIB_FLDR=$DEMO_EXAMPLES_FLDR/lib

if [ $DEBUG -eq 1 ] ; then
  echo
  echo "DEMO_CURR_FLDR=$DEMO_CURR_FLDR"
  echo "DEMO_FLDR=$DEMO_FLDR"
  echo "DEMO_PREFIX_FLDR=$DEMO_PREFIX_FLDR"
  echo "LOCAL_BIN_FLDR=$LOCAL_BIN_FLDR"
  echo "LOCAL_ETC_FLDR=$LOCAL_ETC_FLDR"
  echo "DEMO_EXAMPLES_FLDR=$DEMO_EXAMPLES_FLDR"
  echo "DEMO_EX_BIN_FLDR=$DEMO_EX_BIN_FLDR"
  echo "DEMO_EX_ETC_FLDR=$DEMO_EX_ETC_FLDR"
  echo "DEMO_EX_LIB_FLDR=$DEMO_EX_LIB_FLDR"
  echo
fi

# Indicator if anything did run!
ANYTHING_SELECTED=0

##############################################################################
##  LOAD THE LIBRARY
##
if [ ! -e $DEMO_EX_LIB_FLDR/functions.sh ] ; then
  echo "WARNING: Could not find library script [$DEMO_EX_LIB_FLDR/functions.sh]!"
  echo "         Install framework properly prior usage."
else
  . $DEMO_EX_LIB_FLDR/functions.sh
fi
if [ "$FUNCTION_LIB_VER" == "" ] ; then
  echo "ERROR loading the functions library - Abort operation!"
  exit 1
fi

##############################################################################
##
##  Check for command line skips:
##

# Used in syntax to determine if router was skipped or not configured. It will
# only be set to '1' if ENABLE_ROUTER was original set to '1'
SKIP_ROUTER=0

# Parameters passed through to the router.
ROUTER_PARAMS=( )

_ALL_PARAMS=( )
# as long as enableAllParams is enable, all params are store in _ALL_PARAMS
_enableRtrParams=0
while [ "$1" != "" ] ; do
  case "$1" in 
    "--SKIP-ROUTER")
      if [ "$ENABLE_ROUTER" == "1" ] ; then
        echo "Detected disable router command!"
        echo
        echo "#################################################################"
        echo "IMPORTANT: The router must be up and running to perform the test!"
        echo "#################################################################"
        echo
        SKIP_ROUTER=1
        ENABLE_ROUTER=0
      fi
      ;;
    "--router-param")
      if [ "$2" == "" ] ; then
        echo "ERROR: --router-param requires <parameters>."
        endPrg 1
      fi
      # Fill the ramainder with into ROUTER_PARAMS
      _enableRtrParams=1
      ;;
    *)
      if [ $_enableRtrParams -eq 1 ] ; then
        ROUTER_PARAMS+=( $1 )
      else
        _ALL_PARAMS+=( $1 )
      fi
      ;;
  esac
  shift
done

if [ $DEBUG -eq 1 ] ; then
  echo _ALL_PARAMS=${_ALL_PARAMS[@]}
  echo ROUTER_PARAMS=${ROUTER_PARAMS[@]}
fi

# Going forward we use _ALL_PARAMS in lieu of $@ - _ALL_PARAMS will be replaced
# with _PARAMS at a later point.

##############################################################################
##
##  Check passed configuration and set default values if not set.
##
##
##  If not enabled, disable by default

if [ "$EXPERIMENT_NAME" == "" ] ; then 
  echo "The name of the experiment is needed for the router to find its"
  echo "proper router configuration!"
  exit 1
fi

if [ "$ENABLE_ROUTER"  == "" ] ; then ENABLE_ROUTER=0  ; fi
if [ "$ENABLE_BRIO2"   == "" ] ; then ENABLE_BRIO2=0   ; fi

## Set Router Information
if [ $ENABLE_ROUTER -eq 1 ] ; then
  if [ "$ROUTER_PRG"  == "" ] ; then ROUTER_PRG=rtr-wrapper.sh             ; fi
  if [ "$ROUTER_FLDR" == "" ] ; then ROUTER_FLDR=$DEMO_EX_BIN_FLDR ; fi
else
  echo "INFO: Router is disabled!"
fi

## Set Default Ports
if [ "$PORT_CACHE"     == "" ] ; then PORT_CACHE=50000 ; fi
if [ "$PORT_ROUTER"    == "" ] ; then PORT_ROUTER=179  ; fi

## Set default configurations
# CFG_ROUTER_NAME is optional so no need to process here
if [ "$CFG_ROUTER_NAME" == "" ] ; then CFG_ROUTER_NAME="rtr-wrapper.cfg" ; fi
if [ "$CFG_CACHE_NAME"  == "" ] ; then CFG_CACHE_NAME="cache.script"     ; fi
if [ "$CFG_BRIO1_NAME"  == "" ] ; then CFG_BRIO1_NAME="brio1.conf"       ; fi
if [ "$CFG_BRIO2_NAME"  == "" ] ; then CFG_BRIO2_NAME="brio2.conf"       ; fi

## Set default runtime parameters
# Perform the stop all until $STOP_ALL_LOOP -eq 0. This allows to tweak the 
# stopping of all modiles, recommended is 3
if [ "$STOP_ALL_LOOP"     == "" ] ; then STOP_ALL_LOOP=3      ; fi

if [ "$LISTEN_TIMEOUT"    == "" ] ; then LISTEN_TIMEOUT=10    ; fi
if [ "$SIT_AND_WAIT_TIME" == "" ] ; then SIT_AND_WAIT_TIME=10  ; fi
if [ "$SUDO_MODULES"      == "" ] ; then SUDO_MODULES=( )     ; fi
if [ "$SIT_AND_WAIT_MOD"  == "" ] ; then SIT_AND_WAIT_MOD=( ) ; fi

##############################################################################
##
## Auto configuration
##
if [ $ENABLE_ROUTER -eq 1 ] ; then
  countInParameters "router" ${SUDO_MODULES[@]}
  if [ $? -eq 0 ] ; then
    SUDO_MODULES+=("router")
  fi
  countInParameters "router" ${SIT_AND_WAIT_MOD[@]}
  if [ $? -eq 0 ] ; then
    SIT_AND_WAIT_MOD+=("router")
  fi
fi

##############################################################################
##
## Set non configurable default variables
##

# Check if the port is in priviledged range and add to SUDO_MODULES if needed.
# $1 Module name
# $2 Port number
function checkSUDO()
{
  local module=$1
  local port=$2

  if [ "$module" != "" ] && [ "$port" != "" ] ; then
    if [ $port -gt 0 ] && [ $port -lt 1025 ] ; then
      isInArray $module ${SUDO_MODULES[@]}
      if [ $? -eq 0 ] ; then
        echo "Module: $module uses port $port -> move to sudo"
        SUDO_MODULES+=( $module )
      fi
    fi
  fi
}

PRG_BRIO=brio_tg
BRIO_NAME="BRIO - Traffic Generator"
PRG_CACHE=brio_rc
CACHE_NAME="BRIO - RPKI Router Cache Test Harness"
ROUTER_NAME="Router Under Test"

# Enabled by default
ENABLE_CACHE=1
ENBALE_BRIO1=1

# Will be set with -d
DRYRUN=""

# Will be set to "" if --global-binary us used.
PRG_SFX="./"

# List of tools required for the "all" mode
REQ_TOOLS=("/bin/bash" "screen" "netstat" "awk")

# Indicates if gnome-terminal is available & should be used in lieu of screen
TOOL_TERMINAL=0
USE_TERMINAL=0
which gnome-terminal &> /dev/null
if [  $? -eq 0 ] ; then
  TOOL_TERMINAL=1
fi

# Contains the parameters that can be globally used for each module
GLOB_PARAMS=( "-t" "--wait-for-enter" "--global-binary" )

# These arrays are used for the automated starting and stopping.
# modules are started and stopped in this order
ALL_MOD_NAME=( )     # Contains the module name
ALL_MOD_PORT=( )     # Contains the port, this module uses - 0 no port
ALL_MOD_PORT_REQ=( ) # Contains the port this module wants to connect to 
if [ $ENABLE_CACHE  -eq 1 ] ; then 
  ALL_MOD_NAME+=( "cache" ); 
  ALL_MOD_PORT+=( $PORT_CACHE ); 
  ALL_MOD_PORT_REQ=( 0 )
  checkSUDO "cache" $PORT_CACHE
fi
if [ $ENABLE_ROUTER -eq 1 ] ; then 
  ALL_MOD_NAME+=( "router" )
  ALL_MOD_PORT+=( $PORT_ROUTER )
  ALL_MOD_PORT_REQ+=( $PORT_CACHE ) # Wants the cache to be online
  checkSUDO "router" $PORT_ROUTER
fi
if [ $ENBALE_BRIO1 -eq 1 ] ; then 
  ALL_MOD_NAME+=( "brio1"  ) 
  ALL_MOD_PORT+=( 0 )
  if [ $ENABLE_ROUTER -eq 1 ] ; then
    ALL_MOD_PORT_REQ+=( $PORT_ROUTER ) # Wants the router to be reachable
  else
    ALL_MOD_PORT_REQ+=( 0 ) # The router might be running elsewhere
  fi
fi 
if [ $ENABLE_BRIO2 -eq 1 ] ; then 
  ALL_MOD_NAME+=( "brio2"  )
  ALL_MOD_PORT+=( 0 )
  if [ $ENABLE_ROUTER -eq 1 ] ; then
    ALL_MOD_PORT_REQ+=( $PORT_ROUTER ) # Wants the router to be reachable
  else
    ALL_MOD_PORT_REQ+=( 0 ) # The router might be running elsewhere
  fi
fi

if [ $DEBUG -eq 1 ] ; then
  echo "Service:_ ${ALL_MOD_NAME[@]}" | sed -e "s/ /\t/g" | sed -e "s/_/ /g"
  echo "Own_Port: ${ALL_MOD_PORT[@]}" | sed -e "s/ /\t/g" | sed -e "s/_/ /g"
  echo "Wait_For: ${ALL_MOD_PORT_REQ[@]}" | sed -e "s/ /\t/g" | sed -e "s/_/ /g"
fi

CFG_CACHE=$DEMO_FLDR/$CFG_CACHE_NAME
if [ $ENABLE_ROUTER -eq 1 ] && [ "$CFG_ROUTER_NAME" != "" ] ; then
  echo "Scan for BGP router configuration:"
  CFG_ROUTER=$(pwd)/$CFG_ROUTER_NAME
  echo -n "  - Check for: $CFG_ROUTER_NAME..." 
  if [ -e $CFG_ROUTER_NAME ] ; then
    echo "found"
  else
    echo "not found!"
    CFG_ROUTER=$DEMO_EXAMPLES_FLDR/etc/$CFG_ROUTER_NAME
    echo -n "  - Check for: $CFG_ROUTER..."     
    if [ -e $CFG_ROUTER ] ; then
      echo "found!"
    else
      echo "not found!"
      CFG_ROUTER=$LOCAL_ETC_FLDR/$CFG_ROUTER_NAME
      echo "  - Set to: $CFG_ROUTER!"     
    fi
  fi
else
  CFG_ROUTER=
fi
CFG_BRIO1=$DEMO_FLDR/$CFG_BRIO1_NAME
CFG_BRIO2=$DEMO_FLDR/$CFG_BRIO2_NAME

#
# Display the programs syntax
#
function syntax()
{
  echo "$0 <module> [-t] [--wait-for-enter] [--gloal-binary] [-d] | <command> [-d] [--router-param <parameters>]"
  echo
  echo " --wait-for-enter Wait for the enter key to be pressed"
  echo "                  before the program ends." 
  echo " --global-binary  Indicates if the binaries are in the PATH."
  echo " -t               Use the gnome-terminal in lieu of screen"
  echo "                  when using <all> or <select>"
  if [ $TOOL_TERMINAL -eq 0 ] ; then
    echo "                  IMPORTANT: gnome-terminal required to work!!"
  fi
  echo " -d               Perform a dry run only!"
  if [ $ENABLE_ROUTER -eq 1 ] ; then
    echo " --SKIP-ROUTER    Overwrite configuration and disable router!"
    echo " --router-param <parameters>"
    echo "                Uses all remaining parameters ass pass trhough to the"
    echo "                BGProuter." 
  fi
  echo
  echo "Command:"
  if [ $ENABLE_ROUTER -eq 1 ] ; then
    echo "  view-table:  Display the content of the BGP router's RIB-IN"
  fi
  echo "  view-all     Display all 'screens' available"
  echo "  stop-all     Stop all BGP-SRx 'screens'"
  echo "  version | -v Display the version number and exit" 
  echo 
  echo "Module:"
  echo "  all"
  echo "           Start all modules (root privileges required!)"
  echo "           This uses screen and starts all but the router in the"
  echo "           users environment and router as root."
  echo "  select <module> [ <module>]*"
  echo "           Functions in the same way as all except it does not use"
  echo "           all modules in the predefined order, it starts modules"
  echo "           in the order as scripted and only the scripted ones."
  echo 
  echo "  cache    Start the $CACHE_NAME"
  if [ $ENABLE_ROUTER -eq 1 ] ; then
    echo "  router   Start the $ROUTER_NAME (root privileges required!)"
  fi
  echo "  brio1    Start the $BRIO_NAME traffic generator 1"
  if [ $ENABLE_BRIO2 -eq 1 ] ; then
    echo "  brio2    Start the $BRIO_NAME traffic generator 2"
  fi
  if [ $ENABLE_ROUTER -eq 0 ] ; then
    echo
    echo "Note: The router is not controlled by this starter script! To enable"
    if [ $SKIP_ROUTER -eq 0 ] ; then
      echo "      the router, call this script with ENABLE_ROUTER=1 being set!"
    else
      echo "      the router, call this script without --SKIP-ROUTER!"
    fi
  fi
  echo 
  echo "$PACKAGE_NAME $0 V $VERSION"
  echo
  endPrg $1
}

#
# Make sure to return to the folder this script was called from
# Similar to popd.
#
# @param $1 the exit code/
# 
function endPrg()
{
  cd $DEMO_CURR_FLDR
  echo
  exit $1
}

#
# Start the program specified in the parameters
#
# $1 Indicates if it needs sudp privileges (1=sudo 2=no)
# $1 The program to be started
# $2..n The programs parameter.
#
# return 0 if all went fine
#
function startPrg()
{
  local _retVal=1
  local _sudo=""
  local _sudo_text=""
  local _program="$2"
  
  case "$1" in
    "0")
        shift
        ;;
    "1") 
        if [ "$(whoami)" == "root" ] ; then
          _sudo="sudo "
          _sudo_text=" as 'root'"
        fi
        shift
        ;;
    *)
        echo "Error: Invalid value '$1' for startPrg"
        return 1
        ;;
  esac
  shift

  if [ "$_program" != "" ] ; then
    echo "Starting [ $_sudo$_program $@ ]$_sudo_text..."    
    if [ -e $_program ] ; then
      $DRYRUN $_sudo$_program $@
      _retVal=$?

      if [ ! $_retVal -eq 0 ] ; then 
        echo "ERROR '$_retVal' returned!"
        _retVal=$_retVal
      fi
    else
      echo "ERROR: Cannot find '$_program'"
      echo
      echo "Verify that $(pwd)/$_program is properly installed!"
      echo "Abort operation"
      _retVal=1
    fi
  fi

  return $_retVal
}

#
# Start the given  module in a screen session.
# Return 0 if it was successfull, otherwse 1
#
# $1 "screen" or "terminal"
# $2 The module to be started
# $3 A tabulator for formating (optional)
#
function _startModuleIn()
{
  local _retVal=0
  local _thread=$1
  local _module=$2
  local _sudo_cmd=""
  local _tab="$3"
  local _counter=0
  local _activate=""

  shift
  if [ "$_module" != "" ] ; then
    countInParameters $_module ${SUDO_MODULES[@]}

    if [ $? -gt 0 ] ; then
      if [ "$root" != "root" ] ; then
        _sudo_cmd="sudo"
        echo "$_tab""Module [$_module] requires sudo priviledges!"
        # Allow to enter for terminal
        _activate="--active"
      fi
    fi

    # Check if this module is already active as a screen
    case "$_thread" in
      "screen")
         $DRYRUN $_sudo_cmd screen -ls | grep "\.$_module " > /dev/null 2>&1
         _retVal=$?
         ;;
      "terminal")
         _retVal=1
         ;;
      *)
         _retVal=1
         ;; 
    esac
   
    if [ $_retVal -eq 0 ] ; then
      echo "$_tab""A $_thread module with this name is already active!"
    else
      echo "$_tab- Starting module '$_module'..."
      case "$_thread" in
        "screen")
          $DRYRUN $_sudo_cmd screen -S "$_module" -d -m $0 $_module --wait-for-enter
          _retVal=$?
          ;;
        "terminal")
          $DRYRUN gnome-terminal --title "$_module" --tab $_activate -- /bin/bash -c "$_sudo_cmd $0 $_module --wait-for-enter"
          _retVal=$?
          ;;
        *)
          _retVal=1
          ;;
      esac

      if [ $_retVal -eq 0 ] ; then
        echo "$_tab  Start successful!"
      else
        echo "$_tab  Start failed!"
      fi
      countInParameters $_module ${SIT_AND_WAIT_MOD[@]}
    fi
  fi
  return $_retVal
}

#
# This function does start all components needed
#
# $1..$n The modules to be started
#
function runAutomated()
{
  local _retVal=0
  local _loopCounter=20
  local _MODULES=$@
  local _useTimeout=$LISTEN_TIMEOUT
  local _waitCounter=$SIT_AND_WAIT_TIME

  echo "Perform tool check..."
  for tool in ${REQ_TOOLS[@]} ; do
    echo -n "Check for '$tool'..."
    which $tool &> /dev/null
    if [ $? -eq 0 ] ; then
      echo "Found!"
    else
      echo "Missing!"
      return 1
    fi
  done

  echo "Verify if any requested module is still running..."
  for _checkModule in ${_MODULES[@]} ; do
    # First check the screen tool
    if [ -e /tmp/$_checkModule.pid ] ; then
      pid=$(cat $_checkModule | sed -e "s/ //g")
      if [ "$pid" != "" ] ; then
        ps aux | awk '{ print $2 }' | grep "^$pid$"
        if [ $? -eq 0 ] ; then
          _retVal=1
          echo "Found other instances of '$_checkModule'"
        fi 
      fi
    fi
    if [ $_retVal -eq 1 ] ; then
      echo "Please stop all running instances!"
    fi
  done

  if [ $_retVal -eq 0 ] ; then
    # cache can be started right away
    # srx should wait 2 seconds to allow caceh to fully start
    # router should wait for 5 seconds to allow srx server to accept connections
    # brio1 should wait 5 seconds for router to be able to accept connections
    # brio2 can start right after brio1

    echo "Starting modules: ${_MODULES[@]}"
    # Check if I need to add some sit and wait modules
    countInParameters "router" ${_MODULES[@]}
    if [ $? -gt 0 ] ; then
      # Just in case, add brio1 and brio2 into the sit and wait
      SIT_AND_WAIT_MOD+=( "brio1" )
      if [ $ENABLE_BRIO2 -eq 0 ] ; then
        SIT_AND_WAIT_MOD+=( "brio2" )
      fi
    fi
    for _mod_idx in ${!ALL_MOD_NAME[@]} ; do
      # Check if this module is part of the requested modules.
      countInParameters ${ALL_MOD_NAME[$_mod_idx]} ${_MODULES[@]}
      if [ $? -gt 0 ] ; then
        if [ $_retVal -eq 0 ] ; then
          echo "  * Start module '${ALL_MOD_NAME[$_mod_idx]}'"
          ## 1st Check if the required port is available to accept connections
          if [ ${ALL_MOD_PORT_REQ[$_mod_idx]} -gt 0 ] ; then
            echo -n "    - Wait until port ${ALL_MOD_PORT_REQ[$_mod_idx]} is ready"
            _waitUntilLISTEN ${ALL_MOD_PORT_REQ[$_mod_idx]} $LISTEN_TIMEOUT
            _retVal=$?
            if [ "$DRYRAN" != "" ] ; then
              _retVal=0
            fi
            if [ $_retVal -eq 0 ] ; then
              echo "...done!"
            else
              echo "...time out!"
            fi
          fi
          # 2nd sit and wait until time has passed in case module is in sit and wait array
          countInParameters ${ALL_MOD_NAME[$_mod_idx]} ${SIT_AND_WAIT_MOD[@]}
          if [ $? -gt 0 ] ; then
            echo -n "    - Delay start by $SIT_AND_WAIT_TIME seconds"
            _waitCounter=$SIT_AND_WAIT_TIME
            if [ $_waitCounter -eq 0 ] ; then
              # No dots are printed so add a blank
              echo -n " "
            fi
            while [ $_waitCounter -gt 0 ] ; do
              _waitCounter=$(($_waitCounter-1))
              echo -n "."
              sleep 1
            done
            echo "done."
          fi

          # 3rd Start the module
          if [ $_retVal -eq 0 ] ; then
            if [ $USE_TERMINAL -eq 0 ] ; then
              _startModuleIn screen ${ALL_MOD_NAME[$_mod_idx]} "    "
              _retVal=$?
            else
              _startModuleIn terminal ${ALL_MOD_NAME[$_mod_idx]} "    "
              _retVal=$?
            fi
          fi
          # 4th Verify that the current module is listening on the required port.
          if [ $_retVal -eq 0 ] ; then
            if [ ${ALL_MOD_PORT[$_mod_idx]} -gt 0 ] ; then
              echo    "    - Timeout: $_useTimeout sec."
              echo -n "    - Wait until ${ALL_MOD_NAME[$_mod_idx]} listens on port ${ALL_MOD_PORT[$_mod_idx]}"
              _waitUntilLISTEN ${ALL_MOD_PORT[$_mod_idx]} $_useTimeout
              _retVal=$?
              if [ "$DRYRUN" != "" ] ; then
                _retVal=0
              fi
              if [ $_retVal -eq 0 ] ; then
                echo "...done!"
              else
                echo "...time out!"
              fi
            fi
          fi
          if [ $_retVal -ne 0 ] ; then
            echo "      Module ${ALL_MOD_NAME[$_mod_idx]} did not start properly!"
          fi          
        fi
        _useTimeout=$LISTEN_TIMEOUT
      else
        # Skip this module so reduce the timeout for port listening to 0
        _useTimeout=0
      fi
    done
  fi

  viewAll
  return $_retVal;
}

#
# This function displays all screen sessions found
#
function viewAll()
{
  local user=$(whoami)
  local displayText=0

  echo "Screen for ($user):"
  screen -ls | grep "(Detached)"
  if [ $? -eq 0 ] ; then
    displayText=1
  else
    echo "    Nothing running!"
  fi

  if [ "$user" != "root" ] ; then
    echo "Screen for (root):"
    sudo screen -ls | grep "(Detached)"
    if [ $? -eq 0 ] ; then
      displayText=1
    else
      echo "    Nothing running!"
    fi
  fi

  if [ $USE_TERMINAL -eq 0 ] && [ $displayText -eq 1 ] ; then
    echo
    echo "Use 'screen -r -S <screen-name>' to reattach to any screen."
    echo "Once reattached, press 'Ctrl-a d' to detach again."
    echo "For more information on how to use screen, use 'man screen'."
  fi
}


#
# This function checks is the experiment is started using screen
# and stops all instances it can find
#
function stopAll()
{
  if [ "$1" != "" ] ; then
    echo $1
  fi
  local _sudo_cmd
  local _user=$(whoami)
  local _mod_user=""
  local _program=()
  local _endLoop=10

  for _mod_idx in ${!ALL_MOD_NAME[@]} ; do
     _running_mod=${ALL_MOD_NAME[$_mod_idx]}
     _running_port=${ALL_MOD_PORT[$_mod_idx]}

    _program=( $(ps aux | grep -E "start_exp[0-9]+.sh" | grep -e "$_running_mod" | head -n 1 | awk '{ print $1 " " $2 }' ) )
    _mod_user="${_program[0]}"
    _mod_pid="${_program[1]}"
    if [ "$_mod_pid" == "" ] ; then
      # Now try to find the port number
      echo "  * Module [$_running_mod]: No instance found!"
      if [ $_running_port -gt 0 ] ; then
        # try to find the progam using the port number
        countInParameters "$_running_mod" ${SUDO_MODULES[@]}
        if [ $? -eq 0 ] ; then
          _sudo_cmd=""
          _user_mod="$_user"
        else
          _sudo_cmd="sudo "
          _user_mod="root"
        fi
        echo "    - Try to identify a running instance using the port '$_running_port'..."
        _mod_pid=$($_sudo_cmd netstat -tulpn 2>/dev/null | grep -e "[^0-9]$_running_port[^0-9]" | sed -e "s/.*LISTEN[^0-9]*\([0-9]\+\)[^0-9].*/\1/g" | sort -u)
        if [ "$_mod_pid" == "" ] ; then
          echo "      * Not Found!"
        else
          echo "      * Found!"
        fi
      fi
    fi
    ## Now try again, maybe the running mod needed root priv to get
    ## the proper pid.
    if [ "$_mod_pid" != "" ] ; then
      # Help prevent an endless loop
      _endLoop=10
      while [ "$_mod_pid" != "" ] && [ $_endLoop -gt 0 ] ; do
        _endLoop=$(($_endLoop-1))
        if [ "$_mod_user" != "$_user" ] ; then
          _sudo_cmd="sudo "
        else
          _sudo_cmd=""
        fi  
        readYN "  - Stopping module '$_running_mod' [$_mod_pid] ?"
        if [ $? -eq 1 ] ; then
          $_sudo_cmd kill -9 $_mod_pid > /dev/null
          $_sudo_cmd screen -wipe > /dev/null
        else
          echo "    * Abort stopping module '$_running_mod'!"
          _endLoop=0
        fi
        _program=( $(ps aux | grep -e start.sh | grep -e "$_running_mod" | head -n 1 | awk '{ print $1 " " $2 }' ) )
        _user="${_program[0]}"
        _mod_pid="${_program[1]}"
      done
    fi
  done
}

# Switch into the demo folder
cd $DEMO_FLDR

# Check that all files are configured but also allow "-" to not consider a 
# configure file.
TMP_CFG_FILES=( "$CFG_CACHE" "$CFG_ROUTER" "$CFG_BRIO1" )
CFG_FILES=( )
if [ $ENABLE_BRIO2 -eq 0 ] ; then
  TMP_CFG_FILES+=( "$CFG_BRIO2" )
fi
for tmp_cfg_file in ${TMP_CFG_FILES[@]} ; do
  echo $tmp_cfg_file | grep '/-$' > /dev/null 2>&1
  if [ ! $? -eq 0 ] ; then
    CFG_FILES+=( $tmp_cfg_file )
  fi
done

for cfg_file in "${CFG_FILES[@]}" ; do 
  if [ "$cfg_file" != " " ] && [ ! -e "$cfg_file" ] ; then
    echo "Configuration file: '$cfg_file' not found!"
    echo "Make sure the project is configured properly!"
    endPrg 1
  fi
done

retVal=0
READ_ENTER=0
_PARAMS=( )
# process all parameters and find configuration settings
for param in ${_ALL_PARAMS[@]} ; do
  case $param in 
    "?" | "-?" | "h" | "H" | "-h" | -"H")
      syntax 0
      ;;
    "-d")
      LISTEN_TIMEOUT=0
      SIT_AND_WAIT_TIME=0
      DRYRUN="echo - CALL: "
      echo
      echo "###################################################################"
      echo "## INFO: Enable dry-run!                                         ##"
      echo "###################################################################"
      echo
      ;;
    "--wait-for-enter") 
      READ_ENTER=1 
      ;;
    "--global-binary")
      PRG_SFX=""
      ;;
    "-t") 
      if [ $TOOL_TERMINAL -eq 1 ] ; then
        echo "Enable usage of terminal, do not use screen!"
        USE_TERMINAL=1
      else
        echo "WARNING: gnome-terminal is not installed."
        endPrg 1
      fi
      ;;
    *)
      # Add as parameter for main process
      _PARAMS+=( $param )
      ;;
  esac
done

# Now we use _PARAMS, not $_ALL_PARAMS
if [ ${#_PARAMS[@]} -eq 0 ] ; then
  echo "Nothing to do, try '$0 -?' for more information!"
  exit 1
fi 

_modules=()
_paramIdx=0

if [ $retVal -eq 0 ] ; then
  case ${_PARAMS[$_paramIdx]} in
    #{SEGMENT: view-table}
    "view-table")
      ANYTHING_SELECTED=1
      if [ $ENABLE_ROUTER -eq 1 ] ; then
        echo "Display the routing table:"
        echo "=========================="
        $DRYRUN $ROUTER_FLDR/$ROUTER_PRG viewTable $EXPERIMENT_NAME
      else
        echo "Router is not enabled for this experiment!"
      fi
      ;;
    #{SEGMENT: view-all}
    "view-all")
      ANYTHING_SELECTED=1
      viewAll 
      retVal=$?
      ;;
    #{SEGMENT: stop-all}
    "stop-all")
      ANYTHING_SELECTED=1
      STOP_ALL_LABEL="Stopping all screen modules..."
      if [ $STOP_ALL_LOOP -eq 0 ] ; then
        STOP_ALL_LOOP=1
      fi
      while [ $STOP_ALL_LOOP -gt 0 ] ; do
        stopAll $STOP_ALL_LABEL
        STOP_ALL_LABEL=
        STOP_ALL_LOOP=$(($STOP_ALL_LOOP-1))
        if [ $STOP_ALL_LOOP -gt 0 ] ; then
          sleep 1
        fi
      done
      screen -wipe >> /dev/null 2>&1
      viewAll
      retVal=$?
      READ_ENTER=0
      ;;
    #{SEGMENT: select}
    "select")
      ANYTHING_SELECTED=1
      _paramIdx=$(($_paramIdx+1))
      while [ $_paramIdx -lt ${#_PARAMS[@]} ] ; do
        countInParameters ${_PARAMS[$_paramIdx]} ${ALL_MOD_NAME[@]}
        if [ $? -gt 0 ] ; then
          _modules+=( ${_PARAMS[$_paramIdx]} )
        else  
          countInParameters ${_PARAMS[$_paramIdx]} ${GLOB_PARAMS[@]}
          if [ $? -eq 0 ] ; then
            echo "Invalid module '${_PARAMS[$_paramIdx]}'!"
            endPrg 1
          fi
        fi
        _paramIdx=$(($_paramIdx+1))
      done
      runAutomated ${_modules[@]}
      _retVal=$?
      echo
      if [ ! $_retVal -eq 0 ] ; then
        echo "An error occured during starting one of the modules."
      fi

      if [ $ENABLE_ROUTER -eq 1 ] ; then
        echo "Call $0 view-table to see the routers RIB-IN."
      fi
      READ_ENTER=0
      ;;
    #{SEGMENT: all}
    "all")
      ANYTHING_SELECTED=1
      runAutomated ${ALL_MOD_NAME[@]}
      _retVal=$?
      echo
      if [ ! $_retVal -eq 0 ] ; then
        echo "An error occured during starting one of the modules."
      fi
      echo "Call $0 view-table to see the routers RIB-IN."
      READ_ENTER=0
      ;;
    #{SEGMENT: cache}
    "cache")
      ANYTHING_SELECTED=1
      cd $LOCAL_BIN_FLDR
      isInArray "cache" ${SUDO_MODULES[@]}
      startPrg $? "$PRG_SFX$PRG_CACHE" "-f" "$CFG_CACHE" $PORT_CACHE
      retVal=$?
      ;;
    #{SEGMENT: router}
    "router")
      if [ $ENABLE_ROUTER -eq 1 ] ; then
        ANYTHING_SELECTED=1
        # build additional parameters
        _localParam=( )
        # Overwrite parameters that migt come from the custom experiment starter
        # and also set the configuration file if one is found
        if [ "$PORT_ROUTER" != "" ] ; then _localParam+=( "-rp"  "$PORT_ROUTER" ); fi
        if [ "$PORT_CACHE"  != "" ] ; then _localParam+=( "-cp"  "$PORT_CACHE"  ); fi
        if [ "$CFG_ROUTER"  != "" ] ; then _localParam+=( "-cfg" "$CFG_ROUTER"  ); fi
        cd $ROUTER_FLDR
        isInArray "router" ${SUDO_MODULES[@]}
        startPrg $? "$PRG_SFX$ROUTER_PRG" "start" "$EXPERIMENT_NAME" \
                    "${_localParam[@]}" "${ROUTER_PARAMS[@]}"
        retVal=$?
      else
        if [ $SKIP_ROUTER -eq 0 ] ; then
          echo "WARNING: Module 'brio2' is not supported in this experiment!"
        else
          echo "WARNING: Router is manually disabled '--SKIP-ROUTER'!"
        fi
        retVal=1
      fi      
      ;;
    #{SEGMENT: brio1}
    "brio1")
      ANYTHING_SELECTED=1
      cd $LOCAL_BIN_FLDR
      isInArray "brio1" ${SUDO_MODULES[@]}
      startPrg $? "$PRG_SFX$PRG_BRIO" "-f" "$CFG_BRIO1" "$BRIO1_PARAMS"
      retVal=$?
      ;;
    #{SEGMENT: brio2}
    "brio2")
      if [ $ENABLE_BRIO2 -eq 1 ] ; then
        ANYTHING_SELECTED=1
        cd $LOCAL_BIN_FLDR
        isInArray "brio2" ${SUDO_MODULES[@]}
        startPrg $? "$PRG_SFX$PRG_BRIO" "-f" "$CFG_BRIO2" "$BRIO2_PARAMS"
        retVal=$?
      else
        echo "WARNING: Module 'brio2' is not supported in this experiment!"
        retVal=1
      fi
      ;;
    *)
      echo "Unknown Module '${_PARAMS[$_paramIdx]}'"
      retVal=1
      READ_ENTER=0
      ;;
  esac
fi

if [ $READ_ENTER -eq 1 ] ; then
  read -p "Press Enter "
fi

if [ $ANYTHING_SELECTED -eq 0 ] ; then
  echo "INFO: Nothing was selected to run!"
fi
endPrg $retVal
