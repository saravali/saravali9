
AM_CPPFLAGS = -I. -w -march=native $(WX_CPPFLAGS)

noinst_LIBRARIES = libsaravali_db.a

libsaravali_db_a_SOURCES = \
	sqlite3mc_amalgamation.c \
	sqlite3mc_amalgamation.h \
	sqlite3mc_config.h \
	wxsqlite3.cpp \
	wx/*.h

all-am: 

clean:
	rm -f lib*.a *.o core *~ *.bak 
	rm -fr obj bin .libs

EXTRA_DIST = wx wxsqlite3_version.rc Makefile.mingw wxsqlite3.vcxproj LICENSE.txt

