/*
 * Copyright (C) 2003, by Keith J. Jones.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include<vector>
#include<string>
#include"libpasco.hpp"
//
/* This is the default block size for an activity record */
//
#define BLOCK_SIZE	0x80

#ifdef CYGWIN
ssize_t pread( int d, void *buf, size_t nbytes, off_t offset) {
  lseek( d, offset, SEEK_SET );
  read( d, buf, nbytes );
}
#endif

//
/* Backwards ASCII Hex to Integer */
//
unsigned int bah_to_i( char *val, int size ) {
  int total;
  int i;

  total = 0;

  for ( i=0; i < size; i++ ) {
    total += ((unsigned char)val[i] << 8*i);
  }

  return total;
}

//
/* Backwards 8 byte ASCII Hex to time_t */
//
time_t win_time_to_unix( char *val ) {
  unsigned long low, high;
  double dbl;
  time_t total;

  char fourbytes[4]; 

  fourbytes[0] = val[0];
  fourbytes[1] = val[1];
  fourbytes[2] = val[2];
  fourbytes[3] = val[3];

  low = bah_to_i( fourbytes, 4 );

  fourbytes[0] = val[4];
  fourbytes[1] = val[5];
  fourbytes[2] = val[6];
  fourbytes[3] = val[7];

  high = bah_to_i( fourbytes, 4 );

  dbl = ((double)high * pow(2,32));
  dbl += low;

  if ( dbl==0 ) {
    return 0;
  }

  dbl *= 1.0e-7;
  dbl -= 11644473600.0;

  total = (time_t)dbl;

  return total;
}

//
/* This function prepares a string for nice output */
//
int printablestring( char *str ) {
  int i;

  i = 0;
  while ( str[i] != '\0' ) {
    if ( (unsigned char)str[i] < 32 || (unsigned char)str[i] > 127 ) {
      str[i] = ' ';
    }
    i++; 
  }
  return 0;
}

//
/* This function parses a REDR record. */
//
int parse_redr( int history_file, double filever, int currrecoff, char *delim, int filesize, char *type, std::vector< URLRec * > &urls ) {
  char fourbytes[4];
  char hashrecflagsstr[4];
  char chr;
  int filenameoff;
  int httpheadersoff;
  int i;
  int reclen;
  int dirnameoff;
  time_t modtime;
  time_t accesstime;
  char *url;
  char *filename;
  char *httpheaders;
  char ascmodtime[26], ascaccesstime[26];
  char dirname[9];


  pread( history_file, fourbytes, 4, currrecoff+4 );
  reclen = bah_to_i( fourbytes, 4 )*BLOCK_SIZE; 

  url = (char *)malloc( reclen+1 );
            
  i = 0;
  pread( history_file, &chr, 1, currrecoff+0x10 );
  while ( chr != '\0' && currrecoff+0x10+i+1 < filesize ) {
    url[i] = chr;
    pread( history_file, &chr, 1, currrecoff+0x10+i+1 );
    i++; 
  } 
  url[i] = '\0';

  filename = (char *)malloc( 1 );
  filename[0] = '\0';

  httpheaders = (char *)malloc( 1 );
  httpheaders[0] = '\0';

  dirname[0] = '\0';

  ascmodtime[0] = '\0';
  ascaccesstime[0] = '\0';
  dirname[0] = '\0';

  printablestring( type );
  printablestring( url );
  printablestring( ascmodtime );
  printablestring( ascaccesstime );
  printablestring( filename );
  printablestring( dirname );
  printablestring( httpheaders );

  URLRec *urlrec = new URLRec(type, url, ascmodtime, ascaccesstime, filename, dirname, httpheaders);
  urls.push_back(urlrec);

  type[0] = '\0';

  free( url );
  free( filename );
  free( httpheaders );
}

//
/* This function parses a URL and LEAK activity record. */
//
int parse_url( int history_file, double filever, int currrecoff, char *delim, int filesize, char *type, std::vector< URLRec * > &urls  ) {
  char fourbytes[4];
  char hashrecflagsstr[4];
  char eightbytes[8];
  char chr;
  int filenameoff;
  int httpheadersoff;
  int urloff;
  int i;
  int reclen;
  int dirnameoff;
  time_t modtime;
  time_t accesstime;
  char ascmodtime[26], ascaccesstime[26];
  char dirname[9];
  char *url;
  char *filename;
  char *httpheaders;
  int year, mon;
  struct tm *accesstm, *modtm;

  pread( history_file, fourbytes, 4, currrecoff+4 );
  reclen = bah_to_i( fourbytes, 4 )*BLOCK_SIZE; 

  pread( history_file, eightbytes, 8, currrecoff+8 );
  modtime = win_time_to_unix( eightbytes );
  
  pread( history_file, eightbytes, 8, currrecoff+16 );
  accesstime = win_time_to_unix( eightbytes );
 
  accesstm = localtime( &accesstime );
  year = accesstm->tm_year + 1900;
  mon = accesstm->tm_mon + 1;
  sprintf( ascaccesstime, "%02d/%02d/%02d %02d:%02d:%02d", mon, accesstm->tm_mday, year, accesstm->tm_hour, accesstm->tm_min, accesstm->tm_sec );

  modtm = localtime( &modtime );
  year = modtm->tm_year + 1900;
  mon = modtm->tm_mon + 1;
  sprintf( ascmodtime, "%02d/%02d/%02d %02d:%02d:%02d", mon, modtm->tm_mday, year, modtm->tm_hour, modtm->tm_min, modtm->tm_sec );
  
  if (accesstime == 0) {
    ascaccesstime[0] = '\0';
  }

  if (modtime == 0) {
    ascmodtime[0] = '\0';
  }
  
  url = (char *)malloc( reclen+1 );

  if (filever >= 5) {
    pread( history_file, &chr, 1, currrecoff+0x34 );
  } else {
    pread( history_file, &chr, 1, currrecoff+0x38 );    
  }
  urloff = (unsigned char)chr;

  i = 0;
  pread( history_file, &chr, 1, currrecoff+urloff );
  while ( chr != '\0' && currrecoff+urloff+i+1 < filesize ) {
    url[i] = chr;
    pread( history_file, &chr, 1, currrecoff+urloff+i+1 );
    i++; 
  } 
  url[i] = '\0';

  filename = (char *)malloc( reclen+1 );

  if (filever >= 5) {
    pread( history_file, fourbytes, 4, currrecoff+0x3C );
  } else {
    pread( history_file, fourbytes, 4, currrecoff+0x40 );
  }
  filenameoff = bah_to_i( fourbytes, 4 ) + currrecoff; 

  i = 0;
  if (filenameoff > currrecoff+0x3C) {
    pread( history_file, &chr, 1, filenameoff );
    while ( chr != '\0' && filenameoff+i+1 < filesize ) {
      filename[i] = chr;
      pread( history_file, &chr, 1, filenameoff+i+1 );
      i++; 
    } 
  }
  filename[i] = '\0';

  if (filever >= 5.2) {
    pread( history_file, &chr, 1, currrecoff+0x38 );
  } else if (filever >= 5) {
    pread( history_file, &chr, 1, currrecoff+0x39 );
  } else {
    pread( history_file, &chr, 1, currrecoff+0x3C );
  }
  dirnameoff = (unsigned char)chr;

  if (0x50+(12*dirnameoff)+8 < filesize) {
    pread( history_file, dirname, 8, 0x50+(12*dirnameoff) );
    dirname[8] = '\0';
  } else {
    dirname[0] = '\0';
  }

  httpheaders = (char *)malloc( reclen+1 );

  if (filever >= 5) {
    pread( history_file, fourbytes, 4, currrecoff+0x44 );
  } else {
    pread( history_file, fourbytes, 4, currrecoff+0x48 );
  }
  httpheadersoff = bah_to_i( fourbytes, 4 ) + currrecoff; 

  i = 0;
  if (httpheadersoff > currrecoff+0x44) {
    pread( history_file, &chr, 1, httpheadersoff );

    while ( chr != '\0' && httpheadersoff+i+1 < currrecoff+reclen && httpheadersoff+i+1 < filesize ) {
      httpheaders[i] = chr;
      pread( history_file, &chr, 1, httpheadersoff+i+1 );
      i++; 
    }
  } 
  httpheaders[i] = '\0';
 
  printablestring( type );
  printablestring( url );
  printablestring( ascmodtime );
  printablestring( ascaccesstime );
  printablestring( filename );
  printablestring( dirname );
  printablestring( httpheaders );

  if (type[3] == ' ') {
    type[3] = '\0';
  }

  URLRec *urlrec = new URLRec(type, url, ascmodtime, ascaccesstime, filename, dirname, httpheaders);
  urls.push_back(urlrec);


  type[0] = '\0';
  dirname[0] = '\0';
  ascmodtime[0] = '\0';
  ascaccesstime[0] = '\0';

  free( url );
  free( filename );
  free( httpheaders );
}

int parse_unknown( int history_file, double filever, int currrecoff, char *delim, int filesize, char *type ) {
  type[0] = '\0'; 
}


//
//
// vector is filled with pointers to URLrec. Caller has to delete URLrecs when done with the vector.
//
int pascomain( std::string filename, bool deleted, std::string ieversion, std::vector< URLRec * > &urls ) {
  int history_file;
  char fourbytes[4];
  char chr;
  char delim[10];
  int currrecoff;
  int filesize;
  int i;
  int opt;
  time_t modtime;
  time_t accesstime;
  char type[5];
  char hashrecflagsstr[4];
  int hashoff;
  int hashsize;
  int nexthashoff;
  int offset;
  int hashrecflags;
  double filever;

  strcpy( delim, "\t" );

  history_file = open( filename.c_str(), O_RDONLY );

  if ( history_file < 0 ) { 
    return -3; 
  }

  pread( history_file, fourbytes, 4, 0x18 );
  ieversion = fourbytes ;
  filever = atof(fourbytes);

  pread( history_file, fourbytes, 4, 0x1C );
  filesize = bah_to_i( fourbytes, 4 );

  if (!deleted ) {

    pread( history_file, fourbytes, 4, 0x20 );
    hashoff = bah_to_i( fourbytes, 4 );
  
    while (hashoff != 0 ) {

      pread( history_file, fourbytes, 4, hashoff+8 );
      nexthashoff = bah_to_i( fourbytes, 4 );

      pread( history_file, fourbytes, 4, hashoff+4 );
      hashsize = bah_to_i( fourbytes, 4 )*BLOCK_SIZE;

      for (offset = hashoff + 16; offset < hashoff+hashsize; offset = offset+8) {
        pread( history_file, hashrecflagsstr, 4, offset );
        hashrecflags = bah_to_i( hashrecflagsstr, 4 );

        pread( history_file, fourbytes, 4, offset+4 );
        currrecoff = bah_to_i( fourbytes, 4 );

        if (hashrecflagsstr[0] != 0x03 && currrecoff != 0xBADF00D ) {
          if (currrecoff != 0) {

            pread( history_file, fourbytes, 4, currrecoff );

            for (i=0;i < 4;i++) {
              type[i] = fourbytes[i];
            }
            type[4] = '\0';

            if (strncmp(type,  "REDR", 4)  == 0 ) {

              parse_redr( history_file, filever, currrecoff, delim, filesize, type , urls);

            } else if ( (strncmp(type, "URL", 3)==0) || (strncmp(type, "LEAK", 4) == 0) ) {

              parse_url( history_file, filever, currrecoff, delim, filesize, type , urls );

            } else {

              parse_unknown( history_file, filever, currrecoff, delim, filesize, type );

            }
          }
        }
      }  
    hashoff = nexthashoff;
    }
  } else if (deleted) {

    currrecoff = 0;

    while (currrecoff < filesize ) {

      pread( history_file, fourbytes, 4, currrecoff );

      for (i=0;i < 4;i++) {
        type[i] = fourbytes[i];
      }
      type[4] = '\0';

      if (strncmp(type, "REDR", 4) == 0 ) {

        parse_redr( history_file, filever, currrecoff, delim, filesize, type, urls );

      } else if ( (strncmp(type, "URL", 3) == 0) || (strncmp(type, "LEAK", 4) == 0) ) {

        parse_url( history_file, filever, currrecoff, delim, filesize, type, urls );

      } else {

        parse_unknown( history_file, filever, currrecoff, delim, filesize, type );

      }

      currrecoff = currrecoff + BLOCK_SIZE;
    }

  }
  close (history_file);
}
