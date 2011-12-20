#!/usr/bin/perl
my $DB=$ARGV[0];
open(TABLES,"echo select name from sqlite_master where type=\\'table\\'\\;| sqlite3 $DB|"); 
@tables=<TABLES>;
close(TABLES);
open(SQLITE,"|sqlite3 $DB");
foreach $table (@tables) {
   $table =~ s/\r//;
   chomp($table);
   if ($table =~ /^[a-zA-Z0-9\_\-]+$/) {
     print SQLITE "select '======================================== $table  Schema ================================================';\n";
     print SQLITE "select 'SCHEMA', sql from sqlite_master where type='table' and name='$table';\n";
     print SQLITE "select '========================================  $table  Data  ================================================';\n";
     print SQLITE "select * from $table ;\n";
     print SQLITE "select '========================================  $table  Done  ================================================';\n";
   }
}
close(SQLITE);
