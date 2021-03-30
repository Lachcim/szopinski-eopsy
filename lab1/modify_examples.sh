#!/bin/sh

echo "Invalid use cases:\n"

( set -x; ./modify.sh )
( set -x; ./modify.sh -x test )
( set -x; ./modify.sh -lu )
( set -x; ./modify.sh -l /dev/nonexistent )
( set -x; ./modify.sh s/invalid . )

echo "\nInitializing example file system:\n"

if [ -e /tmp/modify_examples ]; then
	rm -rf /tmp/modify_examples
fi

mkdir /tmp/modify_examples

mkdir /tmp/modify_examples/test_basic
touch /tmp/modify_examples/test_basic/file1
touch /tmp/modify_examples/test_basic/file2
touch /tmp/modify_examples/test_basic/file3
mkdir /tmp/modify_examples/test_basic/dir1
mkdir /tmp/modify_examples/test_basic/dir2

mkdir /tmp/modify_examples/test_recursion
touch /tmp/modify_examples/test_recursion/file1
mkdir /tmp/modify_examples/test_recursion/dir1
touch /tmp/modify_examples/test_recursion/dir1/file2
mkdir /tmp/modify_examples/test_recursion/dir1/dir2
touch /tmp/modify_examples/test_recursion/dir1/dir2/file3
mkdir /tmp/modify_examples/test_recursion/dir1/dir2/dir3

mkdir /tmp/modify_examples/test_special
touch "/tmp/modify_examples/test_special/file1 file1"
touch /tmp/modify_examples/test_special/-n
touch /tmp/modify_examples/test_special/!file2
touch /tmp/modify_examples/test_special/*file3
touch /tmp/modify_examples/test_special/\&file4
touch /tmp/modify_examples/test_special/\"file5
touch /tmp/modify_examples/test_special/\$file6

find /tmp/modify_examples

echo "\nBasic use cases:\n"

( set -x; ./modify.sh -h )
( set -x; ./modify.sh -u /tmp/modify_examples/test_basic/* )
find /tmp/modify_examples/test_basic
( set -x; ./modify.sh -l /tmp/modify_examples/test_basic/* )
find /tmp/modify_examples/test_basic
( set -x; ./modify.sh s/file/myfile/ /tmp/modify_examples/test_basic/* )
find /tmp/modify_examples/test_basic

echo "\nRecursive use cases:\n"

( set -x; ./modify.sh -ur /tmp/modify_examples/test_recursion )
find /tmp/modify_examples/test_recursion
( set -x; ./modify.sh -lr /tmp/modify_examples/test_recursion )
find /tmp/modify_examples/test_recursion

echo "\nSpecial character use cases:\n"

( set -x; ./modify.sh -ur /tmp/modify_examples/test_special )
find /tmp/modify_examples/test_special
( set -x; ./modify.sh -lr /tmp/modify_examples/test_special )
find /tmp/modify_examples/test_special

echo "\nTest over."
rm -rf /tmp/modify_examples
