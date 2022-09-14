#!/usr/bin/bash -e

cd ~
wget https://bitbucket.org/sas_team/sas.planet.bin/downloads/SAS.Planet.Release.220707.zip
wget -O sas-planet-patch.zip 'https://drive.google.com/uc?export=download&id=1xapw4gVa8d2rugKfq15yyGuY6gAfsR4L'
mkdir SASPlanet-2022/
cd SASPlanet-2022/
unzip ../SAS.Planet.Release.220707.zip 
cd Maps
unzip -u ../../sas-planet-patch.zip 
cd ..
wine SASPlanet.exe 
