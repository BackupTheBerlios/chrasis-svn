#! /bin/sh

case `uname` in
Darwin*)
	echo "***"
	echo "*** please go to MacOSX subdirectory and make there"
	echo "***"
	exit 1
	;;
esac

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
