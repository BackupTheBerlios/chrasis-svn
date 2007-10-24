INCLUDES = @LIBXML_CFLAGS@

noinst_PROGRAMS = learn recognize join_chml probability_test char_size

join_chml_SOURCES = join_chml.cpp chmlcodec.cpp
join_chml_LDADD = @LIBXML_LIBS@ 

learn_SOURCES = learn.cpp chmlcodec.cpp
learn_LDADD = \
	@LIBXML_LIBS@ \
	$(top_builddir)/src/libchrasis.la

recognize_SOURCES = recognize.cpp chmlcodec.cpp
recognize_LDADD = \
	@LIBXML_LIBS@ \
	$(top_builddir)/src/libchrasis.la

probability_test_SOURCES = probability_test.cpp chmlcodec.cpp
probability_test_LDADD = \
	@LIBXML_LIBS@ \
	$(top_builddir)/src/libchrasis.la

char_size_SOURCES = char_size.cpp chmlcodec.cpp
char_size_LDADD = \
	@LIBXML_LIBS@ \
	$(top_builddir)/src/libchrasis.la


