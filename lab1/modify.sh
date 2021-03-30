#!/bin/sh

modify()
{
	# split argument into path and name
	basename=$(basename -- "$1")
	dirname=$(dirname -- "$1")
	
	# modify name according to mode
	case $mode in
		lowercase)
			outputname="$(echo ./$basename | tr [:upper:] [:lower:])"
			;;
		uppercase)
			outputname="$(echo ./$basename | tr [:lower:] [:upper:])"
			;;
		sed)
			outputname="$(echo ./$basename | sed $sedpattern)"
			;;
	esac
	
	# implement change, don't overwrite files and don't move into directories
	mv -nT "$dirname/$basename" "$dirname/$outputname"
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
			echo "Usage:"
			echo "\tmodify [-r] [-l|-u] <dir/file names...>"
			echo "\tmodify [-r] <sed pattern> <dir/file names...>"
  			echo "\tmodify [-h]"
			exit
			;;
	esac
done

# shift out options
shift "$((OPTIND-1))"

# probe and shift out sed pattern
if [ $mode = "sed" ]; then
	if [ $# = 0 ]; then
		echo "Missing sed pattern" >&2
		exit 1
	fi
	
	sedpattern=$1
	shift
fi

# ensure all specified files/directories exist
for argument in "$@"
do
	if [ ! -e "$argument" ]; then
		echo "File $argument doesn't exist" >&2
		exit 2
	fi
done

# parse arguments
for argument in "$@"
do
	# handle directories recursively
	if [ -d "$argument" ] && $recursive; then
		# obtain list of files within directory
		files=$(find "$argument" -type f)
		
		# modify each file
		IFS='
		'
		for file in $files
		do
			modify "$file"
		done
		
		continue
	fi
	
	# standard mode of operation
	if [ -f "$argument" ]; then
		modify "$argument"
	fi
done
