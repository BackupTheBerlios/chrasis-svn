lib_LTLIBRARIES = libchrasis.la

SUBDIRS = . @TOOLS_SUBDIRS@
DIST_SUBDIRS = . tools

libchrasis_la_SOURCES = \
	platform_layer/plat_win32.cpp \
	platform_layer/plat_posix.cpp \
	database.cpp \
	chrasisio.cpp \
	internal.cpp \
	character.cpp

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
