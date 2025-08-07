#!/bin/bash
#
# NIST-developed software is provided by NIST as a public service. You may use,
# copy, and distribute copies of the software in any medium, provided that you
# keep intact this entire notice. You may improve, modify, and create derivative
# works of the software or any portion of the software, and you may copy and
# distribute such modifications or works. Modified works should carry a notice
# stating that you changed the software and should note the date and nature of
# any such change. Please explicitly acknowledge the National Institute of
# Standards and Technology as the source of the software.
#
# NIST-developed software is expressly provided "AS IS." NIST MAKES NO WARRANTY
# OF ANY KIND, EXPRESS, IMPLIED, IN FACT, OR ARISING BY OPERATION OF LAW,
# INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT, AND DATA ACCURACY. NIST
# NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE
# UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES
# NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR
# THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY,
# RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
#
# You are solely responsible for determining the appropriateness of using and
# distributing the software and you assume all risks associated with its use,
# including but not limited to the risks and costs of program errors, compliance
# with applicable laws, damage to or loss of data, programs or equipment, and
# the unavailability or interruption of operation. This software is not intended
# to be used in any situation where a failure could cause risk of injury or
# damage to property. The software developed by NIST employees is not subject to
# copyright protection within the United States.
#
# This script requires a bash shell, grep, awk, and sed to be available
#
# 
# Version 3.1.0
#  * Check if dependency folder and .req files exist.
#  * Add --create to generate template .req file.
#  * Added detection of OS version.
# Version 3.0.2
#  * Fixed issues with update and upgrade of systems.
#  * Removed Ubuntu specialization from this script and be used in the init
#    and update configuration.
# Version 3.0.1
#  * Added shell check 
#  * Added check for package installer
# Version 3.0.0
#  * Added REQ to allow simple configuration of requirment files
#  * Allow blank and tabs prior to package or repo tags
#  * Moved OS install initialization into requirement files
# Version 2.1.0
#  * Added Linux Mint
# Version 2.0.0
#  * Simplified the package installation by adding using requirements files
#    With this change only the case section for the OS specific installers needs
#    to be modified
# Version 1.3.0
#  * Added Alpine OS
#

# The version of this script
VERSION=3.1.0
# Detect the home directory
HOME_DIR="$(dirname "$(realpath "$0")")"
# Directory for the requirements files ubuntu.req, rocky.req, ...
REQ_DIR="$HOME_DIR/dependencies";
# The extension for requirement files
REXT="req"

# The name of hte OS, will be auto detected later on
OS_NAME=
# Control the output when installing packages and repos. 0=less, 1=more
verbose=0
# Assure the user confirms to the usage of this tool (0-no, 1=yes)
confirm=0
# Indicates that this instance runs as a recursive one
RECURSIVE=0
# Initialize config variable CFG (used to find configs in the dependency file) 
CFG=""

# Verify the active shell is bash:
if [ ! "$BASH" ] ; then
  echo "ERROR: Restart script in bash shell!"
  exit 1
fi

# Detect the OS name. This needs to be here because some OS's do not have
# bash installed by default. This needs to be done first.
if [ -e /etc/os-release ] ; then
  OS_NAME=$( grep -e "^ID=" /etc/os-release | awk -F'=' '{print $2}' | sed -e "s/\"//g" )
else
  OS_NAME=$( uname )
fi

if [ -f /etc/os-release ]; then
  OS_NAME=$( grep -e "^ID=" /etc/os-release | awk -F'=' '{print $2}' | sed -e "s/\"//g" )
  OS_VERSION=$( grep -e "^VERSION_ID=" /etc/os-release | awk -F'=' '{print $2}' | sed -e "s/\"//g" )
elif command -v sw_vers >/dev/null; then
  OS_NAME=$(sw_vers -productName)
  OS_VERSION=$(sw_vers -productVersion)
else
  OS_NAME=$(uname -s)
  OS_VERSION=$(uname -r)
fi
OS_NAME=$(echo $OS_NAME | sed -E 's#^\b([^ -/]+)\b.*#\1#g')
OS_VERSION=$(echo $OS_VERSION | sed -E 's#^([^\.]+)\..*#\1#g')
OS_KEY=$(echo "${OS_NAME}-${OS_VERSION}" | awk '{print tolower($0)}')

echo
echo "Detected $OS_NAME $OS_VERSION operating system!"
echo

################################################################################
## Provide a clean exit of the program
################################################################################
# Exits the script: clean_exit [$1 [$2]]
# $1 Exit code (Default 0)
# $2 Message
function clean_exit() 
{
  local ERRLEVEL=0
  if [ "$1" != "" ] ; then
    ERRLEVEL=$1
    shift
  fi

  echo

  if [ "$1" != "" ] ; then
    echo "$1"
    echo
  fi

  echo "Dependencies Installer Version $VERSION"
  echo
  exit $ERRLEVEL
}

# Retrieve a configuration value out of the given file
#
# $1 the file
# $2 the configuation key
# $3 default value (optional)
#
# Store the found value in '$CFG'
function getCFG()
{
  CFG="$3"
  local file=$1
  local param=$2
  local value=""

  if [ "$file" != "" ] ; then
    if [ -e $file ] ; then
      value=$(cat $file | grep -e "^[ \t]*cfg[ \t]\+$param:[ \t]*")
      if [ "$value" != "" ] ; then
        value=$( echo $value | sed -e "s/[^:]\+:[ \t]*//g" )
      fi
    fi
  fi

  if [ "$value" != "" ] ; then
    CFG="$value"
  fi
}

# This function verifies a variable with the given name exists and is not empty
#
# $1 just the vairable name, not the variable (e.g. For $myVar: only myVar)
# $2 exit code (optional) 0=no exit, just print
function assert()
{
  local value=""
  eval "value=\$$1"

  if [ "$value" == "" ] ; then
    case "$2" in
      "") ;;
      "0") echo "INFO: Variable '$1' not set!" ;;
      *) echo "ERROR: Variable '$1' not set!"; exit $2 ;;
    esac
  else
    if [ "$2" == "0" ] ; then
      echo "Variable $1=\"$value\""
    fi    
  fi
}

# Create a dependency file/template
# $1 os name (optional)
function setup_dependency()
{
  local _os_dep=$REQ_DIR/"${1:-template}.req"
  local _init=""
  local _pkg_installer=""
  local _yes=""
  local _install=""
  local _update=""
  local _repo=""

  mkdir -p $REQ_DIR

  echo "# For packages, use pkg:<name>" >> $_os_dep
  echo "# For repositories, use repo:<name>" >> $_os_dep
  echo "# Multiple packages and repos can be specified in the same line" >> $_os_dep
  echo "# e.g. pkg: <name1> <name2>" >> $_os_dep
  echo >> $_os_dep
  case "$1" in
    "rocky")
      _init="dnf -y install 'dnf-command(config-manager)'"
      _pkg_installer="dnf"
      _repo_installer="dnf config-manager --enable"
      _yes="-y"
      _install="install"
      _update="update"
      _repo="devel"
      ;;
    "ubuntu")
      _init=""
      _pkg_installer="apt"
      _repo_installer=""
      _yes="-y"
      _install="install"
      _update="update; apt -y upgrade"
      ;;
    *)
      ;;
  esac
  echo "cfg init: $_init" >> $_os_dep
  echo "cfg pkg_installer: $_pkg_installer" >> $_os_dep
  echo "cfg repo_installer: $_repo_installer" >> $_os_dep
  echo "cfg yes: $_yes" >> $_os_dep
  echo "cfg install: $_install" >> $_os_dep
  echo "cfg update: $_update" >> $_os_dep
  echo >> $_os_dep
  echo "#tt Packages needed for this project" >> $_os_dep
  echo "pkg: " >> $_os_dep
  echo >> $_os_dep
  echo "#tt Repositories needed Required" >> $_os_dep
  echo "repo: $_repo" >> $_os_dep
}

################################################################################
## Process program parameters
################################################################################

# store the parameters in case it is detected that bash needs to be installed.
_restartParam="$@"
while [ "$1" != "" ] ; do
  case "$1" in
    "-v")
      verbose=1
      ;;
    "--confirm")
      confirm=1
      ;;
    "--restarted")
      RECURSIVE=1
      ;;
    "--create")
      setup_dependency $2
      clean_exit 0
      ;;
    "-?")
      echo "Usage: $0 [v] <--confirm|--create [OS]>"
      clean_exit 0
      ;;
    *)
      echo "Invalid parameter '$1'"
      clean_exit 1;
      ;;
  esac
  shift
done


################################################################################
## Verify that dependencies are available
################################################################################
# Test if requirements folder is found!
if [ ! -e $REQ_DIR ] ; then
  echo
  echo "ERROR: No dependencies folder found!"
  echo
  clean_exit 1
else
  ls $REQ_DIR/*.req &>/dev/null
  if [ $? -ne 0 ] ; then
    echo
    echo "WARNING: No dependency files found! (e.g. $OS_NAME.req)"
    echo
  fi
fi

################################################################################
## Verify that the script is allowed to proceed
################################################################################
if [ $confirm -eq 0 ] ; then
  echo "#######################################################################"
  echo "#                                                                     #"
  echo "#                        W A R N I N G                                #"
  echo "#                        =============                                #"
  echo "#                                                                     #"
  echo "# This script will attempt to install software on the system. First   #"
  echo "# verify with your system administrator that it is safe to install    #"
  echo "# the packages.                                                       #"
  echo "# Requirements are stored in the requirements file OS_NAME.req        #"
  echo "#                                                                     #"
  echo "#                   USE AT YOUR OWN RISK !!                           #"
  echo "#                                                                     #"
  echo "#                                                                     #"
  echo "#        To enable this script start with --confirm                   #"  
  echo "#                                                                     #"
  echo "#######################################################################"
  clean_exit 1
fi

################################################################################
## Pre-run checkup (currently only for alpine)
################################################################################
if [ ! -n "$BASH_VERSION" ]; then
  if [ $RECURSIVE -eq 1 ] ; then
    clean_exit 1 "This instance is already in recursive mode - bash expected!"
  fi
  echo "Installer is currently not running in bash, restart in bash..."
  case $OS_NAME in
    "alpine")
        which bash 1>/dev/null 2>&1
        if [ ! $? -eq 0 ] ; then
          echo -n "Bash not found, install bash..."
          apk add --no-cache bash
          if [ $? -eq 0 ] ; then
            echo "done."
          else
            echo "failed!"
            clean_exit 1
          fi
        fi
        ;;
    *)
        ;;
  esac
  echo "Restart again..."
  bash $0 $_restartParam --restarted
  echo "Returned"
  exit $?
fi


################################################################################
## Load Requirements
################################################################################
echo -n "Load requirements for $OS_NAME..."

pkgs=( )
repos=( )

# First try to find the specific OS version file rocky-9 or ubuntu-22,....
if [ -e "$REQ_DIR/$OS_KEY.$REXT" ] ; then
  _reqFile=$REQ_DIR/$OS_KEY.$REXT
else # Else try to find the specific OS file rocky or ubuntu,....
  if [ -e "$REQ_DIR/$OS_NAME.$REXT" ] ; then
    _reqFile=$REQ_DIR/$OS_NAME.$REXT
  else
    echo "none found!"
    clean_exit 0 "OS: $OS_NAME not supported!"
  fi
fi
pkgs=(  $(cat "$_reqFile" | grep -e "^[ \t]*pkg:.*"  | sed -e "s/^[ \t]*pkg://g") )
repos=( $(cat "$_reqFile" | grep -e "^[ \t]*repo:.*" | sed -e "s/^[ \t]*repo://g") )
echo "found ${#pkgs[@]} packages and ${#repos[@]} repositories!"

echo "------------------------------------------------------------------------"
echo "Configure package and repository installers for $OS_NAME $OS_VERSION"
echo "------------------------------------------------------------------------"
getCFG $_reqFile init
  init="$CFG"
  assert init
getCFG $_reqFile pkg_installer
  pkg_installer="$CFG"
  assert pkg_installer 1
getCFG $_reqFile repo_installer
  repo_installer="$CFG"
  assert repo_installer
getCFG $_reqFile yes 
  _yes="$CFG"
  assert _yes 
getCFG $_reqFile install "install"
  _install="$CFG"
  assert _install 1
getCFG $_reqFile update "update"
  _update="$CFG"
  assert _update 1

if [ ${#repos[@]} -gt 0 ] && [ "$repo_installer" == "" ] ; then
  echo "ERROR: Repos are configured without a repo installer!"
  exit 1
fi

if [ "$init" != "" ] ; then
  echo "Initialize: "
  eval $init
fi

################################################################################
## Start configuration and installation
################################################################################

if [ ${#repos[@]} -gt 0 ] ; then
  echo "------------------------------------------------------------------------"
  echo "Add additional repositories..."
  echo "------------------------------------------------------------------------"

  for repo in ${repos[@]} ; do
    echo -n "Add repository '$repo'..."
    if [ $verbose -eq 0 ] ; then
      $repo_installer $repo > /dev/null 2>&1
    else
      $repo_installer $repo
    fi
    if [ $? -eq 0 ] ; then
      echo "[OK]"
    else
      echo "[Failed]"
    fi
  done;
fi

echo "------------------------------------------------------------------------"
echo "Update the System..."
echo "------------------------------------------------------------------------"
echo "$pkg_installer $_yes $_update"
eval $pkg_installer $_yes $_update

echo "------------------------------------------------------------------------"
echo "Install Packages..."
echo "------------------------------------------------------------------------"
_retVal=0
_retMessage=""
# Keep the names of the failes repos
_failed=( )
for package in ${pkgs[@]} ; do
  echo -n "Install package '$package'..."
  if [ $verbose -eq 0 ] ; then
    $pkg_installer $_install $_yes $package > /dev/null 2>&1
    if [ $? -eq 0 ] ; then
      echo "[OK]"
    else
      _failed+=( $package )
      echo "[Failed]"
    fi
  else
    echo
    echo "$pkg_installer $_install $_yes $package"
    $pkg_installer $_install $_yes $package
    if [ $? -eq 1 ] ; then
      _failed+=( $package )
    fi
  fi
done



if [ ${#_failed[@]} -gt 0 ] ; then
  _retVal=1
  _retMessage="ERROR: Failed to install ( ${_failed[@]} )!"
fi

clean_exit $_retVal "$_retMessage"