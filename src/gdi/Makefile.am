
AM_CPPFLAGS = -I. -I../base -I../astro -I../wxpdfdoc $(WX_CPPFLAGS)

noinst_PROGRAMS = saravali9gdi
saravali9gdi_SOURCES = textclient.cpp

saravali9gdi_LDADD = libsaravali_gdi.a ../astro/libsaravali_astro.a ../base/libsaravali_base.a ../jsoncpp/libjsoncpp.a \
	../wxpdfdoc/libsaravali_pdf.a ../swe/libswe.a

saravali9gdi_LDADD += $(WX_LIBS) $(FONTCONFIG_LIBS)

noinst_LIBRARIES = libsaravali_gdi.a

libsaravali_gdi_a_SOURCES = \
	AshtakavargaDiagrams.cpp \
	AshtakavargaDiagrams.h \
	BarDiagram.cpp \
	BarDiagram.h \
	CeDasaLords.cpp \
	CeFieldGrid.cpp \
	CeFieldGrid.h \
	CeImage.cpp \
	CeLabelledAnnulus.cpp \
	CeLabelledAnnulus.h \
	CeNakshatras.cpp \
	CePlanetsBase.cpp \
	CePlanetsBase.h \
	ChartElement.cpp \
	ChartElement.h \
	ChartElementImpl.cpp \
	ChartElementImpl.h \
	ChartField.cpp \
	ChartField.h \
	DasaTree.cpp \
	DasaTree.h \
	DasaChart.cpp \
	DasaChart.h \
	DiamondChart.cpp \
	DocHelper.cpp \
	DocHelper.h \
	EastIndianChart.cpp \
	EphemDiagram.cpp \
	EphemDiagram.h \
	FieldWriter.cpp \
	FieldWriter.h \
	GcPainter.cpp \
	GdiHelper.cpp \
	GdiHelper.h \
	GdiWidget.cpp \
	GdiWidget.h \
	GdiWidgetGrid.cpp \
	GdiWidgetGrid.h \
	GraphicalDasa.h \
	HybridChart.cpp \
	HybridChart.h \
	ImageProvider.cpp \
	ImageProvider.h \
	MatchmapDiagram.cpp \
	MatchmapDiagram.h \
	Painter.cpp \
	Painter.h \
	PdfGenerator.cpp \
	PdfGenerator.h \
	PdfPainter.cpp \
	PpTree.cpp \
	PpTree.h \
	PrintoutTextHelper.cpp \
	PrintoutTextHelper.h \
	Sbc.cpp \
	ShadbalaDiagrams.cpp \
	ShadbalaDiagrams.h \
	SouthIndianChart.cpp \
	textclient.cpp \
	TimelineDiagram.cpp \
	TimelineDiagram.h \
	UranianDial.cpp \
	UranianAspects.cpp \
	UranianPlanets.cpp \
	VargaDiagrams.cpp \
	VargaDiagrams.h \
	WesternAngles.cpp \
	WesternAspects.cpp \
	WesternHouses.cpp \
	WesternPlanets.cpp \
	WesternSigns.cpp \
	WidgetResponse.h

all-am: 

clean:
	rm -f lib*.a *.o *core *~ *.bak *.pdf *.png *.jpg *.log $(noinst_PROGRAMS)
	rm -fr obj bin .libs logs *.dSYM

EXTRA_DIST = error.xpm Makefile.mingw ImageProvider_generated.cpp gdi.vcxproj gdicli/gdicli.vcxproj

