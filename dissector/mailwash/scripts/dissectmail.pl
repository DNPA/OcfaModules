#!/usr/bin/perl -w

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

# Note: You need a subdirectory './work' containing al the email messages
#       You need a subdirectory './result' in which the results will be put
#       You need '.' containing this script :-)

# Btw; the script does not check if the above conditions are met.  

# Todo: Calculation of SHA1 & hashes for all subentities.
#       Add more stuff to %transtable
#       Work- and result-directories are hard coded. Maybe you want to change this.
#       Howto on how to use this script.  

use MIME::Parser;
use Mail::Internet;
use Mail::Header;
use Mail::Address;
use File::Basename;

# if you get a lot of unknown.wri-files, try to add the contenttype to this table with
# a reasonable mapping to some default file extension.

my %transtable = (
		  "text/plain" => "plain.wri",
		  "text/html" => "text.html"
		  );



# this function recursively writes the bodies and headers to corresponding files.
# perldoc MIME::Entity (parts & bodies) might explain some stuff

sub parse_entity($$$){ # reference to entity, filename/depth, workdir
  my ($entity,  $depth, $workdir) = @_;
  my $i = 0;
  $entity->head->print;
  if ($entity->parts > 0){
    foreach ($entity->parts()){
      parse_entity($_,  ($depth . ".$i"), $workdir);
      $i++;
    }
  }
  else {
    my $outfilename = $depth . "_" . ($entity->head->recommended_filename() || $transtable{$entity->effective_type()} 
						     || ((print STDERR "Warning: unknown contenttype saved as text for entity $depth !\n") && "unknown.wri"));
    my $outfilepath = $workdir . "/" . $outfilename;
    if (defined $entity->bodyhandle){
      open OUT, "> $outfilepath" or die "Could not open $outfilepath for writing.";
      print "DERIVEDEVIDENCE:$outfilename\n"; # filename must be relative to workdir
      $entity->bodyhandle->print(\*OUT);
      close OUT;
    }
  }
}


my ($messagefile, $workdir) = @ARGV;
print STDERR "Infile: $messagefile \nWorkdir: $workdir \n";
my ($header, $body, $entity, $parser);

open(MESSAGE,"$messagefile") or die "Unable to open $messagefile:$!\n";
  
$parser = new MIME::Parser;
$parser->extract_uuencode(1);
$parser->output_to_core(1);
$entity = $parser->parse(\*MESSAGE);
close(MESSAGE);
$entity->dump_skeleton();
# write the contents of the various parts and bodies to file
parse_entity($entity, basename($messagefile), $workdir);


