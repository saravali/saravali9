
# avoid warning about unused yyunput and yyinput
LEX_CPPFLAGS = -DYY_NO_UNPUT -DYY_NO_INPUT
AM_CPPFLAGS = -I. -I../base -I../jsoncpp $(LEX_CPPFLAGS) $(WX_CPPFLAGS)

noinst_PROGRAMS = saravali9cli
saravali9cli_SOURCES = textclient.cpp

saravali9cli_LDADD = libsaravali_astro.a ../base/libsaravali_base.a ../jsoncpp/libjsoncpp.a \
	../swe/libswe.a ../wxpdfdoc/libsaravali_pdf.a ../wxsqlite3/libsaravali_db.a $(WX_LIBS)

noinst_LIBRARIES = libsaravali_astro.a

libsaravali_astro_a_SOURCES = \
	ArabicParts.cpp \
	ArabicParts.h \
	Ashtakavarga.cpp \
	Ashtakavarga.h \
	Ashtakoota.cpp \
	Ashtakoota.h \
	AshtottariDasa.cpp \
	astrobase.cpp \
	astrobase.h \
	CompositeHoroscope.cpp \
	CompositeHoroscope.h \
	ContextProperties.cpp \
	ContextProperties.h \
	Dasa.cpp \
	Dasa.h \
	DasaTool.cpp \
	DasaTool.h \
	Dignity.cpp \
	Dignity.h \
	Direction.cpp \
	Direction.h \
	DoubleTermList.cpp \
	DoubleTermList.h \
	Eclipse.cpp \
	Eclipse.h \
	Ephemeris.cpp \
	Ephemeris.h \
	EventFinder.cpp \
	EventFinder.h \
	Expert.cpp \
	Expert.h \
	Formulary.cpp \
	Formulary.h \
	GenericDasa.cpp \
	GenericDasa.h \
	Hora.cpp \
	Hora.h \
	Horoscope.cpp \
	Horoscope.h \
	Jaimini.cpp \
	Jaimini.h \
	KalachakraDasa.cpp \
	KalachakraDasa.h \
	MatchWriter.cpp \
	MatchWriter.h \
	Nakshatra.cpp \
	Nakshatra.h \
	PlanetBalance.cpp \
	PlanetBalance.h \
	PlanetaryPicture.cpp \
	PlanetaryPicture.h \
	SpWriter.cpp \
	SpWriter.h \
	Shadbala.cpp \
	Shadbala.h \
	SolarHoroscope.cpp \
	SolarHoroscope.h \
	TableWriter.cpp \
	TableWriter.h \
	Term.cpp \
	Term.h \
	TermList.cpp \
	TermList.h \
	TermSolver.cpp \
	TermSolver.h \
	textclient.cpp \
	TextHelper.cpp \
	TextHelper.h \
	Uranian.cpp \
	Uranian.h \
	UranianHelper.cpp \
	UranianHelper.h \
	UranianTextLoader.cpp \
	UranianTextLoader.h \
	Varga.cpp \
	Varga.h \
	VedicAspect.cpp \
	VedicAspect.h \
	WesternAspect.cpp \
	WesternAspect.h \
	YdlInterpreter.cpp \
	YdlInterpreter.h \
	Yoga.cpp \
	Yoga.h 

clean:
	rm -f lib*.a *.o y.tab.* *core *~ *.bak *.cc
	rm -fr obj bin .libs logs *.dSYM $(noinst_PROGRAMS)
	rm -f $(bin_PROGRAMS)

EXTRA_DIST = Makefile.mingw astro.vcxproj astrocli/astrocli.vcxproj

