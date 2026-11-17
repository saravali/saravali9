
AM_CPPFLAGS = $(WX_CPPFLAGS) -I. -I../gui -I../base -I../astro -I../dialogs -I ../gdi
# AM_CPPFLAGS += -Wno-write-strings -Wno-unused-variable -Wno-ignored-qualifiers -Wno-cast-function-type
AM_CPPFLAGS += $(PLATFORM_OPTS)

noinst_LIBRARIES = libsaravali_widgets.a

libsaravali_widgets_a_SOURCES = \
	AshtakavargaWidget.cpp \
	BasicWidget.cpp \
	BasicWidget.h \
	ChartWidget.cpp \
	CombiWidget.cpp \
	CombiWidget.h \
	ComprehensiveTextWidget.cpp \
	DasaChartWidget.cpp \
	DasaTreeWidget.cpp \
	DirectionWidget.cpp \
	EclipseWidget.cpp \
	EphemWidget.cpp \
	EventFinderWidget.cpp \
	GdiWidgetWrapper.cpp \
	GdiWidgetWrapper.h \
	GenericTextWidget.cpp \
	GenericTextWidget.h \
	GridWrapper.cpp \
	GridWrapper.h \
	HoraWidget.cpp \
	MatchesWidget.cpp \
	MixedWidgetGrid.cpp \
	MultiDirectionWidget.cpp \
	NotebookWidget.cpp \
	NotebookWidget.h \
	PartnerWidget.cpp \
	PpWidget.cpp \
	ShadbalaWidget.cpp \
	SolarWidget.cpp \
	SpWidget.cpp \
	TextWidget.cpp \
	TextWidget.h \
	TextWrapperWidgets.cpp \
	UranianChartWidget.cpp \
	VargaGridWidget.cpp \
	VimsopakabalaWidget.cpp \
	WidgetFactory.cpp \
	WidgetFactory.h \
	YogaEditorWidget.cpp \
	YogaEditorWidget.h \
	YogaWidget.cpp 

all-am: 

clean:
	rm -f lib*.a *.o core *~ *.bak
	rm -fr obj bin .libs

EXTRA_DIST = Makefile.mingw widgets.vcxproj

