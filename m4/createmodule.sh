#!/bin/bash

clear
echo "This script creates C++ files (header, source and makefile) which" 
echo "allow you to develop OCFA modules."
echo 

echo "Enter a name for the module you want to create (suggestion: name it" 
echo "after the tool or library it wraps, or the type of evidence it "
echo "processes). Use lowercase only and no spaces, dots or other chars which might confuse this script or m4."
echo -n "Enter name: "
read modulename
clear
echo
echo Modulename set to $modulename.
echo
echo Terminology; 
echo "Extractor: A module which extracts metadata from evidence (for example the 'file'-module)."
echo "Dissector: A module which derives 'new' evidence from parent evidence (for example the 'zip'-module)."
echo 

echo "What kind of module type do you want to create? "
PS3='Enter the number of your choice: '
TYPES="extractor dissector"
select mtype in $TYPES 
do
  if [ $mtype ]; then
     BASEDIR="../$mtype/$modulename"
     if [ -d $BASEDIR ]; then
        echo 
        echo Directory \"$BASEDIR\" already exists. Since we do not want to overwrite files, 
        echo you have remove \"$BASEDIR\" first, or choose another name for the module.
        echo Bailing out ...
        exit
     fi
     INCDIR=$BASEDIR"/inc"
     SRCDIR=$BASEDIR"/src"
     mkdir -p $INCDIR
     mkdir -p $SRCDIR
     m4 -D moduletype=$mtype  -D modulename=$modulename -D HPP module.m4 > $INCDIR/$modulename.hpp
     m4 -D moduletype=$mtype  -D modulename=$modulename -D CPP module.m4 > $SRCDIR/$modulename.cpp
     m4 -D modulename=$modulename -D MAKE module.m4 > $BASEDIR/Makefile
     break
  else
     echo 'Please enter a valid choice or press ^C (Ctrl-C) to exit this script.'
  fi
 
done
clear
echo
echo The following files have been created:
find $BASEDIR -type f 
echo 
echo "You might want to 'cd' into $BASEDIR and type 'make' to test whether the code compiles."
echo 
echo The source files themselves contain hints on how to proceed.


