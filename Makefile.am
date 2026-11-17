
SUBDIRS = po src debian
DIST_SUBDIRS = po src debian test

# m4 not automatically included on OS X make dist
EXTRA_DIST = config.h autogen.sh Makefile.mingw saravali9.desktop \
	Doxyfile create_mac_release.sh MacOS/* m4/* \
	saravali.sln json_metacompiler.py

pkgdatadir = ${datarootdir}/saravali9

dist_pkgdata_DATA = COPYING 


all-am: echo-font-message

echo-font-message:
	@echo ""
	@echo ""
	@echo "HOW TO START"
	@echo "------------"
	@echo "You can start the program with the following commands"
	@echo "1. local installation: ./src/gui/saravali9"
	@echo "2. system wide installation: type as root 'make install'. The program can be started with 'saravali9'"
	@echo ""
	@echo ""
	@echo "FONT INFO"
	@echo "---------"
	@echo "The symbol font will be installed on most systems by 'make install'."
	@echo "You can also manually install it."
	@echo "It is located at src/resources/font/Saravali.ttf"
	@echo ""

# AUTOMAKE_OPTIONS = dist-bzip2 dist-zip
# AUTOMAKE_OPTIONS = dist-bzip2 
AUTOMAKE_OPTIONS = dist-bzip2 no-dist-gzip

ACLOCAL_AMFLAGS = -I m4

desktopdir = ${datarootdir}/applications
desktop_DATA = saravali9.desktop

clean: clean-recursive
	rm -f *~ *.bak fonts
	rm -fr autom4te.cache doc/* logs bin obj Saravali*.app locale

