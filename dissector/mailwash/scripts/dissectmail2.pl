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
use Date::Parse;
use MIME::Words;
use Text::Iconv;
use Encode qw/encode decode/;
# if you get a lot of unknown.wri-files, try to add the contenttype to this table with
# a reasonable mapping to some default file extension.

my %transtable = (
		  "text/plain" => "plain.wri",
		  "text/html" => "text.html"
		  );


sub headertoutf8 {
  my ($headerval)=@_;
  my $rawtext=$headerval;
  my $rawencoding="LATIN1";
  if ($headerval =~ /=\?(.*)\?\w+\?.*\?=/) {
     $rawencoding=$1;
     $rawtext=MIME::Words::decode_mimewords($headerval);
  }
  my $converted=$rawtext; 
  if ($rawtext =~ /\033\$/) {
     $rawtext =~ s/\033\\\(B/\033\(B/g;
     $rawtext =~ s/^\$B/\033\$B/g;
     $rawtext =~ s/([^\033])\$B/$1\033\$B/g;
     $converted=decode("iso-2022-jp",$rawtext);
  } else {
    my $converter;
    eval {
      $converter = Text::Iconv->new($rawencoding, "UTF-8");
    };
    if ($@) {
      return "OCFA_UNSUPPORTED_ICONV_CHARSET_$rawencoding";
    }
    $converted = $converter->convert($rawtext); 
    unless ($converted) {
       if ($rawtext) {
         return "OCFA_ICONV_CONVERSION_PROBLEM_FROM_$rawencoding";
       }
    }
  }
  $converted =~ s/\r//g;
  $converted =~  s/\n$//;
  $converted =~  s/\n/ /g;
  $converted =~ s/\033/[ESCAPE-CHARACTER]/g;
  return $converted;
}


# this function recursively writes the bodies and headers to corresponding files.
# perldoc MIME::Entity (parts & bodies) might explain some stuff
# 
sub parse_entity($$$$$){ # reference to entity, filename/depth, workdir
  my ($entity,  $depth, $workdir,$count,$level) = @_;
  my $i = 0;
  my $msgid="${count}_${level}";
  if ($level == 0) {
     ($msgid) = ($entity->head->get("Message-ID") =~ /\<(.*)\>/) || "NOID_${count}_${level}_$depth";
     open(HEADERS,">${workdir}/${count}_${level}mail.hdrs") || die "2";
     print HEADERS $entity->head->as_string;
     close(HEADERS);
     print "MAILWASH:HEADERS:${count}_${level}mail.hdrs\n";
     my $date=$entity->head->get("Date");
     if ($date) {
       my $time = str2time($date);
       my $addr=$entity->head->get("From");
       my $timesource="";
       if ($addr =~ /\@/) {
               $addr =~ s/.*\<//;
	       $addr =~ s/\>.*//;
	       $addr =~ s/\".*\"//;
	       $addr =~ s/\s+//g;
               $addr =~ s/\'//g;
               $addr =~ tr/\x80-\xff/_/;
               $addr =~ s/.*\?=//;
	       if ($addr =~ /\@/ ) {
                  $timesource=$addr;
	       }
               if ($timesource =~ /\033/) {
                  $timesource="";
               }
       }
       if (($time) && ($timesource)) {
           print "MAILWASH:DATETIME:creatime:${time}:${timesource}\n";
       }
     }
     my @received=$entity->head->get("Received");
     my ($index);
     my $first=1;
     foreach $index ($#received,0) {
       my $type="recvtime";
       if ($first) {
          $type="sendtime";
	  $first=0;
       }
       my $rec=$received[$index];
       if ($rec) {
         $rec =~ s/\n/ /g;
         $rec =~ s/\r/ /g;
	 $rec =~ s/\s\s/ /g;
         if ($rec =~ /.*by\s+(\S+)\s+.*\;(.*)/)
         {
            $name=$1;
            $name =~ s/\'//g;
            $name =~ s/\033..//g;
            $name =~ tr/\x80-\xff/_/;
            $date=$2;
	    $date =~ s/^\s*//;
	    $date =~ s/\).*/\)/;
	    $date =~ s/\s+$//;
	    $date =~ s/\s+id.*//;
	    if ($date) {
	         $time = str2time($date);
		 if ($time) {
	           print "MAILWASH:DATETIME:$type:${time}:$name\n";
		 }
	    } 
	 }
       }
     }
     my $subject=headertoutf8($entity->head->get("subject"));
     $subject =~ s/\r//g;
     $subject =~ s/\n/ /g;
     if ($subject) {
       print "MAILWASH:UTFSTRING:subject:$subject\n";
     }
     my $agent=$entity->head->get("user-agent");
     unless($agent) {
        $agent=$entity->head->get("x-mailer");
     }
     if ($agent) {
        chomp($agent);
        print "MAILWASH:STRING:useragent:$agent\n";
     }
     my $status=$entity->head->get("status");
     if ($status =~  /RO/) {
        print "MAILWASH:STRING:status:read\n";
     } elsif ($status =~ /O/) {
        print "MAILWASH:STRING:status:opened\n";
     } else {
        print "MAILWASH:STRING:status:undefined\n";
     }
     foreach $header ("From","To","Reply-To","Sender","Envelope-To","Cc","Return-Path","Message-ID","In-Reply-To","References") {
         my $hval=$entity->head->get($header);
         my $fullheader=headertoutf8($hval);
         if ($fullheader) {
           if (($header eq "To")||($header eq "Cc") ) {
             my (@adresses) =split(/\,/,$fullheader);
             my $addr;
             foreach $addr (@adresses) {
                 print "MAILWASH:UTFARRAY:full$header:$addr\n"; 
             }
           } elsif (($header eq "From") || ($header eq "Reply-To") || ($header eq "Sender") ) {
              print "MAILWASH:UTFSTRING:full$header:$fullheader\n"; 
            } 
         }
	 if ($hval) {
	   my @hparts=split(/[\n\r\<\>]+/,$hval);
	   foreach $hpart (@hparts) {
	     my $addr;
	     my (@adresses) =split(/\,/,$hpart);
	     foreach $addr (@adresses) {
	       if ($addr =~ /\@/) {
                 $addr =~ s/\033..//g;
	         $addr =~ s/.*\<//;
	         $addr =~ s/\>.*//;
	         $addr =~ s/\".*\"//;
	         $addr =~ s/\s+//g;
                 $addr =~  s/\'//g;
                 $addr =~ tr/\x80-\xff/_/;
	         if ($addr =~ /\@/ ) {
		   if (($header eq "To")||($header eq "Cc") || ($header eq "References")) {
	             print "MAILWASH:ARRAY:$header:$addr\n";
		   } else {
                     print "MAILWASH:STRING:$header:$addr\n";
		   }
	         }
	       }
	     }
	   }
	 }
     }
  } 
  if ($entity->parts > 0){
    foreach ($entity->parts()){
      parse_entity($_,  ($depth . ".$i"), $workdir, $count,$level+1);
      $i++;
    }
  }
  else {
    my $recomended=$entity->head->recommended_filename();
    $recomended =~ tr/\x80-\xff/_/;
    $recomended =~ s/\033..//g;
    $recomended =~ s/:/_/g;
    my $outfilename = "${count}_${level}_${depth}_" .($transtable{$entity->effective_type()} ||  "unknown");
    my $outfilepath = $workdir . "/" . $outfilename;
    if (defined $entity->bodyhandle){
      open OUT, "> $outfilepath" or die "Could not open $outfilepath for writing.";
      my ($mimetype)=$entity->head->mime_type;
      my ($charset)= uc($entity->head->mime_attr('content-type.charset'));
      $mimetype =~ s/\;.*//;
      $mimetype =~ s/\s+//;
      unless ($mimetype =~ /^\w+\/\w+$/) {
          $mimetype="text/x-ocfa-mailcontent";
      }
      my ($mimetop)=$mimetype;
      $mimetop =~ s/\/.*//;
      #print "DERIVEDEVIDENCE:$outfilepath\n";
      print "MAILWASH:BODY:$outfilename\nMAILWASH:BODYSTRING:mimetype:$mimetype\nMAILWASH:BODYSTRING:mimetop:$mimetop\n";
      if ($charset) {
        print "MAILWASH:BODYSTRING:charset:$charset\n";
      } elsif ($mimetype =~ /text/) {
        print "MAILWASH:BODYSTRING:charset:LATIN1\n";
      }
      if ($recomended) {
         print "MAILWASH:BODYSTRING:bodyname:$recomended\n";
         if ($recomended =~ /\.(\w{2,5})$/) {
            print "MAILWASH:BODYSTRING:fileextention:$1\n";
         }
      }
      $entity->bodyhandle->print(\*OUT);
      close OUT;
    }
  }
}
$parser = new MIME::Parser;
$parser->extract_uuencode(1);
$parser->decode_headers(0); 
$parser->output_to_core(1);

my ($messagefile, $workdir) = @ARGV;
#We must trust the binary to give sane paths
$messagefile =~ /^(.*)$/; $messagefile=$1;
$workdir =~ /^(.*)$/; $workdir=$1;
our ($header, $body, $entity, $parser);
$message="";
my $count=1;
open(MESSAGE,"$messagefile") or die "Unable to open $messagefile:$!\n";
print "MAILWASH:START:$count\n";
while(<MESSAGE>) {
  if (/^From\s+(\S+)\s+\w{3}\s+\w{3}\s+\d{1,2}\s+\d{2}:\d{2}:\d{2}\s+[12][09][90]\d\r?$/) {
     if ($message) {
       $entity = $parser->parse_data($message);
       parse_entity($entity, "body", $workdir, $count,0);
       print "MAILWASH:END:$count\n\n";
       $message="";
       $count++;
       print "MAILWASH:START:$count\n";
     }
  } else {
      $message .= $_;
  }
}
if ($message) {
  $entity = $parser->parse_data($message);
  parse_entity($entity, "body", $workdir, $count,0);
  print "MAILWASH:END:$count\n\n";
  $message="";
}

