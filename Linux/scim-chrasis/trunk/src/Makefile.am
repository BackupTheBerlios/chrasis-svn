MAINTRAINERCLEANFILES	= Makefile.in

INCLUDES		= -I$(top_builddir) \
			  -I$(top_srcdir) \
			  -I$(top_srcdir)/intl \
			  -I$(top_srcdir)/src \
			  -DSCIM_ICONDIR=\"@SCIM_ICONDIR@\" \
			  -DSCIM_CHRASIS_LOCALEDIR=\"$(datadir)/locale\"

moduledir		= @SCIM_MODULEDIR@/Helper
module_LTLIBRARIES	= chrasis.la

chrasis_la_SOURCES	= scim-chrasis.cpp
chrasis_la_CXXFLAGS	= @GTK2_CFLAGS@ @SCIM_CFLAGS@ @CHRASIS_CFLAGS@
chrasis_la_LDFLAGS	= -avoid-version \
			  -rpath $(moduledir) \
			  -module \
			  @LIBTOOL_EXPORT_OPTIONS@ \
			  @LTLIBINTL@ \
			  @CHRASIS_LIBS@ \
			  @SCIM_LIBS@ \
			  @GTK2_LIBS@
