# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)

AC_INIT(libchrasis, 1.0.0, chrasis-devel@googlegroups.com)
AC_CONFIG_SRCDIR([src/chrasisio.cpp])

AC_SUBST(PACKAGE_VERSION)

# increment if the interface has additions, changes, removals.
LIBCHRASIS_CURRENT=1

# increment any time the source changes; set to 0 if you increment CURRENT
LIBCHRASIS_REVISION=0

# increment if any interfaces have been added; set to 0
# if any interfaces have been removed. removal has 
# precedence over adding, so set to 0 if both happened.
LIBCHRASIS_AGE=0

AC_SUBST(LIBCHRASIS_CURRENT)
AC_SUBST(LIBCHRASIS_REVISION)
AC_SUBST(LIBCHRASIS_AGE)

# Define a string for the earliest version that this release has
# binary compatibility with. This is used for module locations.
#
LIBCHRASIS_BINARY_VERSION=1.0.0
AC_SUBST(LIBCHRASIS_BINARY_VERSION)

AC_DEFINE_UNQUOTED(
	LIBCHRASIS_BINARY_VERSION,
	"$LIBCHRASIS_BINARY_VERSION",
	[The binary version of libchrasis.])
AC_DEFINE_UNQUOTED(
	LIBCHRASIS_VERSION,
	"$PACKAGE_VERSION",
	[The release version of libchrasis.])

# Check for modules
LIBXMLPP_VERSION=2.10
SQLITE3_VERSION=3.3

PKG_CHECK_MODULES(LIBXMLPP, libxml++-2.6 >= $LIBXMLPP_VERSION)
AC_SUBST(LIBXMLPP_CFLAGS)
AC_SUBST(LIBXMLPP_LIBS)

PKG_CHECK_MODULES(SQLITE3, sqlite3 >= $SQLITE3_VERSION)
AC_SUBST(SQLITE3_CFLAGS)
AC_SUBST(SQLITE3_LIBS)

# Init automake stuff
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([include/config.h])
#AC_PREFIX_DEFAULT(/usr)

# Init libtool
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)

# libtool option to control which symbols are exported
# right now, symbols starting with _ are not exported
LIBTOOL_EXPORT_OPTIONS='-export-symbols-regex "^[[^_]].*"'
AC_SUBST(LIBTOOL_EXPORT_OPTIONS)

hecks if doxygen is avaiable
AC_PATH_PROG(DOXYGEN, doxygen, no)
AM_CONDITIONAL(HAVE_DOXYGEN, test x$DOXYGEN != xno)
AC_SUBST(DOXYGEN)

# Checks if graphviz is available
AC_CHECK_PROG(HAVE_GRAPHVIZ, dot, YES, NO)
AC_PATH_PROG(DOT, dot)
AC_SUBST(DOT)
AC_PATH_PROG(PERL, perl)
AC_SUBST(PERL)

# Checks for libraries.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Options
AC_ARG_ENABLE([debug],
	[AS_HELP_STRING(--enable-debug,Turn on debugging support)],
	[case "${enableval}" in
	yes)
		LIBDEBUG="true"
		AC_DEFINE(
			ENABLE_DEBUG, 1,
			[Define to 1 if you want native library runtime debugging code enabled])
		CFLAGS="$CFLAGS -g"
		;;
	no)
		LIBDEBUG="false"
		;;
	*)
		AC_MSG_ERROR(bad value ${enableval} for --enable-debug)
		;;
	esac],
	[LIBDEBUG="false"])
AC_SUBST(LIBDEBUG)

# Platform-dependent
dnl What kind of system are we using?
case $host_os in
win32*)
	SYSTEM=win32
	;;
cygwin*)
	SYSTEM=unix
	;;
*)
	SYSTEM=unix
	;;
esac

case $SYSTEM in
win32)
	AC_DEFINE(
		UNDER_WIN32, 1,
		[Runtime is under Win32 environment])
	;;
unix)
	AC_DEFINE(
		UNDER_POSIX, 1,
		[Runtime is under POSIX environment])
	;;
esac

AC_OUTPUT([
	Makefile 
	data/Makefile 
	src/Makefile
	src/tools/Makefile 
	chrasis.pc 
	Doxyfile
])

AC_MSG_RESULT([
Build options:
  Version                 $PACKAGE_VERSION
  Install prefix          $prefix
  Enable debug            $LIBDEBUG
])
