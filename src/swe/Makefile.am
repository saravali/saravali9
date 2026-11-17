AM_CFLAGS = -w

noinst_LIBRARIES = libswe.a
libswe_a_SOURCES = \
	swecl.c \
	swedate.c \
	swedate.h \
	swehouse.c \
	swehouse.h \
	swejpl.c \
	swejpl.h \
	swemini.c \
	swemmoon.c \
	swemplan.c \
	swemptab.h \
	swenut2000a.h \
	sweodef.h \
	sweph.c \
	sweph.h \
	swephexp.h \
	swephlib.c \
	swephlib.h 

EXTRA_PROGRAMS = swetest
swetest_SOURCES = swetest.c swehel.c
swetest_LDADD = libswe.a -lm

EXTRA_DIST = Makefile.mingw swe.vcxproj

clean:
	rm -f lib*.a *.o *~ *.bak swetest
	rm -fr obj bin .libs

