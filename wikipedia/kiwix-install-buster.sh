
sudo apt-get -y install kiwix-tools/buster-backports


# See https://archive.org/details/@wikipediaarchives

mkdir ~/Wikipedia && cd ~/Wikipedia
wget https://archive.org/download/wikipedia_en_all_novid_2018-10.zim_201904/wikivoyage_en_all_novid_2019-04.zim

cd ..

kiwix-serve -p 8543 ~/Wikipedia/*.zim

