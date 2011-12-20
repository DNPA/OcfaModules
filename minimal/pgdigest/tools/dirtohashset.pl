#!/usr/bin/perl
use strict;
use Digest::MD5;
use Digest::SHA qw(sha1);
use DBI;

sub process_file {
  my ($sth,$id,$file) =@_;
  my $sha = Digest::SHA->new("sha1");
  $sha->addfile($file);
  my $sha1 = $sha->hexdigest;
  my $md5hash = Digest::MD5->new();
  open(FIL,$file);
  $md5hash->addfile(*FIL);
  close(FIL);
  my $md5=$md5hash->hexdigest;
  print "$md5,$sha1,$id\n";
  $sth->execute($md5,$sha1,$id);
}

sub process_dir {
  my ($sth,$id,$dir) =@_;
  if (-d $dir) {
     if (opendir(D1,$dir)) {
       my @entries=readdir(D1);
       closedir(D1);
       foreach my $entry (@entries) {
          if (($entry ne ".") && ($entry ne "..")) {
             my $fullpath=$dir . "/" . $entry;
             if (-f $fullpath) {
                 &process_file($sth,$id,$fullpath);
             } elsif (-d $fullpath) {
                 &process_dir($sth,$id,$fullpath);
             }
          }
       }
     }
  }
}
my ($dbname,$county,$source,$clasification,$path)=@ARGV;
unless (-d $path) {
   print STDERR "Usage:\n\n\tdirtohashset.pl <DBNAME> <COUNTRYCODE> <CLASSIFICATIONSOURCE> <CLASSIFICATION> <PATH>\n\n";
} else {
  unless ($ENV{"PGPASSWORD"} && $ENV{"PGUSER"}) {
      print STDERR "Both PGUSER and PGPASSWORD should be set in the envinronment\n";
      return 1;
  }
  my $dbh  = DBI->connect("dbi:Pg:dbname=$dbname", "", "");
  unless ($dbh) {
     print STDERR "Unable to connect to database $dbname\n";
     exit 1;
  }
  my $query="SELECT categoryid from categorytable,categorydefinitions where categorydefinitions.country=? AND categorydefinitions.name = ? and categorydefinitions.categorydefinitionsid = categorytable.categorydefinitionsid and categorytable.name=?";
  my $sth = $dbh->prepare($query);
  $sth->execute($county,$source,$clasification);
  my @row = $sth->fetchrow_array();
  my $id=$row[0];
  unless ($id) {
     print STDERR "No categorytable entry found in database for CY=$county SRC=$source CLS=$clasification\n";
     exit 1;
  }
  my $query2="INSERT INTO hashset (md5,sha1,categoryid) VALUES(?,?,?)";
  $sth = $dbh->prepare($query2);
  &process_dir($sth,$id,$path);
}
