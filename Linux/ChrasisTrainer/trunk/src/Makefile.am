## Makefile.am -- Process this file with automake to produce Makefile.in
bin_PROGRAMS = chrasistrainer

noinst_HEADERS = trainer.h chrasistrainer.h chmlcodec.h
include_HEADERS = trainer.h

chrasistrainer_SOURCES = main.cpp trainer.cpp chmlcodec.cpp
chrasistrainer_LDADD = @GTKMM_LIBS@ @CHRASIS_LIBS@ @LIBXML_LIBS@
chrasistrainer_CXXFLAGS = @GTKMM_CFLAGS@ @CHRASIS_CFLAGS@ @LIBXML_CFLAGS@

# tried to use precompiled headers, but failed.
#pch: common.h.gch

#BUILT_SOURCES = common.h.gch

#common.h.gch: common.h
#	$(mkdir_p) common.h.gch
#	$(CXXCOMPILE) $? -o $?.gch/normal.gch
#	$(CXXCOMPILE) $? -fPIC -DPIC -o $?.gch/pic.gch
