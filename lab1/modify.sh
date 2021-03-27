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

# ensure all specified files/directories exist
shift "$((OPTIND-1))"
for argument in "$@"
do
	if [ ! -e $argument ]; then
		echo "File $argument doesn't exist" >&2
		exit 2
	fi
done

# parse arguments
for argument in "$@"
do
	if [ -d $argument ]	&& $recursive; then
		echo "parsing directory recursively"
		continue
	fi
	
	# split argument into path and name
	basename="$(basename $argument)"
	dirname="$(dirname $argument)"
	
	# modify name according to mode
	case $mode in
		lowercase)
			basename="$(echo $basename | tr [:upper:] [:lower:])"
			;;
		uppercase)
			basename="$(echo $basename | tr [:lower:] [:upper:])"
			;;
		sed)
			echo "not implemented"
			;;
	esac
	
	# implement change, don't overwrite files and don't move into directories
	mv -nT "$argument" "$dirname/$basename"
done
