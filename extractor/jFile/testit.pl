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

$PURE=99; #percentage of files that are true files of a single type.
$SKIPZERO=1; #dont include 0 values in the output.
while(<>) {
  chop;
  sysopen(IF,$_,O_RDONLY);
  sysread(IF,$buffer,64);
  close(IF);
  @bytes=split(//,$buffer);
  foreach $index (0 .. 63) {
     $loc=256*$index+ord($bytes[$index]);
     $COUNT[$loc]++;
  }
  $filecount++;
}
if ($filecount < 20) {
  die "You should provide at least 20 files of the same type\n";
}
print "#Auto generated magic file entry";
$firstline="";
foreach $index (0 .. 63) {
   $list="";
   $match=0;
   foreach $index2 ($SKIPZERO .. 255) {
     $cnt=$COUNT[256*$index+$index2];
     if ($cnt >= ($PURE * $filecount / 100)) {
       $match=1;
       print "\n";
       if ($firstline) {print ">";}
       else {$firstline=1;}
       print "$index\tbyte\t$index2";
     }
   }
}
print "\tapplication/x-ocfa-editme\n";
