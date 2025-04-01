#!/bin/bash
# The following parameters are defined in the main start program
# but can be overwritten here. For more detail on that they do
# please see the file ../bin/start_example.sh

# PORT_CACHE=50000
# PORT_SRX=17900
# PORT_ROUTER=179
# STOP_ALL_LOOP=3
# LISTEN_TIMEOUT=10
# SUDO_MODULES+=("router")
# SIT_AND_WAIT_TIME=5
# SIT_AND_WAIT_MOD=("router")

# BRIO1_PARAMS="--show-settings"
# BRIO2_PARAMS=[Additional command line parameter]*

# By Default, all are enabled (1), to diable set to (0).
# ENABLE_CACHE=1
ENABLE_SCA=0
ENABLE_SRX=0
ENABLE_ROUTER=0
# ENABLE_BRIO1=1
ENABLE_BRIO2=0

CFG_CACHE_NAME="exp8.brio_rc.script"
CFG_SRX_NAME="-"
CFG_SCA_NAME="-"
CFG_ROUTER_NAME="-"
CFG_BRIO1_NAME="exp8.brio_tg.as65050.conf"
CFG_BRIO2_NAME="-"

_STARTER="../bin/start_example.sh"
if [ -e $_STARTER ] ; then
  _BGP_SRX_CALLER=$(pwd | sed -e "s#.*/\([^/]*\)#\1#g")
  source $_STARTER 
else
  echo "Cannot find '$_STARTER', Abort Operation"
  exit 1 
fi
