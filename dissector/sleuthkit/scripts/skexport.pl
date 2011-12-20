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

use IO::Seekable;
use FileHandle;

sub nonexist {
  my ($orig)=@_;
  my $prefix=$orig;
  my $extension="";
  if ($orig =~ /^(.*)\.(\w+)$/) {
    $prefix=$1;
    $extension=$2;
  }
  foreach my $seq (1 .. 99) {
    my $new=$orig . "_" . $seq . "." . $extension; 
    unless (-e $new) {
      print STDERR "USING A NEW ALTERNATIVE NAME FOR $orig : $new AS INITIAL NAME EXISTS\n";
      return $new;
    }
  }
  return $orig;
}

sub sparse {
  my ($cmd,$output)=@_;
  my $pwd=$ENV{'PWD'};
  unless (open(ZERO,"/dev/zero")) {
     print STDERR "Unable to open /dev/zero\n";
     return;
  }
  my $nullblock;
  read(ZERO,$nullblock,4096);
  close(ZERO);
  if (-e $output) {
    $output=&nonexist($output); 
  }
  unless (sysopen(OUTFIL,$output,O_WRONLY | O_CREAT | O_EXCL| O_LARGEFILE)) {
      print STDERR "Problem opening destination '$output' ($pwd) :  $!\n";
      return;
  }
  unless (open(CMD,"$cmd|")) {
      print STDERR "Problem starting command : $cmd\n";
      return;
  } 
  my $datasize;
  my $datablock;
  my $lastnull=0;
  while ($datasize = read(CMD,$datablock,4096)) {
    if ($datablock eq $nullblock) {
      sysseek(OUTFIL,$datasize,SEEK_CUR);
      $lastnull=1;
    } else {
      $lastnull=0;
      syswrite(OUTFIL,$datablock,$datasize);
    }
  } 
  if ($lastnull) {
     sysseek(OUTFIL,-1,SEEK_CUR);
     $data="\0x00";
     syswrite(OUTFIL,$data,1);
  }
}

$OCFAROOT=$ENV{"OCFAROOT"};
unless ($OCFAROOT) {
  print STDERR "No OCFAROOT set !!\n";
  exit;
}
$BINCONF=$OCFAROOT . "/etc/binaries.conf";
unless (open(BCONF,$BINCONF)) {
  print STDERR "Unable to open $BINCONF for reading\n";
  exit;
}
$SKBINDIR="";
while (<BCONF>) {
  if (/BINARY_IMG_CAT\t(.*)img_cat/) {
    $SKBINDIR=$1;
  }
}
unless ($SKBINDIR) {
  print "No BINARY_IMG_CAT entry found in $BINCONF\n";
  exit;
}

$OUTPUT=shift(@ARGV);
$imglist=join(" ",@ARGV);
unless ($imglist) {
  print STDERR "Please specify image files\n";
  exit;
}
unless (open(MMLS,"${SKBINDIR}mmls $imglist|")) {
  print STDERR "Problem spawning \'${SKBINDIR}mmls $imglist\'\n";
  exit;
}
sub exportchunk {
  my ($subdir,$offset,$size,$imglist)=@_;
  my $cmd="${SKBINDIR}img_cat $imglist|dd skip=$offset";
  if ($size) {$cmd .= " count=$size";}
  print STDERR "$cmd\n";
  &sparse($cmd,"${subdir}.part");
}
sub sanitize {
  my ($name)=@_;
  $name =~ s/[^a-zA-Z0-9\.\_\-]/_/g;
  return $name;
}
sub doinode {
   my ($subdir,$offset,$size,$imglist,$inode) = @_;
   my ($cmd)="${SKBINDIR}fls -o $offset $imglist $inode";
   print STDERR "$cmd\n";
   open(FLS,"$cmd|");
   my (@dirlist)=<FLS>;
   close(FLS);
   my ($line);
   foreach $line (@dirlist) {
      if ($line =~ /^.\/(\w)\s+(\*?)\s*((\d+)\S*):\s+(\S.*)$/) {
         my ($type)= $1;
	 my ($deleted)=$2;
	 my ($childstream)=$3;
	 my ($childinode)=$4;
	 my ($name)=$5;
         $childstream =~ s/\(.*//;
	 $name=&sanitize($name);
	 if ($deleted) {
            $name="SKDELETED_$name";
	 }
	 if ($type eq "d") {
            mkdir("$subdir/$name",0755);
	    #RJM: Quickfix, aparently sometimes infinite loops arise here so we dont support deleted dirs!
	    unless ($deleted) {
	        doinode("$subdir/$name",$offset,$size,$imglist,$childinode);
	    }
	 } elsif ( $type eq "l" ) {
	    my ($cmd)="${SKBINDIR}icat -o $offset $imglist $childstream";
	    print STDERR "$cmd\n";
            &sparse($cmd,"$subdir/SKSYMLINK_$name");
	 } else {
	    if ($deleted) {
	      my ($cmd)="${SKBINDIR}icat -r -o $offset $imglist $childstream";
	      print STDERR "$cmd\n";
              &sparse($cmd,"$subdir/$name");
	    } else {
	      my ($cmd)="${SKBINDIR}icat -o $offset $imglist $childstream";
	      print STDERR "$cmd\n";
              &sparse($cmd,"$subdir/$name");
	    }
	 }
      } 
   }
}
sub dounalloc {
  my ($dumpdir,$offset,$size,$imglist) =@_;
  my ($cmd1) = "${SKBINDIR}dls -o $offset $imglist";
  my ($cmd2) = "${SKBINDIR}dls -s -o $offset $imglist";
  print STDERR "$cmd1\n";
  &sparse($cmd1,"$dumpdir/data.unaloc");
  print STDERR "$cmd2\n";
  &sparse($cmd2,"$dumpdir/data.slack");
}
sub dopartition {
  ($subdir,$offset,$size,$imglist)=@_;
  my $pdir="$OUTPUT/$subdir";
  my $linecount=0;
  my ($cmd)= "${SKBINDIR}fls -o $offset $imglist";
  print STDERR "$cmd\n";
  open(FLS,"$cmd|");
  while(<FLS>) {
     $linecount++;
  }
  close(FLS);
  if (($linecount==0) || $?) {
     #print "NoFS $subdir\n";
     &exportchunk($pdir,$offset,$size,$imglist);     
  } else {
     #print "FS   $subdir\n";
     mkdir($pdir,0755);
     mkdir("$pdir/ROOTDIR",0755);
     mkdir("$pdir/UNALLOC",0755);
     &doinode("$pdir/ROOTDIR",$offset,$size,$imglist,"");
     &dounalloc("$pdir/UNALLOC",$offset,$size,$imglist);
  }
}
mkdir("$OUTPUT",0755)|| die "Unable to create $OUTPUT dir";
$count=0;
while (<MMLS>) {
  if (/^\d\d:\s+\S+\s+(\d+)\s+\d+\s+(\d+)\s+(.*)$/) {
    $lable++;
    $offset=$1;
    $size=$2+0;
    $info=$3;
    $info =~ s/\s*\(.*//;
    $info =~ s/\s*\/.*//;
    $info =~ s/\s+/_/g;
    &dopartition("${lable}_$info",$offset,$size,$imglist);
    $count++;
  }
}
unless ($count) {
  &dopartition("NO_PTABLE",0,$size,$imglist);
}
