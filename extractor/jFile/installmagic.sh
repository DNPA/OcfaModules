#!/bin/bash

#remove for embedding in makefile

#INSTALL_ETC=/usr/local/digiwash/etc

if [ $# -ne 1 ]
then 
	echo 
	echo USAGE: $0 install_dir
	echo Please give destination install dir for magic files
	echo
	exit 1
fi


if [ ! -d $1 ]
then
	echo Target path $1 does not exists! Please give legal destination path
	exit 1
fi

if [ -f $1/magic.mime ]
then
	rm $1/magic.mime
fi
cp magic.mime $1

if [ -f $1/magic.mime.mgc ]
then
	rm $1/magic.mime.mgc
fi
cp magic.mime.mgc $1

if [ -f $1/magic ]
then
	rm $1/magic
fi
cp magic $1

if [ -f $1/magic.mgc ]
then  
      rm $1/magic.mgc
fi
cp magic.mgc $1

exit 0

