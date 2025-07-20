#/!bin/bash

# Detect the OS name. This needs to be here because some OS's do not have
# bash installed by default. This needs to be done first.
if [ -e /etc/os-release ] ; then
  OS_NAME=$( grep -e "^ID=" /etc/os-release | awk -F'=' '{print $2}' | sed -e "s/\"//g" )
else
  OS_NAME=$( uname )
fi

case "$OS_NAME" in
  "rocky" | "ubuntu") 
    echo "Detected $OS_NAME!"
    ;;
  *)
    echo "The OS '$OS_NAME' is not supported (Yet)!"
    exit 1
    ;;
esac

HOME=$(pwd)
FIND_CMD="find . "
SED_CMD="sed -i "

VERSION=b0.7.1.2
PRG_NAME=$0

# These are the Settings listed in the markdown scripts
KNOWN_TOKENS=(
  "AS_Number IP_Address"
  "Peer_AS Peer_IP"
  "Mode"
  "Protocol"
  "RPKI_Cache_IP RPKI_Cache_Port RPKI_Cache_Protocol"
)

# Provide some help 
function syntax()
{
  echo
  echo "Syntax: $0 [(-l|-?|<type exp>)]"
  echo
  echo "  -l, --list  list all current knows parameters"
  echo "  -?          this screen"
  echo 
  echo "  type exp    if provided this is the filter for the configuration"
  echo "              files." 
  echo "              the values are encoded in the file names. the structure"
  echo "              demo-aspa-upsteam/exp2..... results in "
  echo "              the parameter demo-aspa-upsteam exp2"
  echo
  echo "Version $VERSION"
  echo
  exit 0
}

# Here onlty the first parameter can be help or list.
# All remaining parameters are passed to buildRepository
if [ "$1" != "" ] ; then
  case "$1" in
    "-?") syntax ;;
    "-l" | "--list") 
          echo "Known parameters in configuration templates:"
          echo "   ${KNOWN_TOKENS[@]}"
          echo
          echo "Duplicate settings e.g. Peer_AS will be changed into Peer_AS "
          echo "and Peer_AS_2, and so on." 
          echo 
          exit 0
          ;;
    *)
      ;;
  esac
fi

# Enable "1" or disabe "2" coloring
USE_COLOR=1
if [ $USE_COLOR -eq 1 ] ; then
  COL_RED="\033[0;31m"
  COL_GREEN="\033[0;32m"
  COL_OFF="\033[0m"
fi

OK="$COL_GREEN[OK]$COL_OFF"
FAILED="$COL_RED[FAILED]$COL_OFF"

BRIO_PREFIX=$HOME/local
SRX_PREFIX=$( $FIND_CMD | grep -E "local-[0-9\.]+$" )

router=0
validator=1
experimentation=2

# QuaggaSRx Configuration
PROGRAMS[$router]=$($FIND_CMD | grep local | grep bgpd$)
CONFIGS[$router]=$BRIO_PREFIX/etc/bgpd.conf

# SRx Server Configuration
PROGRAMS[$validator]=$($FIND_CMD | grep local | grep srx_server$)
CONFIGS[$validator]=$BRIO_PREFIX/etc/srxcryptoapi.conf

# Markdown description for router experiments 
EXPERIMENT_FILES=( $($FIND_CMD | grep local \
                      | grep -E "exp[0-9]+.router-conf.md" \
                      | sed -e "s#^\(.*\)\/\(demo[^\/]+\)\/\(.*\)#\1:\2:\3#g") )
# Contains the data of each experiment file. The tokens are comma separated
# and each token has name:value. The first token contains the experiment type 
# and name
EXPERIMENTS=( )

OK="[OK]"
FAILED="[FAILED]"
LOG=/dev/null
ELOG=$LOG

# Exit in a clean manner. No parameter needed, default exit level is 0
# This function does print an echo prior exit
# $1 exit level
# $2..n text being printed out 
function clean_exit()
{
  local _retVal=${1:-0}
  shift
  local _message="$( echo $@ )"

  if [ "$_message" != "" ] ; then
    echo -e "$_message"
  fi
  echo

  exit $retVal
}

# Print $OK or $FAILED and returns $1
function match()
{
  local _retVal=1
  if [ "$1" == "$2" ] ; then
    echo -e $OK
  else
    echo -e $FAILED
    shift; shift
    $@
  fi

  return $_retVal
}

##
## Count the number a specific value occurs in the 
## list of parameters.
##
## $1 The value to search for
 #
## $2..$n the parameters to look in
##
## Return How often the given value was found
##
function countInParameters()
{
  local _search=$1
  local _count=0
  shift
  while [ "$1" != "" ] ; do
    if [ "$1" == "$_search" ] ; then
      _count=$(($_count+1))
    fi
    shift
  done

  return $_count
}

##
## determines if the search value is in the given parameters.
##
## $1 The value to search for
 #
## $2..$n the parameters to look in
##
## Return 1 if the given value was found, 0 otherwise
##
function isInArray()
{
  local _found
  
  countInParameters $@
  _found=$?

  if [ $_found -gt 0 ] ; then
    _found=1
  fi

  return $_found
}

# Return a string of charachters (default blank) up to the next tab position.
# $1 the current position from where tabs should be calculated (default 0)
# $2 the tab charachter (default blank)
# $3 the length of each tab (default 8)
# Return a string of tabs to allign the text with the next tab position
function tab()
{
  # Fill $1 with 0 if not set
  local _pos=${1:-0}
  # Fill $2 with one blank if not set
  local _char="${2:-" "}"
  # Fill $3 with the default tab length of 8 if not set
  local _tab=${3:-8}
  local _paddingLen=$(( $_tab - ( $_pos % $_tab ) ))

  printf "%.0s$_char" $(seq 1 $_paddingLen)
}

# Show the configuration
function showConfig()
{
  echo
  echo "BGP-SRx Configuration Setting:"
  echo "======================================================================="
  echo "  ROUTER.....: ${PROGRAMS[$router]} -f ${CONFIGS[$router]}"
  echo "  VALIDATOR..: ${PROGRAMS[$validator]} -f ${CONFIGS[$validator]}"
  echo
}

function buildRouterCfgTemplate()
{
  cat << EOF > ${CONFIGS[$router]}.tpl
! -*- bgp -*-
!
! QuaggaSRx BGPd sample configuration file
!
! bgpd.conf,v 6.0
!
! Generated by BRIO {PRG_NAME} Version {VERSION} 
!
hostname bgpd
password zebra

router bgp {ASN_Number}
  bgp router-id {IP_Address}

  srx display
  srx set-proxy-id {IP_Address}

  srx set-server {RPKI_Cache_IP} {RPKI_Cache_Port}
  srx connect
  no srx extcommunity

  ! [no] srx evaluation (origin|bgpsec|aspa)
  {EVAL-ORIGIN}
  {EVAL-BGPSEC}
  {EVAL-ASPA}

  ! srx set-(origin|bgpsec|aspa)-value (undefined|valid|invalid|notfound)
  srx set-origin-value undefined
  srx set-bgpsec-value undefined
  srx set-aspa-value undefined

!
! Configure "Prefer Valid" using local preference
! Increase the local preference if route is valid.
!

  srx policy origin local-preference valid    add      20 
  srx policy origin local-preference notfound add      10 
  srx policy origin local-preference invalid  subtract 20 
  no srx policy origin ignore undefined

  srx policy bgpsec local-preference valid    add      20 
  srx policy bgpsec local-preference invalid  subtract 20 
  no srx policy bgpsec ignore undefined

  srx policy aspa local-preference valid    add      20 
  srx policy aspa local-preference notfound add      10 
  srx policy aspa local-preference invalid  subtract 20 
  no srx policy aspa ignore undefined

! Specify Neighbors
! =================
  neighbor AS {Peer_AS}
  neighbor {Peer_IP} remote-as {Peer_AS}
  neighbor {Peer_IP} ebgp-multihop
  neighbor {Peer_IP} {Mode}

  neighbor AS {Peer_AS_2}
  neighbor {Peer_IP_2} remote-as {Peer_AS_2}
  neighbor {Peer_IP_2} ebgp-multihop
  neighbor {Peer_IP_2} {Mode}

  neighbor AS {Peer_AS_3}
  neighbor {Peer_IP_3} remote-as {Peer_AS_3}
  neighbor {Peer_IP_3} ebgp-multihop
  neighbor {Peer_IP_3} {Mode}

log stdout

EOF
}

# Go through the EXPERIMENT_FILES array and either process all or just the one
# that produces a match. These data is encoded in the file name.
# This function fills the EXPERIMENTS array. First token contains the 
# experiment type:name followed by key:value pairs, seprated by comma.
# In case $1 an $2 are provided, only the data for this experiment will be
# used. Otherwise all will be processed.
#
# $1 the experiment type (demo-...)
# $2 the experiment number (exp123)
function buildRepository()
{
  local _data
  local _meta
  local _exp
  local _tokenArr
  local _key_value
  local _key 
  local _value
  local _counter

  local _exp_type=${1:-""}
  local _exp_num=${2:-""}
  local _matchOnly=0

  if [ "$1$2" != "" ] ; then
    _matchOnly=1
  fi

  for _file in ${EXPERIMENT_FILES[@]} ; do
    _meta=( $_file )
    _meta+=( $(echo $_file \
             | sed -E "s#(.*)/(demo[^/]+)/(exp[^.]+)(.*)#\2 \3#g") )
    if [ $_matchOnly -eq 1 ] ; then
      if [ "${_meta[1]}" != "$_exp_type" ] && [ "${_meta[2]}" != "$_exp_num" ] ; then
        echo "  - Skip $_file - No match!"
        # Move on
        continue
      fi
    fi

    echo "  - Process $_file"
    echo -n "    * Extract configuration data..." 
    _data=( ${_meta[1]}:${_meta[2]} VERSION:$VERSION PRG_NAME:$PRG_NAME)
    _data+=( 
        $(grep -E '^[[:space:]]*\|[^|]+\|[^|]+\|.*' $_file`` \
          | sed '/^[[:space:]:|-]*$/d' \
          | sed -E 's#^[[:space:]]*\|([^|]+)\|([^|]+)\|.*#\1:\2#g' \
          | sed -E "s#[[:space:]]*:[[:space:]]*#:#g" \
          | sed -E 's/^[[:space:]]+//; s/[[:space:]]+$//' | sed -e "s/ /_/g") 
    )
    match
    echo "    * Process configuration data:" 
    _tokenArr=( )
    _line=""
    for _token in ${_data[@]} ; do
      if [ "$_token" == "Setting:Value" ] ; then
        continue;
      fi
      echo -n "      > Token: $_token"
      _key_value=( $(echo $_token | sed -e "s/:/ /g") )
      _key=${_key_value[0]}
      _value=${_key_value[1]}
      countInParameters $_key ${_tokenArr[@]}
      _counter=$?
      _tokenArr+=( $_key )
      if [ $_counter -gt 0 ] ; then
        _counter=$(($_counter+1))
        _key+="_$_counter"
        echo -n "$(tab ${#_token} ' ' 20 )--> $_key:$_value"
      fi 
      if [ "$_line" != "" ] ; then
        _line+=",$_key:$_value"
      else
        _line="$_key:$_value"
      fi
      echo
    done
    EXPERIMENTS+=( $_line )
  done
}

# This function uses the generated bgpd.conf.tpl template and creates a function
# configuration file out of it.
function buildConfigurations
{
  local _data
  local _expType
  local _expNum
  local _element
  local _idx
  local _key_vale
  local _configFile

  for _token in ${EXPERIMENTS[@]} ; do
    _data=( $(echo $_token | sed -e "s/,/ /g") )
    _element=( $(echo ${_data[0]} | sed -e "s/:/ /g") )
    _expType=${_element[0]}
    _expNum=${_element[1]}
    _configFile=$( echo ${CONFIGS[$router]}.tpl | sed -e "s/.conf.tpl/$_expType-$_expNum.conf/g")
    echo -n "  - Create configuration file $_configFile..."
    cp ${CONFIGS[$router]}.tpl $_configFile &>/dev/null
    match $? 0 clean_exit 1 "ERROR: Configuration file could not be created!"

    # Skip firast item, it is already processed
    _idx=1
    echo -n "  - Update configuration file..."
    while [ $_idx -lt ${#_data[@]} ] ; do
      _key_value=( $(echo ${_data[$_idx]} | sed -e "s/:/ /g") )
      _value=${_key_value[1]}
      if [ "${_key_value[0]}" == "Mode" ] && [ "$_value" == "Passive" ]; then
        # Mode has unfortunately the setting Passive but QSRx uses passive
        _value="passive"
      fi
      _idx=$(($_idx+1))
      $SED_CMD "s#{${_key_value[0]}}#$_value#g" $_configFile
    done
    match 0 0

    echo -n "  - Cleanup un-used configurations..."
    $SED_CMD -E '/\{.*\}/ s/^/! /' $_configFile
    match $? 0
  done
}

showConfig
echo "Build cfg template:"
buildRouterCfgTemplate
echo "Build repository:"
buildRepository $@
echo "Build configurations:"
buildConfigurations

