AUX_DIST = \
	$(ac_aux_dir)/config.guess \
	$(ac_aux_dir)/config.sub \
	$(ac_aux_dir)/compile \
	$(ac_aux_dir)/depcomp \
	$(ac_aux_dir)/install-sh \
	$(ac_aux_dir)/ltconfig \
	$(ac_aux_dir)/ltmain.sh \
	$(ac_aux_dir)/missing
EXTRA_DIST = \
	config.rpath \
	mkinstalldirs \
	autogen.sh \
	chrasis.pc \
	Doxyfile

AUTOMAKE_OPTIONS = gnu

SUBDIRS = src @DATA_SUBDIRS@
DIST_SUBDIRS = src data

MAINTAINERCLEANFILES = \
	Makefile.in \
	aclocal.m4 \
	configure \
	config.h.in \
	$(AUX_DIST)

DISTCLEANFILES = chrasis.pc

CLEANFILES = *.bak

pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = chrasis.pc

chrasis_headers_hidden = \
	include/chrasis/internal.h \
	include/chrasis/character.h \
	include/chrasis/database.h
chrasis_headers = \
	include/chrasis.h

install-data-local:
	$(mkinstalldirs) $(DESTDIR)/$(prefix)/include/chrasis/chrasis
	$(INSTALL_DATA) $(chrasis_headers) $(DESTDIR)/$(prefix)/include/chrasis
	$(INSTALL_DATA) $(chrasis_headers_hidden) $(DESTDIR)/$(prefix)/include/chrasis/chrasis

docs:
	if [ -x "@DOXYGEN@" ]; then \
		@DOXYGEN@ Doxyfile; \
	fi
