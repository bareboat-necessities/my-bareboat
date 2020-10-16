#!/bin/bash

# https://gdal.org/drivers/vector/s57.html
# https://gdal.org/drivers/vector/mvt.html
# https://github.com/alukach/s57-vector-tile-server
# https://github.com/LarsSchy/SMAC-M

# IMPORTANT!!!
# these are needed for ogr2ogr s-57 plugin
export OGR_S57_OPTIONS="RETURN_PRIMITIVES=ON,RETURN_LINKAGES=ON,LNAM_REFS=ON,SPLIT_MULTIPOINT=ON,ADD_SOUNDG_DEPTH=ON,LIST_AS_STRING=ON,RECODE_BY_DSSI=ON"
export S57_PROFILE=iw

#export OGR_S57_OPTIONS="RETURN_PRIMITIVES=ON,RETURN_LINKAGES=ON,LNAM_REFS=ON,SPLIT_MULTIPOINT=ON,ADD_SOUNDG_DEPTH=ON"
#export S57_PROFILE=

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
wget https://raw.githubusercontent.com/OpenCPN/OpenCPN/master/data/s57data/s57attributes.csv
wget https://raw.githubusercontent.com/OpenCPN/OpenCPN/master/data/s57data/s57objectclasses.csv

cp s57attributes.csv s57attributes_iw.csv
cp s57objectclasses.csv s57objectclasses_iw.csv

rm -rf *-mvt *.temp.db || true
find . -name "*.000" -type f | while read -r in
do
   echo "processing $in"
   set -x

#   ogr2ogr -append -skipfailures -update \
#      -f MVT -dsco FORMAT=DIRECTORY -dsco MAXZOOM=${MAXZOOM} -dialect SQLITE \
#      -nlt POINT25D \
#      -fieldTypeToString StringList,IntegerList \
#      `basename $in .000`-mvt $in

   ogr2ogr -append -skipfailures -update \
      -f MVT -dsco FORMAT=MBTILES -dsco MAXZOOM=${MAXZOOM} -dialect SQLITE \
      -nlt POINT25D \
      -fieldTypeToString StringList,IntegerList \
      `basename $in .000`.mbtiles $in

   set +x
done

# Displaying projection CRS EPSG:3857
# WSG 84 Pseudo-Mercator

