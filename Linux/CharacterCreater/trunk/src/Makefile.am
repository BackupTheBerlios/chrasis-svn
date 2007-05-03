## Makefile.am -- Process this file with automake to produce Makefile.in
SUBDIRS = . tools

bin_PROGRAMS = charactercreater

noinst_HEADERS = charactercreater.h character.h creater.h chmlcodec.h database.h learner.h recognizer.h version.h
include_HEADERS = character.h chmlcodec.h database.h learner.h recognizer.h version.h

lib_LTLIBRARIES = libchrasis.la libchmlcodec.la libdatabase.la
libchmlcodec_la_SOURCES = chmlcodec.cpp
libchmlcodec_la_LIBADD = @LIBXMLPP_LIBS@
libchmlcodec_la_CXXFLAGS = @LIBXMLPP_CFLAGS@
libchrasis_la_SOURCES = recognizer.cpp learner.cpp
libchrasis_la_LIBADD = libdatabase.la
libdatabase_la_SOURCES = database.cpp
libdatabase_la_LIBADD = @SQLITE3_LIBS@
libdatabase_la_CXXFLAGS = @SQLITE3_CFLAGS@

charactercreater_SOURCES = main.cpp creater.cpp
charactercreater_LDADD = @GTKMM_LIBS@ libchmlcodec.la libchrasis.la
charactercreater_CXXFLAGS = @GTKMM_CFLAGS@

# tried to use precompiled headers, but failed.
#pch: common.h.gch

#BUILT_SOURCES = common.h.gch

#common.h.gch: common.h
#	$(mkdir_p) common.h.gch
#	$(CXXCOMPILE) $? -o $?.gch/normal.gch
#	$(CXXCOMPILE) $? -fPIC -DPIC -o $?.gch/pic.gch
