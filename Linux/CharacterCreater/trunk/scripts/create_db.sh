#!/bin/bash

rm -f result.db
mkdir -p good_chml
for I in `find . -maxdepth 1 -type f -name \*.chml` `find ./good_chml -type f -name \*.chml`
do
	../learn "${I}" result.db
done
sqlite3 result.db 'ANALYZE;VACUUM;'

for I in `find . -maxdepth 1 -type f -name \*.chml`
do
	fn=good_chml/${I}
	while [ -e ${fn} ]
	do
		fn=good_chml/${I/.chml/_$(uuidgen).chml}
	done
	mv -i ${I} $fn
done
