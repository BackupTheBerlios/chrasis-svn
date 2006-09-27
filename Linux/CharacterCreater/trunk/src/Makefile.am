## Makefile.am -- Process this file with automake to produce Makefile.in
bin_PROGRAMS = charactercreater

noinst_HEADERS = charactercreater.h character.h creater.h chmlcodec.h version.h

charactercreater_SOURCES = main.cpp character.cpp creater.cpp chmlcodec.cpp
charactercreater_LDADD = @GTKMM_LIBS@ @LIBXMLPP_LIBS@
charactercreater_CXXFLAGS = @GTKMM_CFLAGS@ @LIBXMLPP_CFLAGS@


