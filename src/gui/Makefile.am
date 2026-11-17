
bin_PROGRAMS = saravali9

AM_CPPFLAGS = $(WX_CPPFLAGS) -I. -I../base -I../astro -I../widgets -I../gdi -I../pics
AM_CPPFLAGS += -I../dialogs -DSTRICT
AM_CPPFLAGS += $(PLATFORM_OPTS)

saravali9_LDADD = ../widgets/libsaravali_widgets.a ../gdi/libsaravali_gdi.a \
	../astro/libsaravali_astro.a ../dialogs/libsaravali_dialogs.a ../base/libsaravali_base.a \
	../jsoncpp/libjsoncpp.a ../wxpdfdoc/libsaravali_pdf.a ../wxsqlite3/libsaravali_db.a \
	../swe/libswe.a

saravali9_LDADD += $(WX_LIBS) $(FONTCONFIG_LIBS)

saravali9_SOURCES = \
	ApplicationWindow.cpp \
	ApplicationWindow.h \
	ChildWindow.cpp \
	ChildWindow.h \
	GuiApplication.cpp \
	guibase.cpp \
	guibase.h \
	MenuProvider.cpp \
	MenuProvider.h \
	PdfLauncher.cpp \
	PdfLauncher.h \
	Statusbar.cpp \
	Statusbar.h \
	ToolPanel.cpp \
	ToolPanel.h \
	WindowDirectory.cpp \
	WindowDirectory.h \
	WindowFactory.cpp \
	WindowFactory.h \
	WindowListCtrl.cpp \
	WindowListCtrl.h \
	YogaEditorWindow.cpp

all-am: saravali9

# create links to translations (only for local usage)
intllinks:
	for l in `cat ../../po/LINGUAS`; \
	do \
	echo "Creating link for language $$l"; \
	test -d $$l || mkdir $$l; \
	test -L $$l/saravali9.mo || ln -fs ../../../po/$$l.gmo $$l/saravali9.mo; \
	done

removeintllinks:
	for l in `cat ../../po/LINGUAS`; \
	do \
	echo "Removinging link for language $$l"; \
	if [ -d "$l" ]; then rm -fr "$l"; fi \
	done

EXTRA_DIST = Makefile.mingw saravali9.rc gui.vcxproj

clean: removeintllinks
	rm -f *.o *~ *.bak *core $(bin_PROGRAMS)
	rm -fr obj bin .libs


