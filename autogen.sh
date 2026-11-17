#!/bin/sh

echo "Running aclocal, autoheader, automake and autoconf ..."

mkdir -p m4
aclocal -I m4 --install || exit 1
autoheader --force || exit 1
autoreconf --force --install || exit 1
# intltoolize --copy --force --automake || exit 1

echo "Done"

