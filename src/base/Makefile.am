
AM_CPPFLAGS = -I. -I../jsoncpp -I../wxsqlite3 -I../wxpdfdoc -I../swe $(WX_CPPFLAGS)

AM_CPPFLAGS += $(PLATFORM_OPTS)

# AM_CPPFLAGS += -DPREFIX=\"$(prefix)\" -DDATAROOTDIR=\"$(datarootdir)\"
AM_CPPFLAGS += -DDATAROOTDIR=\"$(datarootdir)\"

BUILT_SOURCES = YdlParser.hh

YdlLexer.ll : YdlParser.yy
YdlParser.hh : YdlParser.yy

YdlLexer.cc : YdlLexer.ll
	$(LEX) -t YdlLexer.ll > YdlLexer.cc

YdlParser.cc : YdlParser.yy
	$(YACC) -d YdlParser.yy
	mv y.tab.c YdlParser.cc
	mv y.tab.h YdlParser.hh

noinst_LIBRARIES = libsaravali_base.a

libsaravali_base_a_SOURCES = \
	ArabicPartBase.cpp \
	ArabicPartBase.h \
	AspectConfig.cpp \
	AspectConfig.h \
	astroconfig.cpp \
	astroconfig.h \
	AtlasDao.cpp \
	AtlasDao.h \
	AtlasImporter.cpp \
	AtlasImporter.h \
	AtlasLogic.cpp \
	AtlasLogic.h \
	BaseDao.cpp \
	BaseDao.h \
	bootstrap.h \
	Calculator.cpp \
	Calculator.h \
	CategoryHelper.cpp \
	CategoryHelper.h \
	ChartConfig.h \
	ChartGeometry.cpp \
	ChartGeometry.h \
	Commands.cpp \
	Commands.h \
	ConfigListItems.h \
	constants.h \
	ColorConfig.cpp \
	ColorConfig.h \
	ConfigListItems.h \
	ConfigListLoader.cpp \
	ConfigListLoader.h \
	ConfigLoaderImpls.h \
	DateFormatter.cpp \
	DateFormatter.h \
	datelocation.cpp \
	datelocation.h \
	enumserializer.cpp \
	enumserializer.h \
	Exporter.cpp \
	Exporter.h \
	FileConfig.cpp \
	FileConfig.h \
	Formatter.cpp \
	Formatter.h \
	FontTool.cpp \
	FontTool.h \
	gdibase.cpp \
	gdibase.h \
	gdiconfig.cpp \
	gdiconfig.h \
	geometry.h \
	HtmlExporter.cpp \
	JsonConverter.cpp \
	JsonConverter.h \
	JsonHandler.cpp \
	JsonHandler.h \
	JsonWorker.cpp \
	JsonWorker.h \
	jsonstubs.cpp \
	Lang.cpp \
	Lang.h \
	LanguageConfig.cpp \
	LanguageConfig.h \
	logger.cpp \
	logger.h \
	mathbase.cpp \
	mathbase.h \
	MDocument.cpp \
	MDocument.h \
	mstring.cpp \
	mstring.h \
	MultipleView.cpp \
	MultipleView.h \
	Planet.cpp \
	Planet.h \
	PrintoutConfig.cpp \
	PrintoutConfig.h \
	Sheet.cpp \
	Sheet.h \
	SymbolProvider.cpp \
	SymbolProvider.h \
	Table.cpp \
	Table.h \
	TableStyle.cpp \
	Viewpoint.cpp \
	Viewpoint.h \
	YdlExpression.cpp \
	YdlExpression.h \
	YdlLexer.cc \
	YdlLexer.ll \
	YdlParser.cc \
	YdlParser.yy \
	YogaConfig.cpp

all-am: 

clean:
	rm -f lib*.a *.o core *~ *.bak 
	rm -fr obj bin .libs

EXTRA_DIST = YdlParser.h* Makefile.mingw base.vcxproj


