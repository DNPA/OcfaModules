#!/usr/bin/perl
use Mail::Box::Dbx;
sub processfolder {
   my ($folder,$basedir)=@_;
   $count=0;
   foreach $message (@$folder) {
     $count++;
     open(EML,">${basedir}/${count}.eml")|| die "Could not create output file";
     select(EML);
     $message->print;
     close(EML);
     select(STDOUT);
   }
}
my $mbox=$ARGV[0];
my $outputdir=$ARGV[1];
unless ((-d "$outputdir") && (-f "$mbox")) {
  print "Usage:   dbxsplit.pl <dbxfile> <outputdir>\n";
  exit 1;
}
my $folder = Mail::Box::Dbx->new(folder => $mbox);
unless ($folder) {
  print STDERR "Problem opening $mbox : $!\n";
  exit 3;
}
$newdir="${outputdir}/output";
unless (mkdir($newdir)) {
  print "Problem creating output dir $newdir\n";
  exit 2;
}
&processfolder($folder,$newdir);
