
FORBIDDEN = testdir/forbidden

AM_CPPFLAGS = -I. -I../../src/base -I../../src/astro $(WX_CPPFLAGS)

AM_CPPFLAGS += $(GTEST_CFLAGS)

noinst_bin_PROGRAMS = TestRunner

noinst_bindir = .

.cpp.o :
	$(CC) $(CXXFLAGS) -c -o$@ $*.cpp $(AM_CPPFLAGS)

TestRunner_SOURCES = \
	AshtakavargaTest.cpp \
	ArabicPartsTest.cpp \
	DasaTest.cpp \
	DE200Test.cpp \
	EphemTest.cpp \
	GdiTest.cpp \
	JsonTest.cpp \
	TestRunner.cpp \
	TestRunner.h \
	ShadbalaTest.cpp \
	UranianTest.cpp \
	VedicTest.cpp

TestRunner_LDADD = ../../src/astro/libsaravali_astro.a \
	../../src/base/libsaravali_base.a \
	../../src/swe/libswe.a \
	../../src/jsoncpp/libjsoncpp.a

TestRunner_LDADD += $(GTEST_LIBS) $(WX_LIBS) $(LIBINTL)
TestRunner_DEPENDENCIES = ../../src/base/libsaravali_base.a ../../src/swe/libswe.a

all-am:
	@echo "How to build and run the tests"
	@echo "------------------------------"
	@echo "  'make' to build"
	@echo "  'make test' to build and run the tests"
	@echo "You need Google Test (gtest) to build and run the tests."

test: testdir TestRunner
	TestRunner

testdir:
	prepare_dirs.sh

clean:
	if test -d $(FORBIDDEN); \
	then chmod -f 755 $(FORBIDDEN); \
	fi
	rm -fr testdir
	rm -f lib*.a *.o core *~ *.bak TestRunner
	rm -fr obj bin .libs

EXTRA_DIST = prepare_dirs.sh Makefile.mingw 


