tooldir = $(top_builddir)/src/tools
data = chr_data.db empty_db.db

all: $(data)

chr_data.db:
	sqlite3 chr_data.db < database_schema.sql
	time $(tooldir)/learn `find ./chmls/ -type f -name \*.chml` chr_data.db
	sqlite3 chr_data.db "ANALYZE;VACUUM;"

empty_db.db:
	sqlite3 empty_db.db < database_schema.sql

install-data-local: $(data)
	$(mkinstalldirs) $(DESTDIR)/$(datadir)/chrasis
	$(INSTALL_DATA) $(data) $(DESTDIR)/$(datadir)/chrasis

CLEANFILES = $(data)
