#!/bin/sh

echo "Running aclocal, automake and autoconf ..."

mkdir -p m4
aclocal -I m4 --install || exit 1
automake -a || exit 1
autoconf || exit 1

echo "Done"

