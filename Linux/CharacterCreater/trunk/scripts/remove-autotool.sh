#!/bin/sh

if [ -f Makefile ]; then
	if [ -f test/Makefile ]; then
		make -C test distclean
	fi
	make distclean
fi

rm -rf autom4te.cache

find -name Makefile | xargs rm -f 
find -name Makefile.in | xargs rm -f
find -name .deps | xargs rm -rf
find -name '*.gmo' | xargs rm -f

rm -rf \
	aclocal.m4 \
	config.sub \
	config.guess \
	config.status.lineno \
	configure \
	install-sh \
	autom4te.cache \
	config.log \
	depcomp \
	libtool \
	config.status \
	ltmain.sh \
	missing \
	config.h.in \
	intltool-* \
	po/boldquot.sed \
	po/insert-header.sin \
	po/Makevars.template \
	po/remove-potcdate.sin \
	po/stamp-po \
	po/Makefile.in.in  \
	po/Rules-quot \
	po/Makevars \
	po/quot.sed           
