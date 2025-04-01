#!/bin/bash

if [ "$2" == "" ] ; then
	echo "Syntax: $0 fromExp toExp"
	exit 1
fi

from="$1"
to="$2"

# Gather all from files
fromFiles=( $(ls exp$from.* start_exp$from.sh 2>/dev/null) )
toFiles=( $(ls exp$to.* start_exp$to.sh 2>/dev/null) )

if [ ${#fromFiles[@]} -eq 0 ] ; then
       echo "No Experiments exp$from found!"
       exit 1
fi

if [ ${#fromFiles[@]} -eq ${#toFiles[@]} ] ; then
       echo "It seems all files are already generated!"
       num=0
       while [ $num -lt ${#fromFiles[@]} ] ; do
	       echo "${fromFiles[$num]}          ${toFiles[$num]}"
	       ((num++))
       done
       exit 1
fi

for file in ${fromFiles[@]} ; do
	toFile="$(echo $file | sed -e "s/$from/$to/g")-tmp"
	if [ -e $toFile ] ; then
		echo "$toFile already exists!"
	else
		echo -n "Generate $toFile..."
		cp $file $toFile 1>/dev/null 2>&1
		if [ $? -eq 0 ] ; then
			echo "done."
		else
			echo "failed."
		fi
	fi
done
