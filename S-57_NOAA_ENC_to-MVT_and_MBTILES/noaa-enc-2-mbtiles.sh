#!/bin/bash

# IMPORTANT!!!
# these are needed for ogr2ogr s-57 plugin
export OGR_S57_OPTIONS="RETURN_PRIMITIVES=ON,RETURN_LINKAGES=ON,LNAM_REFS=ON,SPLIT_MULTIPOINT=ON,ADD_SOUNDG_DEPTH=ON,RECODE_BY_DSSI=ON"

MAXZOOM=5

NOAA_FILE=NJ_ENCs.zip

mkdir noaa_enc/
cd  noaa_enc/ || exit 255
rm -rf *.zip || true
wget https://www.charts.noaa.gov/ENCs/${NOAA_FILE}
unzip ${NOAA_FILE}
rm -rf ${NOAA_FILE} || true
cd ..

# IMPORTANT!!!
# these are needed to keep ogr2ogr from crashing
wget https://raw.githubusercontent.com/OpenCPN/OpenCPN/master/data/s57data/s57objectclasses.csv
wget https://raw.githubusercontent.com/OpenCPN/OpenCPN/master/data/s57data/s57attributes.csv

rm -rf *-mvt *.temp.db || true
find . -name "US*.000" -type f | while read -r in
do
    ogr2ogr -append -skipfailures -f MVT -dsco FORMAT=DIRECTORY -dsco MAXZOOM=${MAXZOOM} -dialect SQLITE \
     `basename $in .000`-mvt $in ;
done

# Displaying projection CRS EPSG:3857
# WSG 84 Pseudo-Mercator


