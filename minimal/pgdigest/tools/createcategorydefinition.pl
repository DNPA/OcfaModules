#!/usr/bin/perl
use DBI;
use strict;
my ($dbname,$country,$source,$description)=@ARGV;
unless ($description) {
   $description="non given";
} 
unless ($source) {
   $source="TESTSET";
}
unless ($country) {
   print STDERR "USAGE:\n\n\t createcategorydefinition.pl< DBNAME> <COUNTRYCODE> [SOURCENAME [ DESCRIPTION] ] \n\n";
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
my $query="INSERT INTO categorydefinitions (country,name,description)  VALUES (?,?,?)";
my $sth = $dbh->prepare($query);
$sth->execute($country,$source,$description);
print "Done.\n";
