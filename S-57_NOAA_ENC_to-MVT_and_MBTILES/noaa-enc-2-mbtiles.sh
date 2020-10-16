#!/bin/bash

export OGR_S57_OPTIONS="RETURN_PRIMITIVES=ON,RETURN_LINKAGES=ON,LNAM_REFS=ON,SPLIT_MULTIPOINT=ON,ADD_SOUNDG_DEPTH=ON"

NOAA_FILE=NJ_ENCs.zip

mkdir noaa_enc/
cd  noaa_enc/ || exit 255
rm -rf ${NOAA_FILE} || true
wget https://www.charts.noaa.gov/ENCs/${NOAA_FILE}
unzip ${NOAA_FILE}
rm -rf ${NOAA_FILE} || true
cd ..

# these are needed to keep ogr2ogr from crashing
wget https://raw.githubusercontent.com/OpenCPN/OpenCPN/master/data/s57data/s57objectclasses.csv
wget https://raw.githubusercontent.com/OpenCPN/OpenCPN/master/data/s57data/s57attributes.csv

rm -rf *-mvt *.temp.db || true
find . -name "US*.000" -type f | while read -r in; do  ogr2ogr -append -skipfailures -f MVT -dsco FORMAT=DIRECTORY -dsco MAXZOOM=5 `basename $in .000`-mvt $in; done
























# To MapBox MVT directory
find . -name "US*.000" -exec ogr2ogr -append -skipfailures -f MVT -dsco FORMAT=DIRECTORY -dsco MAXZOOM=5 {}  \;

# To MapBox .mbtiles
find . -name "US*.000" -exec ogr2ogr -append -skipfailures -f MVT -dsco FORMAT=DIRECTORY -dsco MAXZOOM=5 `echo $(basename {})`.mbtiles {}  \;

ogr2ogr -append -skipfailures -f MVT -dsco FORMAT=DIRECTORY -dsco MAXZOOM=5


 -f MVT -dsco FORMAT=MBTILES -dsco MAXZOOM=5 -splitlistfields ./mvt4.mbtiles ./datatest/US5OH10M/US5OH10M.000




find . -name "*.000" -type f |
while read -r in;
do
  ogr2ogr -append -skipfailures -f MVT -dsco FORMAT=DIRECTORY -dsco MAXZOOM=5 `basename $in .000` $in
done


find . -name "*.000" -type f |
while read -r f
do
    echo "f"
done



ogr2ogr -skipfailures -f MVT -dsco FORMAT=DIRECTORY -dsco MAXZOOM=5 ./mvt66 ./datatest/US5OH10M/US5OH10M.000


ogr2ogr -append -skipfailures -f MVT -dsco FORMAT=DIRECTORY -dsco MAXZOOM=5 ./mvt12 ./datatest/US5OH10M/US5OH10M.000





# Displaying projection CRS EPSG:3857
# WSG 84 Pseudo-Mercator



