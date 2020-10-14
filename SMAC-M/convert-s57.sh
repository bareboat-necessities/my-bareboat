#!/bin/bash

#
# run it like this:
#  SMAC_M_HOME=<path_to where you cloned https://github.com/LarsSchy/SMAC-M> \
#  ENC_ROOT=<path to directory with ENC files> \
#  OUT=<output-toml-config> \
#  ./convert-s57.sh

OLD_DIR=`pwd`

cd ${SMAC_M_HOME} || exit 255

python3 ./bin/generate_toml_config.py \
     --chart ${ENC_ROOT} \
     -rule-default-color IHO \
     --chartsymbols ./chart-installation/generate_map_files/resources/chartsymbols/chartsymbols_S57.xml \
     -enhancedchartdata ../chart-installation/data_files_conversion/shp_s57data/shp \
     --tablename Paper \
     --displaycategory Standard \
     --rule-set-path ./chart-installation/generate_map_files/resources/rules \
     -o ${OUT}

cd ${OLD_DIR} || exit 255
