INCLUDES = @LIBXMLPP_CFLAGS@

noinst_PROGRAMS = learn recognize compact_db

learn_SOURCES = learn.cpp chmlcodec.cpp
learn_LDADD = \
	@LIBXMLPP_LIBS@ \
	$(top_builddir)/src/libchrasis.la

compact_db_SOURCES = compact_db.cpp
compact_db_LDADD = \
	@SQLITE3_LIBS@ \
	$(top_builddir)/src/libchrasis.la

recognize_SOURCES = recognize.cpp chmlcodec.cpp
recognize_LDADD = \
	@LIBXMLPP_LIBS@ \
	$(top_builddir)/src/libchrasis.la

