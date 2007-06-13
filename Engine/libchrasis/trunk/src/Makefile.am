lib_LTLIBRARIES = libchrasis.la

SUBDIRS = . @TOOLS_SUBDIRS@
DIST_SUBDIRS = . tools

libchrasis_la_SOURCES = \
	database.cpp \
	chrasisio.cpp \
	internal.cpp

libchrasis_la_LIBADD = \
	@SQLITE3_LIBS@

libchrasis_la_CXXFLAGS = \
	@SQLITE3_CFLAGS@ \
	-DCHRASIS_DATADIR=\"$(datadir)/chrasis\"

libchrasis_la_LDFLAGS = \
	-version-info $(LIBCHRASIS_CURRENT):$(LIBCHRASIS_REVISION):$(LIBCHRASIS_AGE) \
	-export-dynamic \
	$(LIBTOOL_EXPORT_OPTIONS) \
	-rpath $(libdir)
