
This projekte contains the (Google) test cases for Saravali9 plus some further resources.

It is still quite incomplete.

## Prerequisites

You need the Swiss Ephemeris data files in the directory src/swe. This can be a symbolic to whereever you have the files.

Expected contents in the swe directory are the (version 2.0) data files for the current epoch.

The required contents looks like this

```
$ ls -1l
total 3944
-rw-r--r--  1 martin  staff   223002 16 Mai  2020 seas_18.se1
-rw-r--r--  1 martin  staff  1304771 16 Mai  2020 semo_18.se1
-rw-r--r--  1 martin  staff   484055 16 Mai  2020 sepl_18.se1

```

## How to Build

You need Google test as prerequisite.

Currently only Linux/macOS is supported. Mingw and Visual Studio are to be done.

Build params should be the same as under the main project (autogen + configure + make).

## Test Cases

Are under the src directory. Run it from there (make test).

## Yoga Editor Scripts

Can be found under src/resources/yoga.

