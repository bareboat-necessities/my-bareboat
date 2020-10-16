#!/bin/bash

cd $SMAC_M_HOME || exit 255

mkdir ../noaa_enc/
cd ../noaa_enc/ || exit 255
wget https://www.charts.noaa.gov/ENCs/NJ_ENCs.zip
unzip NJ_ENCs.zip
mkdir shp

cd $SMAC_M_HOME || exit 255

source "$(pipenv --venv)/bin/activate"

python3 bin/generate_toml_config.py \
          --chart ../noaa_enc/ENC_ROOT \
          -rule-default-color IHO \
          --chartsymbols ./chart-installation/generate_map_files/resources/chartsymbols/chartsymbols_S57.xml \
          -enhancedchartdata ../noaa_enc/shp \
          --tablename Paper \
          --displaycategory Standard \
          --rule-set-path ./chart-installation/generate_map_files/resources/rules/ \
          -o config.enc.noaa.toml

python3 ./bin/generate_shapefiles.py config.enc.noaa.toml

python3 ./chart-installation/generate_map_files/generate_map_config.py config.enc.noaa.toml

# URL
#
# http://localhost/cgi-bin/mapserv?map=/home/pi/TILE/noaa_enc/map/SeaChart_DAY_BRIGHT.map&SERVICE=WMS&VERSION=1.3.0&REQUEST=GetMap&FORMAT=application/openlayers&TRANSPARENT=true&LAYERS=default&WIDTH=1024&HEIGHT=1024&CRS=EPSG%3A3857&STYLES=&BBOX=-9069712.02694336%2C3649409.477939453%2C-9064820.05713379%2C3654301.4477490233