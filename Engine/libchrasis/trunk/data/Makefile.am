tooldir = $(top_builddir)/src/tools
data = chr_data.db db_schema.sql

all: $(data)

chr_data.db:
	sqlite3 chr_data.db < database_schema.sql
	time for I in `find ./chmls/ -type f -name \*.chml`;do \
		$(tooldir)/learn $${I} chr_data.db; \
	done
	echo "ANALYZE;VACUUM;" | sqlite3 chr_data.db

db_schema.sql:
	cp database_schema.sql db_schema.sql

install-data-local: $(data)
	$(mkinstalldirs) $(DESTDIR)/$(datadir)/chrasis
	$(INSTALL_DATA) $(data) $(DESTDIR)/$(datadir)/chrasis

CLEANFILES = $(data)
