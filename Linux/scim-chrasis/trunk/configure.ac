dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)

AC_INIT([scim-chrasis], [0.0.1], [palatis@gmail.com])
AC_CONFIG_SRCDIR([src/scim-chrasis.cpp])

AC_SUBST(PACKAGE_VERSION)
AC_DEFINE_UNQUOTED(SCIM_CHRASIS_VERSION, "$PACKAGE_VERSION", [The release version of Chrasis Trainer.])

SCIM_VERSION=1.2.0
GTK_VERSION=2.4.0

AC_SUBST(SCIM_VERSION)
AC_SUBST(GTK_VERSION)

# libtool versioning for SCIM-Chrasis

# increment if the interface has additions, changes, removals.
SCIM_CHRASIS_CURRENT=0

# increment any time the source changes; set to 0 if you increment CURRENT
SCIM_CHRASIS_REVISION=0

# increment if any interfaces have been added; set to 0
# if any interfaces have been removed. removal has 
# precedence over adding, so set to 0 if both happened.
SCIM_CHRASIS_AGE=0

AC_SUBST(SCIM_CHRASIS_CURRENT)
AC_SUBST(SCIM_CHRASIS_REVISION)
AC_SUBST(SCIM_CHRASIS_AGE)

GETTEXT_PACKAGE=scim-chrasis
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Gettext package])

# Init automake stuff
AM_INIT_AUTOMAKE([1.9])
AM_CONFIG_HEADER([config.h])

# Init gettext
ALL_LINGUAS="zh_TW"
AM_GNU_GETTEXT

# Standard Checks
IT_PROG_INTLTOOL([0.33], [no-xml])
AC_HEADER_STDC
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL
AC_LIB_LTDL
AC_SUBST(LIBTOOL_DEPS)
AC_LANG([C++])

AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Check for modules
PKG_CHECK_MODULES(CHRASIS, chrasis)
AC_SUBST(CHRASIS_CFLAGS)
AC_SUBST(CHRASIS_LIBS)

PKG_CHECK_MODULES(SCIM, [scim >= $SCIM_VERSION])
AC_SUBST(SCIM_CFLAGS)
AC_SUBST(SCIM_LIBS)

PKG_CHECK_MODULES(GTK2, [gtk+-2.0 >= $GTK_VERSION])
AC_SUBST(GTK2_CFLAGS)
AC_SUBST(GTK2_LIBS)

# libtool option to control which symbols are exported
# right now, symbols starting with _ are not exported
LIBTOOL_EXPORT_OPTIONS='-export-symbols-regex "^[[^_]].*"'
AC_SUBST(LIBTOOL_EXPORT_OPTIONS)

# Extra args.
AC_ARG_ENABLE(
	debug,
	[  --enable-debug            Turn on debugging],
	enable_debug=yes,
	enable_debug=no)

if test "$enable_debug" = "yes"; then
	AC_DEFINE(ENABLE_DEBUG, 1, [Define this to enable the debug facility in libscim])
	CFLAGS="$CFLAGS -g"
	CXXFLAGS="$CXXFLAGS -g"
fi

SCIM_ICONDIR=`$PKG_CONFIG --variable=icondir scim`
SCIM_MODULEDIR=`$PKG_CONFIG --variable=moduledir scim`
SCIM_DATADIR=`$PKG_CONFIG --variable=scimdatadir scim`
SCIM_LIBEXECDIR=`$PKG_CONFIG --variable=moduledir scim`

if test "x$SCIM_ICONDIR" = "x"; then
	SCIM_ICONDIR=${datadir}/scim/icons
fi

if test "x$SCIM_MODULEDIR" = "x"; then
	SCIM_MODULEDIR=${libdir}/scim-1.0
fi

if test "x$SCIM_DATADIR" = "x"; then
	SCIM_DATADIR=${datadir}/scim
fi

if test "x$SCIM_LIBEXECDIR" = "x"; then
	SCIM_LIBEXECDIR=${libdir}/scim-1.0
fi

AC_SUBST(SCIM_ICONDIR)
AC_SUBST(SCIM_MODULEDIR)
AC_SUBST(SCIM_DATADIR)
AC_SUBST(SCIM_LIBEXECDIR)

# Check for programs
AM_MAINTAINER_MODE

AC_CONFIG_FILES(
	[Makefile]
	[po/Makefile.in]
	[src/Makefile])
AC_OUTPUT
