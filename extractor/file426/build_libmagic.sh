#!/bin/bash
#
# Build a tweaked libmagic from file-4.26
# Extra ocfa magic entries embedded in magic.mgc
# 
# Store lib in ./libmagic/lib to build file module
# against the static lib libmagic.a


prefix=/usr/local/digiwash

tar zxf file-4.26_ocfa.tar.gz

cd file-4.26_ocfa

./configure --prefix=$prefix --exec-prefix=$prefix --includedir=$prefix/inc
make
sudo make install

cd ..
