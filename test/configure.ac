dnl
dnl  Maitreya
dnl
dnl  File: configure.ac
dnl
dnl

AC_INIT([saravali9test],[9.pre.2])
AM_INIT_AUTOMAKE()

dnl should be included according to documentation, but not supported by older versions of automake
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_RANLIB

PKG_CHECK_MODULES([GTEST], [gtest],[],[AC_MSG_ERROR([Google (gtest) test not found, exit])] )

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

AC_CONFIG_FILES([Makefile
	src/Makefile
	resources/Makefile
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
echo "Additional library options"
echo "   gtest cflags     : ${GTEST_CFLAGS} "
echo "   gtest libs       : ${GTEST_LIBS} "
echo
echo "You can type 'make' now"
echo
echo "------------------------------------------------------"
echo

