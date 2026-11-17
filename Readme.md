This is the repository of Saravali9, the successor of Maitreya8.

Saravali is an Open Source platform for Vedic and western astrology.
Supported platforms are Windows, Max OS X, Linux and BSD.

# New Features in Version 9

- New views for Uranian Astrology: Uranian Chart, Sensitive points, Planetary Pictures
- Multidirection view (multiple directions like transit and Solar Arc in one view)
- Event Finder View shows multidirection events for a given time span
- New types of charts, e.g. mixed types
- New functions for Yoga calculation (Yoga configurations still missing)
- Improved configuration management for printout
- Yearly horoscope (based on 0 Capricorn return of Sun)
- New philosophy of *Viewpoints* allows for flexible handling of configuration options (advanced)
- New license: http://unlicense.org, i.e. the software is public domain

# Compilation
The software is based on wxWidgets 3.2 and C++26. Since C++26 is quite new, there are
wrapper scripts for compilers with C++23 support. This will likely be removed in the future.

## Linux/BSD

With gcc-16

```
export CXXFLAGS="-std=c++26 -DCPP26 -freflection --with-wx-config=xxx"
```

Otherwise (including Clang)

```
export CXXFLAGS="-std=c++23 --with-wx-config=xxx"
```

## Visual Studio 2022

Compile wxWidgets with Visual Studio, set the environment variable WXWIDGETS to the directory (e.g. c:\wxWidgets-3.2.12).

Open saravali.sln. Make "gui" the start project and build.

## Mingw

Install gcc-16 and wxWidgets with pacman.

```
	make -f Makefile.mingw
```

