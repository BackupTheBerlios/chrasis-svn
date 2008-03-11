dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)

AC_INIT([scim-chrasis], [0.0.1], [palatis@gmail.com])
AC_CONFIG_SRCDIR([src/scim-chrasis.h])

AC_SUBST(PACKAGE_VERSION)

AC_DEFINE_UNQUOTED(SCIM_CHRASIS_VERSION, "$PACKAGE_VERSION", [The release version of Chrasis Trainer.])

# Init automake stuff
AM_INIT_AUTOMAKE([1.9])
AM_CONFIG_HEADER([src/config.h])

# Init gettext
ALL_LINGUAS="zh_TW"
AM_GLIB_GNU_GETTEXT()
AM_GLIB_DEFINE_LOCALEDIR(SCIM_CHRASIS_LOCALEDIR)

GETTEXT_PACKAGE=scim-chrasis
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Gettext package])

# Standard Checks
AC_HEADER_STDC
AC_PROG_CXX
AC_PROG_LIBTOOL
IT_PROG_INTLTOOL
AC_LANG([C++])

AC_C_CONST
AC_TYPE_SIZE_T

# Check for modules
PKG_CHECK_MODULES(CHRASIS, chrasis)
AC_SUBST(CHRASIS_CFLAGS)
AC_SUBST(CHRASIS_LIBS)

PKG_CHECK_MODULES(SCIM, scim)
AC_SUBST(SCIM_CFLAGS)
AC_SUBST(SCIM_LIBS)

# Check for programs
AM_MAINTAINER_MODE

AC_CONFIG_FILES(
	[Makefile]
	[po/Makefile.in]
	[src/Makefile])
AC_OUTPUT
