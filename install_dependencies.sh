#!/bin/bash

echo
echo "WARNING:"
echo "========"
echo
echo "Remove the line containing ```exit 1``` to allow this script to run."
echo "This script will attempt to install software on the system. First verify"
echo "with your system administrator that it is safe to install the packages."
echo "For Ubuntu stystems see the array pkg_UBUNTU, and for Rocky systems"
echo "See the array pkg_ROCKY."
echo
echo "USE AT YOUR OWN RISK !!"
echo
echo "Installation Stopped!"
echo
exit 1

if [ "$1" == "-v" ] ; then
  verbose=1
else
  verbose=0
fi

if [ -e /etc/os-release ] ; then
  OS_NAME=$( grep -e "^ID=" /etc/os-release | awk -F'=' '{print $2}' | sed -e "s/\"//g" )
else
  OS_NAME=$( uname )
fi

echo
echo "Detected '$OS_NAME' operating system"
echo 

pkg_installer=
repo_installer=
pkgs=( )
repos=( )

pkg_UBUNTU=( 
  vim
  unzip
  gcc
  automake
  make
  libtool
  openssl
  libreadline-dev
  libconfig-dev
  libssl-dev
  uthash-dev
  sed
  screen
)
repo_UBUNTU=( )

pkg_ROCKY=(
  gcc
  openssl
  epel-release
  autoconf
  net-tools
  sudo
  automake
  make
  libtool
  libconfig
  libconfig-devel
  openssl-devel
  uthash-devel
  readline-devel
  sed
  screen
)
repo_ROCKY=( devel )

echo "------------------------------------------------------------------------"
echo "Configure repos and packages.."
echo "------------------------------------------------------------------------"
case "$OS_NAME" in
  "ubuntu")
	  pkgs=( ${pkg_UBUNTU[@]} )
	  repos=( ${repo_UBUNTU[@]} )
	  pkg_installer=apt
    repo_installer="add-apt-repository" 
	  ;;
  "rocky")
	  pkgs=( ${pkg_ROCKY[@]} )
	  repos=( ${repo_ROCKY[@]} )
	  pkg_installer=dnf
    repo_installer="dnf config-manager --enable" 
    if [ ${#repos[@]} -gt 0 ] ; then
      dnf -y install 'dnf-command(config-manager)'
    fi
	  ;;
  *)
	  echo "OS $OS_NAME not yet supported"
	  exit 1
	  ;;
esac

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

echo "------------------------------------------------------------------------"
echo "Update the System..."
echo "------------------------------------------------------------------------"
$pkg_installer -y update
if [ "$OS_NAME" == "ubuntu" ] ; then
  $pkg_installer -y upgrade
fi

echo "Install Packages..."
for package in ${pkgs[@]} ; do
  echo -n "Install package '$package'..."
  if [ $verbose -eq 0 ] ; then
    $pkg_installer install -y $package > /dev/null 2>&1
  else
    $pkg_installer install -y $package
  fi
  if [ $? -eq 0 ] ; then
    echo "[OK]"
  else
    echo "[Failed]"
  fi
done

openssl version
echo
