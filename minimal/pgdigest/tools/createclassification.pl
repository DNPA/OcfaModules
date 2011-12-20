#!/usr/bin/perl
use DBI;
use strict;
my ($dbname,$country,$source,$clasification,$colorcode,$description)=@ARGV;
unless ($description) {
   $description="non given";
} 
unless ($colorcode) {
   $colorcode="black";
}
unless ($clasification) {
   print STDERR "USAGE:\n\n\tcreateclassification.pl <DBNAME> <COUNTRYCODE> <SOURCENAME> <CLASSIFICATION> [ COLOR [ DESCRIPTION] ] \n\n";
   exit 1;
}
unless ($ENV{"PGPASSWORD"} && $ENV{"PGUSER"}) {
      print STDERR "Both PGUSER and PGPASSWORD should be set in the envinronment\n";
      exit 1;
}
my $dbh  = DBI->connect("dbi:Pg:dbname=$dbname", "", "");
unless ($dbh) {
     print STDERR "Unable to connect to database $dbname\n";
     exit 1;
}
my $query="SELECT categorydefinitionsid FROM categorydefinitions WHERE country=? AND name=?";
my $sth = $dbh->prepare($query);
$sth->execute($country,$source);
my @row = $sth->fetchrow_array();
my $id=$row[0];
unless ($id) {
  print STDERR "No categorydefinitions entry found in database for CY=$country SRC=$source\n";
  exit 1;
}
print "categorydefinitions id found: $id\n";
my $query2="INSERT INTO categorytable (name,colorcode,description,categorydefinitionsid) VALUES (?,?,?,?)";
$sth = $dbh->prepare($query2);
$sth->execute($clasification,$colorcode,$description,$id);
print "Done.\n";
