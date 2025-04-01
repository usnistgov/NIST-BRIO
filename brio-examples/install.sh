#!/bin/bash

MY_FOLDER="opt/brio-examples"
FOLDERS=( $(ls -d */) )
LOCAL_FLDR=""

#
# Attempt to find the local install folder. This only functions if the install
# folder is sandboxed. This means ../local-xxxx
#
# The install folder name will be saved in LOCAL_FLDR
#
# @retunr 0 if a folder was found, otherwise 1
#
function find_local()
{
  local RETVAL=1
  local locFldr=( $(ls .. | grep -e "^local*" ) )

  for folder in ${locFldr[@]} ; do
    if [ -e ../$folder/bin/brio_tg ] ; then
      LOCAL_FLDR="../$folder"
      RETVAL=0
      break
    fi
  done

  return $RETVAL
}

find_local

# Print the given text and exit the shell scipt
# 
# $1 The text to display
# $2 The exit code or 0
#
function print_and_exit()
{
  if [ "$1" != "" ] ; then
    echo $1
    shift
  fi
  if [ "$1" == "" ] ; then
    exit
  else
    exit $1
  fi 
}

# 
# Print the program syntax and exit with the given error code
#
# $1 Exit code (optional)
#
function syntax()
{
  local RETVAL=0
  if [ "$1" != "" ] ; then
    RETVAL=$1
  fi
  echo "Syntax: $0 -I | <install-dir>"
  echo
  echo "  -I   Detect the installation folder and use it as install directory"
  print_and_exit " " $RETVAL
}

#
# Check if the given $1 parameter is not 0. In this case
# exit the program and print the text in $2
#
function checkRetVal()
{
  if [ ! $1 -eq 0 ] ; then
    print_and_exit "$2" $1
  fi
}

##############################################################################
##     Main Section
##############################################################################

if [ "$1" == "" ] ; then
  syntax 0
fi

# Check that the project is properly configured.
./configure.sh -c
_retVal=$?
if [ $_retVal -eq 1 ] ; then
  exit $_retVal
fi

INSTALL_FOLDER=" "
while [ "$1" != "" ] ; do
  case "$1" in
    "-?" | "-h" | "-H" | "?")
      syntax 0
      ;;
    "-I")
      find_local
      if [ $? -eq 0 ] ; then
        INSTALL_FOLDER=$( echo "$LOCAL_FLDR"/$MY_FOLDER | sed -e "s#//#/#g")
        echo "Detected installation directory '$INSTALL_FOLDER'"
      else
        echo "ERROR: Could not detect instalation folder"
      fi
      ;;
    *)
      INSTALL_FOLDER=$( echo "$1"/$MY_FOLDER | sed -e "s#//#/#g")        
      ;;
  esac
  shift
done

if [ "$INSTALL_FOLDER" == " " ] ; then
  echo "Install directory ' ' invalid!"
  print_and_exit "Abort installation!"
fi

if [ ! -e $INSTALL_FOLDER ] ; then
  echo "Create folder $INSTALL_FOLDER"  
  mkdir -p $INSTALL_FOLDER
  _RET_VAL=$?
  if [ ! $? -eq 0 ] || [ ! -e $INSTALL_FOLDER ]; then
    echo "An error occurred during creation of the install folder."
    print_and_exit "Abort operation" 1
  fi
fi

# backup of install folder is not necessary, uninsall does a unique sort
INSTALL_LOG=$0.log

# Now install each module within the examples folder exept template files.
for folder in ${FOLDERS[@]} ; do
  echo "Install $folder in  $INSTALL_FOLDER"
  _INSTALL=( $(find $folder -type d) )
  for instFolder in ${_INSTALL[@]} ; do
    mkdir -p $INSTALL_FOLDER/$instFolder >> /dev/null 2>&1
    checkRetVal $? "Error during install of '$folder', abort install"
  done
  _INSTALL=( $(find $folder | sed -E "s/.*\.tpl(\.md)?$//g" | sed -e "/^$/d") )
  for instFile in ${_INSTALL[@]} ; do
    cp $instFile $INSTALL_FOLDER/$instFile >> /dev/null 2>&1
  done
  
  echo "$INSTALL_FOLDER/$folder" >> $INSTALL_LOG
done
# write the README.md file into local - needs some link replacement
echo "Install README.md in $INSTALL_FOLDER"
cat README.md | sed -e "s/README\.tpl\.md)/README.md)/g" > $INSTALL_FOLDER/README.md

print_and_exit "Done." 0
