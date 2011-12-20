#!/usr/bin/perl
$file=$ARGV[0];
$workdir=$ARGV[1];
die "No such file '$file'" unless (-f $file);
die "No such workdir '$workdir'" unless (-d $workdir);
die "Could not open '$file'" unless open(INPUT,$file);
die "Could not create output dir" unless mkdir("${workdir}/output",0750);
while(<INPUT>) {
   if ($inmail) {
      if ($_ eq ".\r\n") {
         close(MAIL);
         $inmail=0;
      } else {
         s/^\.//;
         print MAIL "$_";
      }
   } else {
      if ((!(/^\+/)) && (/:/)) {
         $inmail=1;
         $counter++;
         open(MAIL,">${workdir}/output/${counter}.eml");
         print MAIL "$_";
      }
   }
}
