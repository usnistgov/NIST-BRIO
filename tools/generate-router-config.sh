#/!bin/bash

# This software was developed by employees of the National Institute of
# Standards and Technology (NIST), an agency of the Federal Government and is
# being made available as a public service. Pursuant to title 17 United States
# Code Section 105, works of NIST employees are not subject to copyright
# protection in the United States.  This software may be subject to foreign
# copyright.  Permission in the United States and in foreign countries, to the
# extent that NIST may hold copyright, to use, copy, modify, create derivative
# works, and distribute this software and its documentation without fee is hereby
# granted on a non-exclusive basis, provided that this notice and disclaimer of
# warranty appears in all copies.
# 
# THE SOFTWARE IS PROVIDED 'AS IS' WITHOUT ANY WARRANTY OF ANY KIND, EITHER
# EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY
# THAT THE SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND FREEDOM FROM
# INFRINGEMENT, AND ANY WARRANTY THAT THE DOCUMENTATION WILL CONFORM TO THE
# SOFTWARE, OR ANY WARRANTY THAT THE SOFTWARE WILL BE ERROR FREE.  IN NO EVENT
# SHALL NIST BE LIABLE FOR ANY DAMAGES, INCLUDING, BUT NOT LIMITED TO, DIRECT,
# INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES, ARISING OUT OF, RESULTING FROM,
# OR IN ANY WAY CONNECTED WITH THIS SOFTWARE, WHETHER OR NOT BASED UPON
# WARRANTY, CONTRACT, TORT, OR OTHERWISE, WHETHER OR NOT INJURY WAS SUSTAINED
# BY PERSONS OR PROPERTY OR OTHERWISE, AND WHETHER OR NOT LOSS WAS SUSTAINED
# FROM, OR AROSE OUT OF THE RESULTS OF, OR USE OF, THE SOFTWARE OR SERVICES
# PROVIDED HEREUNDER.
# 
# To see the latest statement, please visit:
# https://www.nist.gov/director/copyright-fair-use-and-licensing-statements-srd-data-and-software
# 

# Detect the OS name. This needs to be here because some OS's do not have
# bash installed by default. This needs to be done first.
if [ -e /etc/os-release ] ; then
  OS_NAME=$( grep -e "^ID=" /etc/os-release | awk -F'=' '{print $2}' | sed -e "s/\"//g" )
else
  OS_NAME=$( uname )
fi

case "$OS_NAME" in
  "rocky" | "ubuntu" ) 
    echo "Detected $OS_NAME!"
    ;;
  *)
    echo "The OS '$OS_NAME' is not supported (Yet)!"
    exit 1
    ;;
esac

# Get the directory name of ..
HOME=$(dirname $(realpath $0 | sed -E 's#^(.*)/[^/]+$#\1#g'))
if [ -e "$HOME" ] ; then cd $HOME &>/dev/null ; fi
FIND_CMD="find . "
SED_CMD="sed -i "

VERSION=b0.7.1.2-0016
PRG_NAME=$0
VERBOSE=0

# These are the Settings listed in the markdown scripts
KNOWN_TOKENS=(
  "AS_Number IP_Address"
  "Peer_AS Peer_IP"
  "Peer_Relation"
  "Peer_BGPsec"
  "Protocol"
  "Evaluation"
  "Mode"
  "RPKI_Cache_IP" 
  "RPKI_Cache_Port" 
  "RPKI_Cache_Protocol"
)

# Provide some help 
function syntax()
{
  echo
  echo "Syntax: $0 [(-l|-?|<type exp>)] [-v]"
  echo
  echo "  -l, --list  list all current knows parameters"
  echo "  -?          this screen"
  echo "  -v          additional output"
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
    "-v")
      VERBOSE=1
      shift
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

BRIO_PREFIX=$(find $HOME -type f -name 'brio_tg' | sed -nE 's#^(.*)/bin/brio_tg$#\1#p')
SRX_PREFIX=$(find $HOME -type f -name 'bgpd' | sed -nE 's#^(.*)/sbin/bgpd$#\1#p')

validator=0
router_qsrx=1

# SRx Server Configuration
PROGRAMS[$validator]=$($FIND_CMD | grep local | grep srx_server$ | head -n 1)
CONFIGS[$validator]=$BRIO_PREFIX/etc/srxcryptoapi.conf

# QuaggaSRx Configuration
PROGRAMS[$router_qsrx]=$($FIND_CMD | grep local | grep bgpd$ | head -n 1)
# The configuration filename is only used to generate a template. 
CONFIGS[$router_qsrx]=$BRIO_PREFIX/etc/bgpd.conf

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
  echo "  ROUTER.....: ${PROGRAMS[$router_qsrx]} -f ${CONFIGS[$router_qsrx]}"
  echo "  VALIDATOR..: ${PROGRAMS[$validator]} -f ${CONFIGS[$validator]}"
  echo
}

# This script builds the router configuration for QuaggaSRx, the NIST-BGP-SRx
# frameworks BGP router (Quagga Based).
function buildRouterCfgTemplate_QSRx()
{
  cat << EOF > ${CONFIGS[$router_qsrx]}.tpl
! -*- bgp -*-
!
! QuaggaSRx BGPd sample configuration file
!
! bgpd.conf,v 6.0
!
! Current available configuration tokens: 
! AS_Number,  IP_Address, Peer_AS - Peer_AS_x Peer_IP - Peer_IP_x, Mode, 
! Protocol, Evaluation, RPKI_Cache_IP, RPKI_Cache_Port, RPKI_Cache_Protocol
!
! For Protocol, RPKI_Cache_Protocol, and Mode see the router configuration files
! within the examples.
!
! Generated by BRIO {PRG_NAME} Version {VERSION} 
!
hostname bgpd
password zebra

router bgp {AS_Number}
  bgp router-id {IP_Address}

  srx display
  srx set-proxy-id {IP_Address}
  ! We assume QSRx and SRx-Server are running on the same system
  ! QuaggaSRx does not connect directly to RPKI Cache, it uses the srx-server
  ! srx set-server {RPKI_Cache_IP} {RPKI_Cache_Port}
  srx set-server {IP_Address} 17900
  srx connect
  no srx extcommunity

  srx keep-window 900
  ! [no] srx evaluation (origin|bgpsec|aspa)
  srx evaluation bgpsec
  srx evaluation {Validation}
  srx evaluation {Validation_2}
  srx evaluation {Validation_3}

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

  srx policy aspa local-preference valid        add      20
  srx policy aspa local-preference invalid      subtract 20
  srx policy aspa local-preference unknown      add      10
  srx policy aspa local-preference unverifiable subtract 5
  no srx policy aspa ignore undefined

! Specify Neighbors
! =================
  {Peer}! neighbor AS {Peer_AS}
  {Peer}neighbor {Peer_IP} remote-as {Peer_AS}
  {Peer}neighbor {Peer_IP} {Mode}
  {Peer}neighbor {Peer_IP} ebgp-multihop
  {Peer}neighbor {Peer_IP} bgpsec {BGPsec}
  {Peer}neighbor {Peer_IP} aspa {Peer_Relation}
  
  {Peer_2}! neighbor AS {Peer_AS_2}
  {Peer_2}neighbor {Peer_IP_2} remote-as {Peer_AS_2}
  {Peer_2}neighbor {Peer_IP_2} {Mode}
  {Peer_2}neighbor {Peer_IP_2} ebgp-multihop
  {Peer_2}neighbor {Peer_IP_2} bgpsec {BGPsec_2}
  {Peer_2}neighbor {Peer_IP_2} aspa {Peer_Relation_2}

  {Peer_3}! neighbor AS {Peer_AS_3}
  {Peer_3}neighbor {Peer_IP_3} remote-as {Peer_AS_3}
  {Peer_3}neighbor {Peer_IP_3} {Mode}
  {Peer_3}neighbor {Peer_IP_3} ebgp-multihop
  {Peer_3}neighbor {Peer_IP_3} bgpsec {BGPsec_3}
  {Peer_3}neighbor {Peer_IP_3} aspa {Peer_Relation_3}

  line vty
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
    _data=( ${_meta[1]}:${_meta[2]} VERSION:$VERSION PRG_NAME:$PRG_NAME )
    _data+=( 
        $(grep -E '^[[:space:]]*\|[^|]+\|[^|]+\|.*' $_file`` \
          | sed '/^[[:space:]:|-]*$/d' \
          | sed -E 's#^[[:space:]]*\|([^|]+)\|([^|]+)\|.*#\1:\2#g' \
          | sed -E "s#[[:space:]]*:[[:space:]]*#:#g" \
          | sed -E 's/^[[:space:]]+//; s/[[:space:]]+$//' | sed -e "s/ /_/g") 
    )
    match
    if [ $VERBOSE -eq 1 ] ; then
      echo "    * Process configuration data:" 
    fi
    _tokenArr=( )
    _line=""
    for _token in ${_data[@]} ; do
      _Peer=""
      if [ "$_token" == "Setting:Value" ] ; then
        continue;
      fi
      _key_value=( $(echo $_token | sed -e "s/:/ /g") )
      _key=${_key_value[0]}
      _value=${_key_value[1]}
      countInParameters $_key ${_tokenArr[@]}
      _counter=$?
      _tokenArr+=( $_key )
      if [ "$_key" == "Peer_AS" ] ; then
        # Activate the peer
        _Peer="Peer"
      fi
      if [ $_counter -gt 0 ] ; then
        _counter=$(($_counter+1))
        if [ "$_key" == "Peer_AS" ] ; then
          # Update the peer number
          _Peer+="_$_counter"
        fi
        _key+="_$_counter"
        if [ $VERBOSE -eq 1 ] ; then
          echo -n "      > Token: $_key$(tab ${#_key} '.' 22 ): $_value$(tab ${#_value} ' ' 20)($_token)"
        fi
      else
        if [ $VERBOSE -eq 1 ] ; then
          echo -n "      > Token: $_key$(tab ${#_key} '.' 22 ): $_value"
        fi
      fi
      if [ "$_Peer" != "" ] ; then
        # Add the peer activation before the key
        if [ "$_line" != "" ] ; then
          _line+=",$_Peer:"
        else
          _line="$_Peer:"
        fi
      fi

      if [ "$_line" != "" ] ; then
        _line+=",$_key:$_value"
      else
        _line="$_key:$_value"
      fi
      if [ $VERBOSE -eq 1 ] ; then
        echo
      fi
    done
    if [ $VERBOSE -eq 1 ] ; then
      echo "      Line: $_line"
      echo
    fi
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
    _experiment=$(echo ${_data[0]} | sed -e "s/:/-/g")
    _configFile=$( echo ${CONFIGS[$router_qsrx]}.tpl | sed -e "s/.conf.tpl/-$_experiment.conf/g")
    echo -n "  - Create configuration file $_configFile..."
    cp ${CONFIGS[$router_qsrx]}.tpl $_configFile &>/dev/null
    match $? 0 clean_exit 1 "ERROR: Configuration file could not be created!"

    # Skip firast item, it is already processed
    _idx=1
    echo -n "    * Update configuration file..."
    while [ $_idx -lt ${#_data[@]} ] ; do
      _key_value=( $(echo ${_data[$_idx]} | sed -e "s/:/ /g") )
      # Convert value to lower case
      _key=${_key_value[0]}
      _value=$(echo ${_key_value[1]} | tr '[:upper:]' '[:lower:]')
      _idx=$(($_idx+1))
      if [ "$_value" != "" ] ; then
        $SED_CMD "s#{$_key}#$_value#g" $_configFile
      fi
      # Use the regular expression in the if statement.
      echo "$_key" | grep -Eq '^Peer(_[1-9][0-9]*)?$' &>/dev/null
      if [ $? -eq 0 ] ; then
        $SED_CMD "s#{$_key}#$_value#g" $_configFile        
      fi
    done
    match 0 0

    echo -n "    * Cleanup un-used configurations..."
    # Each line with a {...} block will receive a '!'
#    $SED_CMD -E '/\{.*\}/ s/^/! /' $_configFile
    $SED_CMD -E '/\{.*\}/d' $_configFile
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

echo