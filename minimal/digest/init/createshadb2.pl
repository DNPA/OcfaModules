#!/usr/bin/perl

#  The Open Computer Forensics Architecture moduleset.
#  Copyright (C) 2003..2006 KLPD  <ocfa@dnpa.nl>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

use DB_File;
if ($#ARGV == -1) {
  $DBDIR=".";
  print "NOTE: using current dir to create db files in\n";
} else {
  $DBDIR=$ARGV[0];
}
$|=1;
$targetdb="$DBDIR/digestdb";
$addinfodb="$DBDIR/adinfodb";
$proddb="$DBDIR/proddb";
unlink($targetdb);
unlink($addinfodb);
unlink($proddb);
$X = tie(%DIGESTDB,  'DB_File', "$targetdb",O_CREAT, 0644, $DB_BTREE) || die "Cant tie to db file $targetdb : $!";
$X2 =tie(%ADDINFO,  'DB_File', "$addinfodb",O_CREAT, 0644, $DB_BTREE) || die "Cant tie to db file $addinfodb : $!";
$X3 = tie(%PROD,'DB_File', "$proddb",O_CREAT, 0644, $DB_BTREE) || die "Cant tie to db file $proddb : $!";
$cnt=0;
print "Give sourcename, productinfo file and a list of digest files\n\tor send an end of file:";

#read the imput files
while (<>) {
  chomp;
 ($sourcename,$productfile,@digestfiles)=split(/[ ,]+/,$_);
 unless (($sourcename) && ($#digestfiles >= 0) && ($productfile)) {
    print STDERR "Invalid file specification line\n";
    exit;
 }
 unless (open(PFIL,"$productfile")) {
     print STDERR "OOPS cant open $productfile\n";
     exit;
 }
 print "Processing $productfile\n";
 while(<PFIL>) {
   s/\r//;
   if (/^(\d+)\,\"(.*)\",\"(.*)\",\"(.*)\",\"(.*)\",\"(.*)\",\"(.*)\"$/) {
      print "product['${sourcename}_${1}:name'] = $2\n";
      print "product['${sourcename}_${1}:type'] = $7\n";
      $PROD{"${sourcename}_${1}:name"}=$2;
      $PROD{"${sourcename}_${1}:type"}=$7;
   }
 }
 close(PFIL);
 foreach $digestfile (@digestfiles) {
   $cnt=0;
   unless (open(INF,"$digestfile")) {
      print STDERR "OOPS cant open $digestfile\n";
      next;
   }
   print "Processing $digestfile\n";
   while(<INF>)
   {
     s/\r//;
     #"b7ffc19b901de6eabab177e7b582100592304730","1a5d4e227ef0a2fe1c5b9403c6ae0e01","be6025d2","SKYPESETUP.EXE-00C2485F.pf","16268","1","WINDOWS",""
     if (/^\"([0-9A-F]{40})\",\".*\",\".*\",\"(.*)\",(\d+),(\d+),\"(.*)\",\"(.*)\"$/i)
     {
       $digest=lc($1);
       $filename=$2;
       #$filesize=$3;
       $productcode=$4;
       #$oscode=$5;
       #$special=$6;
       print "digest['$digest'] = '${sourcename}:${filename}'\n";
       print "addinfo['${sourcename}:${digest}'] = '${sourcename}_${productcode}'\n";
       $DIGESTDB{"$digest"}="${sourcename}:$filename";
       $ADDINFO{"${sourcename}:$digest"}="${sourcename}_${productcode}";
       $cnt++;
       if ($cnt >= 1000) {print "+";$cnt=0;}
     } else {
       # print "# $_";
     }
   }
   close(INF);
   print "\n";
 }
 print "Give sourcename, productinfo file and a list of digest files \n\tor send an end of file:";
}
untie %PROD;
untie %DIGESTDB;
untie %ADDINFO;
