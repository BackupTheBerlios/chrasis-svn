INCLUDES = @LIBXMLPP_CFLAGS@

noinst_PROGRAMS = learn recognize join_chml

join_chml_SOURCES = join_chml.cpp chmlcodec.cpp
join_chml_LDADD = @LIBXMLPP_LIBS@ 

learn_SOURCES = learn.cpp chmlcodec.cpp
learn_LDADD = \
	@LIBXMLPP_LIBS@ \
	$(top_builddir)/src/libchrasis.la

recognize_SOURCES = recognize.cpp chmlcodec.cpp
recognize_LDADD = \
	@LIBXMLPP_LIBS@ \
	$(top_builddir)/src/libchrasis.la

