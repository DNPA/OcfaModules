#!/usr/bin/perl
use strict;
use warnings;
sub dopartition {
  my ($output,$part,$num,$description)=@_;
  unless (open(META,">${output}/${num}.meta")) {
    print STDERR "Problem creating ${output}/${num}.meta\n";
    return 3;
  }
  print META "PATH=$part\n";
  print META "partno=$num\n";
  print META "description=$description\n";
  unless (open(FSSTAT,"/usr/bin/fsstat $part|")) {
    print STDERR "Problem running '/usr/bin/fsstat $part'\n";
    return 4;
  }
  $/ = "\n\n";
  while (<FSSTAT>) {
     my ($sectionname,$sepline,@lines)=split(/\n/,$_);
     if ($sectionname =~ /FILE SYSTEM/) {
        foreach my $line (@lines) {
          my ($key,$val) = split(/:/,$line);
	  $key =~ s/\s+$//;
	  $val =~ s/^\s+//;
	  $key = lc($key);
	  $key =~ s/\W+/_/g;
	  print META "$key=$val\n";
	}
     }
  }
  close(FSSTAT);
  close(META);
  print "PART:${output}/${num}.meta\n";
}
sub mmlscp {
  my ($image,$output)=@_;
  unless (open(MMLSCP,"/usr/bin/mmls-cp $image|")) {
    print STDERR "Problem forking /usr/bin/mmls-cp\n";
    return 2;
  }
  my @mmls=<MMLSCP>;
  close(MMLSCP);
  foreach my $line (@mmls) {
     if ($line =~ /^(\d+):\s.*\s(\/.*\.crv)\s+(\S.*)$/) {
       my $num=$1;
       my $path=$2;
       my $description=$3;
       &dopartition($output,$path,$num,$description);       
     }
  }
}
sub mmls {
  my ($image,$output)=@_;
  unless (open(MMLS,"/usr/bin/mmls $image|")) {
      print STDERR "Problem forking /usr/bin/mmls\n";
      return 2;
  }
  my @mmls=<MMLS>;
  close(MMLS);
  foreach my $line (@mmls) {
     if ($line =~ /^(\d+):\s+\S+\s+(\d+)\s+\d+\s+(\d+)\s+(\S.*$)$/) {
        my $num=$1;
        my $start=$2;
	my $size=$3;
	my $description=$4;
	my $path="${output}/${num}.dd";
	`dd if=$image of=$path bs=512 skip=$start count=$size`;
	&dopartition($output,$path,$num,$description);
	print "COPY:$path\n";
     }
  }
}

$|=1;
my $image=$ARGV[0];
my $output=$ARGV[1];
my @partitions;
my $mode="c";
while (-l $image) {
  $image=readlink($image);
}
unless (-f $image) {
  print STDERR "$image is not a file\n";
  exit 1;
}
if ($image =~ /\.crv$/) {
  $mode="l";
}
if ($mode eq "l") {
  &mmlscp($image,$output);
} else {
  &mmls($image,$output);
}
