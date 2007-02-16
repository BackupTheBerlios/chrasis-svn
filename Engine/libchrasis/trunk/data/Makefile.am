tooldir = $(top_builddir)/src/tools
data = chr_data.db

all: $(data)

chr_data.db:
	for I in `find ./chmls/ -type f -name \*.chml`;do \
		$(tooldir)/learn $${I} chr_data.db; \
	done
	$(tooldir)/compact_db chr_data.db

install-data-local: $(data)
	$(mkinstalldirs) $(DESTDIR)/$(datadir)/chrasis
	$(INSTALL_DATA) $(data) $(DESTDIR)/$(datadir)/chrasis

CLEANFILES = $(data)
