#!/bin/bash

rm -rf result.db organized
mkdir -p processed
mkdir -p organized

# learn each chml
for I in `find . -maxdepth 1 -type f -name \*.chml` `find ./processed -type f -name \*.chml`
do
	../learn "${I}" result.db
done
sqlite3 result.db 'ANALYZE;VACUUM;'

../chml_organizer . ./organized/

for I in `find . -maxdepth 1 -type f -name \*.chml`
do
	fn=processed/${I}
	while [ -e ${fn} ]
	do
		fn=processed/${I/.chml/_$(uuidgen).chml}
	done
	mv -i ${I} $fn
done
