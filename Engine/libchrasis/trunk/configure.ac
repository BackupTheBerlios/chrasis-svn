# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)

AC_INIT([libchrasis],[0.1.2],[chrasis-devel@googlegroups.com])
AC_CONFIG_SRCDIR([src/chrasisio.cpp])
AC_CANONICAL_TARGET

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
LIBCHRASIS_BINARY_VERSION=0.1.1
AC_SUBST(LIBCHRASIS_BINARY_VERSION)

AC_DEFINE_UNQUOTED(LIBCHRASIS_BINARY_VERSION, "$LIBCHRASIS_BINARY_VERSION",
	[The binary version of libchrasis.])
AC_DEFINE_UNQUOTED(LIBCHRASIS_VERSION, "$PACKAGE_VERSION",
	[The release version of libchrasis.])

# Init automake stuff
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([include/config.h])
#AC_PREFIX_DEFAULT(/usr)

# Init libtool
#AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)

# libtool option to control which symbols are exported
# right now, symbols starting with _ are not exported
LIBTOOL_EXPORT_OPTIONS='-export-symbols-regex "^[[^_]].*"'
AC_SUBST(LIBTOOL_EXPORT_OPTIONS)

# checks if doxygen is avaiable
AC_PATH_PROG(DOXYGEN, doxygen, no)
AM_CONDITIONAL(HAVE_DOXYGEN, test x$DOXYGEN != xno)
AC_SUBST(DOXYGEN)

# Checks if graphviz is available
#AC_CHECK_PROG(HAVE_GRAPHVIZ, dot, YES, NO)
#AC_PATH_PROG(DOT, dot)
#AC_SUBST(DOT)
#AC_PATH_PROG(PERL, perl)
#AC_SUBST(PERL)

# Checks for libraries.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Options
AC_ARG_ENABLE([debug],
	[AS_HELP_STRING(--enable-debug,Turn on debugging support)],
	ac_arg_debug=${enableval}, ac_arg_debug="no")
AC_ARG_ENABLE([tools],
	[AS_HELP_STRING(--disable-tools,Build the character database mantaince tools)],
	ac_arg_tools=${enableval}, ac_arg_tools="yes")

# Platform-dependent
dnl What kind of system are we using?
os_is_posix=0
os_is_unknown=0

case "$target" in
*-linux*)
	os_is_posix=1
	os_is_unknown=0
	;;
*)
	os_is_posix=1
	;;
esac

AC_DEFINE_UNQUOTED(UNDER_POSIX, ${os_is_posix}, [Runtime is under POSIX environment])

case "${ac_arg_debug}" in
yes)
	LIBDEBUG="true"
	AC_DEFINE(ENABLE_DEBUG, 1,
		[Define to 1 if you want native library runtime debugging code enabled])
	CFLAGS="$CFLAGS -g"
	;;
*)
	LIBDEBUG="false"
	;;
esac
AC_SUBST(LIBDEBUG)

case "${ac_arg_tools}" in
yes)
	BUILD_TOOLS="true"
	AC_DEFINE(ENABLE_TOOLS, 1,
		[Define to 1 if you want to build the character database maintaince tools])
	PKG_CHECK_MODULES(LIBXML, [libxml-2.0])
	TOOLS_SUBDIRS=tools
	DATA_SUBDIRS=data
	;;
*)
	BUILD_TOOLS="false"
	TOOLS_SUBDIRS=""
	DATA_SUBDIRS=""
	;;
esac
AC_SUBST(LIBXML_CFLAGS)
AC_SUBST(LIBXML_LIBS)
AC_SUBST(BUILD_TOOLS)
AC_SUBST(TOOLS_SUBDIRS)
AC_SUBST(DATA_SUBDIRS)

PKG_CHECK_MODULES(SQLITE3, [sqlite3])
AC_SUBST(SQLITE3_CFLAGS)
AC_SUBST(SQLITE3_LIBS)

AC_CONFIG_FILES([
	Makefile
	data/Makefile
	src/Makefile
	src/tools/Makefile
	chrasis.pc
	Doxyfile
])

AC_OUTPUT

AC_MSG_RESULT([
Build options:
  Version                 $PACKAGE_VERSION
  Install prefix          $prefix
  Build tools             $BUILD_TOOLS
  Enable debug            $LIBDEBUG

  Building for            $host_os
])
if test ${os_is_unknown} = 1; then
AC_MSG_RESULT([

  *** Your host is unknown, build result may vary. ***

])
fi
