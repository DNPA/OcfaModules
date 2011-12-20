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
$|=1;
$NISTBASEDIR="/nist";
$KPHKS="./kp1.hsh";
$DBDIR=".";
$targetdb="$DBDIR/digestdb";
$addinfodb="$DBDIR/adinfodb";
$proddb="$DBDIR/proddb";
unlink($targetdb);
unlink($addinfodb);
unlink($proddb);
$X = tie(%DIGESTDB,  'DB_File', "$targetdb",O_CREAT, 0644, $DB_BTREE) || die "Cant tie to db file $targetdb : $!";
$X2 =tie(%ADDINFO,  'DB_File', "$addinfodb",O_CREAT, 0644, $DB_BTREE) || die "Cant tie to db file $addinfodb : $!";
$X3 = tie(%NISTPROD,'DB_File', "$proddb",O_CREAT, 0644, $DB_BTREE) || die "Cant tie to db file $proddb : $!";
$cnt=0;
open(INF,"$KPHKS")||die "OOPS cant open $KPHKS";
print "Processing $KPHKS\n";
while(<INF>)
{
  if (/^\d+,\d+,\"(.*)\",\"(.*)\",\"[0-9A-F]{32}\",.*,\"([0-9A-F]{40})\",/)
  {
     $filename=$1;
     $path=$2;
     $digest=lc($3);
     $DIGESTDB{$digest}="KPNIJMEGEN:$filename";
     $ADDINFO{"KPNIJMEGEN:$digest"}=$path;
     $cnt++;
     if ($cnt >= 1000) {print "*";$cnt=0;}
 }
}
close(INF);
print "\n";
open(INF,"${NISTBASEDIR}/1/NSRLProd.txt")||die "OOPS cant open ${NISTBASEDIR}/1/NSRLProd.txt";
print "Processing ${NISTBASEDIR}/1/NSRLProd.txt\n";
while(<INF>) {
  s/\r//;
  if (/^(\d+)\,\"(.*)\",\"(.*)\",\"(.*)\",\"(.*)\",\"(.*)\",\"(.*)\"$/) {
     $NISTPROD{"${1}:name"}=$2;
     $NISTPROD{"${1}:version"}=$3;
     $NISTPROD{"${1}:os"}=$4;
     $NISTPROD{"${1}:mf"}=$5;
     $NISTPROD{"${1}:lang"}=$6;
     $NISTPROD{"${1}:type"}=$7;
  }
}
close(INF);
foreach $subdir (1..4) {
$cnt=0;
open(INF,"${NISTBASEDIR}/${subdir}/NSRLFile.txt")||die "OOPS cant open ${NISTBASEDIR}/${subdir}/NSRLFile.txt";
print "Processing ${NISTBASEDIR}/${subdir}/NSRLFile.txt\n";
while(<INF>)
{
  s/\r//;
  if (/^\"([0-9A-F]{40})\",\".*\",\".*\",\"(.*)\",(\d+),(\d+),\"(.*)\",\"(.*)\"$/)
  {
    $digest=lc($1);
    $filename=$2;
    $filesize=$3;
    $productcode=$4;
    $oscode=$5;
    $special=$6;
    $DIGESTDB{"$digest"}="NIST:$filename";
    $ADDINFO{"NIST:$digest"}="${productcode}";
    $cnt++;
    if ($cnt >= 1000) {print "+";$cnt=0;}
  }
}
close(INF);
print "\n";
}


