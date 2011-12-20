#!/bin/dash

echo "ERROR: PLEASE MAKE A REAL MAKEFILE, Mister bin/dash"
echo ""


g++ src/fileMetaExtractor.cpp -o filemetaextrator -I inc -I /usr/local/ocfa2.1.0pl0/inc/ -L /usr/local/ocfa2.1.0pl0/lib/ -locfafacade -locfamodule -locfaevidence -locfatreegraph -locfafs -locfastore -locfamessage -lextractor
sudo cp filemetaextrator /usr/local/ocfa2.1.0pl0/sbin/
sudo chgrp ocfa /usr/local/ocfa2.1.0pl0/sbin/filemetaextrator
sudo chmod g+s /usr/local/ocfa2.1.0pl0/sbin/filemetaextrator
