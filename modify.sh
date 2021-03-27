#!/bin/sh

# define mode of operation
mode="sed"
recursive=false

# parse command line options
while getopts 'hrlu' option
do
	case $option in
		l)
			# prevent conflict
			if [ $mode = "uppercase" ]; then
				echo "Cannot combine -l and -u" >&2
				exit 1
			fi
			
			# set translation mode
			mode="lowercase"
			;;
		u)
			# prevent conflict
			if [ $mode = "lowercase" ]; then
				echo "Cannot combine -l and -u" >&2
				exit 1
			fi
			
			# set translation mode
			mode="uppercase"
			;;
		r)
			# mark as recursive
			recursive=true
			;;
		h)
			# print help and quit
			echo help
			exit
			;;
	esac
done
