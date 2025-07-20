#!/bin/bash
# The following parameters are defined in the main start program
# but can be overwritten here. For more detail on that they do
# please see the file ../bin/start_example.sh

# The setinga commented below are the default setting
# PORT_CACHE=50000
# PORT_ROUTER=179
# STOP_ALL_LOOP=3
# LISTEN_TIMEOUT=10
# SUDO_MODULES+=("router")
# SIT_AND_WAIT_TIME=10
# SIT_AND_WAIT_MOD=("router")
#
# Needed if BGPsec is part of the experimentation / needed for BRIO
# Default value is calculated to the <install directory>/etc/srxcryptoapi.conf"
# CFG_SCA_NAME=""
#
# ENABLE_ROUTER=0
# ENBALE_BRIO2=0
#
# ROUTER_PRG=router.sh
# Default value is calculated to the <examples directory>/bin"
# ROUTER_FLDR=""
#
# CFG_CACHE_NAME="cache.script"
# CFG_BRIO1_NAME="brio1.script"
# CFG_BRIO2_NAME="-"

# Command line parameter examples for BRIO1 and BRIO2
# BRIO1_PARAMS="--show-settings"
# BRIO2_PARAMS=""
# ROUTER_PARAMS=""


EXPERIMENT_NAME="downstream-exp14"

ENABLE_ROUTER=1
ENABLE_BRIO2=0

CFG_CACHE_NAME="exp14.brio_rc.script"
CFG_BRIO1_NAME="exp14.brio_tg.as65040.conf"
CFG_BRIO2_NAME="-"

_STARTER="../bin/start_example.sh"
if [ -e $_STARTER ] ; then
  _BRIO_CALLER=$(pwd | sed -e "s#.*/\([^/]*\)#\1#g")
  source $_STARTER
else
  echo "Cannot find '$_STARTER', Abort Operation"
  exit 1 
fi
