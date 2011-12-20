#!/usr/bin/perl
use Mail::Box::Mbox;
my $mbox=$ARGV[0];
my $outputdir=$ARGV[1];
unless ((-d "$outputdir") && (-f "$mbox")) {
  print "Usage:   mailsplit.pl <mbox> <outputdir>\n";
  exit 1;
}
my $folder = Mail::Box::Mbox->new(folder => $mbox);
unless ($folder) {
  exit 3;
}
if (($folder->size > 0) && ($folder->nrMessages(ALL) == 0)) {
  exit 3;
}
$newdir="${outputdir}/output";
unless (mkdir($newdir)) {
  print "Problem creating output dir $newdir\n";
  exit 2;
}
my $count=0;
foreach $message (@$folder) {
  $count++;
  open(EML,">${newdir}/${count}.eml")|| die "Could not create output file";
  select(EML);
  $message->print; 
  close(EML);
  select(STDOUT);
}
exit 0;
