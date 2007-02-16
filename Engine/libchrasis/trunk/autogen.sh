#! /bin/sh

set -x

if [ "x${ACLOCAL_DIR}" != "x" ]; then
	ACLOCAL_ARG=-I ${ACLOCAL_DIR}
fi

${ACLOCAL:-aclocal} ${ACLOCAL_ARG}

libtoolize --copy --force
# intltoolize -c --automake
${AUTOHEADER:-autoheader}
${AUTOMAKE:-automake} --add-missing --copy --include-deps
${AUTOCONF:-autoconf}

if [ -d autom4te.cache ]; then
	rm -rf autom4te.cache
fi
