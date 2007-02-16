lib_LTLIBRARIES = libchrasis.la

libchrasis_la_SOURCES = \
	chmlcodec.cpp \
	database.cpp \
	learner.cpp \
	recognizer.cpp \
	chrasisio.cpp

libchrasis_la_LIBADD = \
	@LIBXMLPP_LIBS@ \
	@SQLITE3_LIBS@

libchrasis_la_CXXFLAGS = \
	@LIBXMLPP_CFLAGS@ \
	@SQLITE3_CFLAGS@ \
	-DCHRASIS_DATADIR=\"$(datadir)/chrasis\"

libchrasis_la_LDFLAGS = \
	-version-info $(LIBCHRASIS_CURRENT):$(LIBCHRASIS_REVISION):$(LIBCHRASIS_AGE) \
	-export-dynamic \
	$(LIBTOOL_EXPORT_OPTIONS) \
	-rpath $(libdir)
