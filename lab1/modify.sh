#!/bin/sh

modify()
{
	# split argument into path and name
	basename="$(basename $1)"
	dirname="$(dirname $1)"
	
	# modify name according to mode
	case $mode in
		lowercase)
			basename="$(echo $basename | tr [:upper:] [:lower:])"
			;;
		uppercase)
			basename="$(echo $basename | tr [:lower:] [:upper:])"
			;;
		sed)
			basename="$(echo $basename | sed $sedpattern)"
			;;
	esac
	
	# implement change, don't overwrite files and don't move into directories
	mv -nT "$1" "$dirname/$basename"
}

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

# shift out options
shift "$((OPTIND-1))"

# probe and shift out sed pattern
if [ $mode = "sed" ]; then
	sedpattern=$1
	shift 1
fi

# ensure all specified files/directories exist
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
	
	modify $argument
done
