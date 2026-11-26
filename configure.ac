dnl
dnl  Saravali
dnl
dnl  File: configure.ac
dnl
dnl

AC_INIT([saravali9],[9.pre.460])
AM_INIT_AUTOMAKE()

dnl should be included according to documentation, but not supported by older versions of automake
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_RANLIB

AC_PROG_YACC
AM_PROG_LEX

AM_GNU_GETTEXT([external])

AC_CONFIG_HEADERS(config.h)

dnl AUTOMAKE_OPTIONS = dist-bzip2 dist-zip

dnl
dnl  Check for GUI toolkit wxWidgets
dnl
AM_OPTIONS_WXCONFIG

AM_PATH_WXCONFIG(3.2.0, wxWin=1)
if test "$wxWin" != 1; then
AC_MSG_ERROR([
wxWindows >= 3.2 must be installed on your system
but wx-config script could not be found.

Please check that wx-config is in path, the directory
where wxWindows libraries are installed (returned by
'wx-config --libs' command) is in LD_LIBRARY_PATH or
equivalent variable and wxWindows version is 3.0 or above.
])
fi

dnl
dnl Fontconfig is implicitly needed by wxpdfdoc for wxgtk
dnl
dnl so check if WXGTK is used and add fontconfig stuff then
dnl
WX_BASENAME=`$WX_CONFIG_WITH_ARGS --basename`
wxgtkset="`echo $WX_BASENAME | grep gtk`"

if test "x$wxgtkset" != x; then
  PKG_CHECK_MODULES([FONTCONFIG], [fontconfig >= 2.0])
fi

AC_CONFIG_FILES([Makefile po/Makefile.in
  debian/Makefile
  src/Makefile
  src/jsoncpp/Makefile
  src/wxpdfdoc/Makefile
  src/wxsqlite3/Makefile
  src/base/Makefile
  src/swe/Makefile
  src/astro/Makefile
  src/gdi/Makefile
  src/widgets/Makefile
  src/dialogs/Makefile
  src/gui/Makefile
  src/resources/Makefile
  src/resources/atlas/Makefile
  src/resources/chart/Makefile
  src/resources/dasa/Makefile
  src/resources/font/Makefile
  src/resources/pic/Makefile
  src/resources/pic/16x16/Makefile
  src/resources/pic/24x24/Makefile
  src/resources/pic/32x32/Makefile
  src/resources/print/Makefile
  src/resources/skin/Makefile
  src/resources/table/Makefile
  src/resources/yoga/Makefile
])
AC_OUTPUT

echo
echo "------------------------------------------------------"
echo
echo "  Configuration successful!"
echo
echo "General options"
echo "   Software version :   $PACKAGE_VERSION"
echo "   Prefix           :   $prefix"
echo "   C Compiler       :   $CC"
echo "   C++ Compiler     :   $CXX"
echo
echo "wxWidgets options"
echo "   wx version       :   $WX_VERSION"
echo "   wx-config        :   $WX_CONFIG_WITH_ARGS"
echo
echo "You can type 'make' now"
echo
echo "------------------------------------------------------"
echo

