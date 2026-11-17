#!/bin/sh

echo "Prepare files and directories .."

rm -fr testdir

mkdir testdir

cd testdir

touch file1 file2 file3

echo "0123456789" > file4.txt

echo "0123456789" > file4a.TXT

chmod 000 file3

mkdir -p "forbidden"

chmod 000 "forbidden"

echo "done"

