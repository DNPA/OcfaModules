
//  The Open Computer Forensics Architecture moduleset.
//  Copyright (C) 2003..2006 KLPD  <ocfa@dnpa.nl>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<iostream>        
#include<string.h>
#include <antiword.hpp>
#include <stdexcept>
#include <vector>
#include <errno.h>
#include "../../../binaries.hpp"
//using namespace ocfa;
using namespace ocfa::misc;
using namespace ocfa::store;
using namespace ocfa::evidence;

void Antiword::processEvidence() {
  FILE *command;
  char *line;
  size_t size;
  char cline[1024];

  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject();
  string wd=getWorkDir();
  getLogStream(LOG_DEBUG) << "Workdir is: " << wd << endl;
  
  if (evidence == 0) throw OcfaException("Evidence without evidence store",this);
  int retval = snprintf(cline,1023,"%s -m UTF-8.txt -s %s 2>%s",BINARY_ANTIWORD,
		  evidence->getAsFilePath().c_str(),
		  (wd+"/errlist").c_str()
  );
  if (retval < 0 || retval > 1023){
    getLogStream(LOG_CRIT) << "Commandstring does not fit in allocated buffer or an error occurred\nCmd: " << cline << endl;
    throw OcfaException("an error occurred while creating the command line argument", this);
  }

  cline[1023]=0; 
  getLogStream(LOG_DEBUG) << "Command is " << cline << endl;
  command=popen(cline,"r");
  if (command==0) 
    throw OcfaException("Unable to fork: antiword -s %s \n");
  size=0;
  line=0;
  int fdo = open((wd+"/output").c_str(),(O_WRONLY | O_CREAT | O_TRUNC),(S_IRUSR | S_IRGRP));
  if (fdo == -1){
    getLogStream(LOG_CRIT) << "Error opening output file: " << strerror(errno) << endl;
    throw OcfaException("Error opening output file", this);
  }
  while (getline(&line,&size,command) != -1) {
    if (line){
       write(fdo,line, strlen(line)); // we don't need to write the 0 terminator, I think 	   
       free(line);
    }
    line=0;
    size=0;
  }
  pclose(command);
  close(fdo);
  FILE *errorfil=fopen((wd+"/errlist").c_str(),"r");
  if (errorfil == 0) {
      throw OcfaException("Unable to open antiword errorfile for reading\n");
  }
  while (getline(&line,&size,errorfil) != -1) {
    if (line){
            logEvidence(LOG_ERR,line);
	    free(line);
    }
    line=0;
    size=0;
  }
  fclose(errorfil);
  delete evidence;
  evidence = 0;
  unlink((wd+"/errlist").c_str());
  Evidence *derived= derive("output",Scalar("output"));
  ScalarMetaValue mimetype(Scalar("text/plain"));
  ScalarMetaValue mimetop(Scalar("text"));
  ScalarMetaValue encoding(Scalar("utf-8"));
  derived->getActiveJob()->setMeta("mimetype", &mimetype);
  derived->getActiveJob()->setMeta("mimetop", &mimetop);
  derived->getActiveJob()->setMeta("charset", &encoding);
  submitEvidence(derived);
  delete derived;

  return ;
}

int main(int , char *[]){
  Antiword *antiword=0;
  try {
    antiword = new Antiword();
  } catch(OcfaException &ex) {
    ex.logWhat();
    cerr << "Ocfa Exeption cougth in module constructor: " << ex.what() << "\n";
    return 1;
  }
  try {
     antiword->run();
  } catch(OcfaException &ex) {
    ex.logWhat();
    antiword->getLogStream(LOG_CRIT) << "Ocfa Exeption cougth: " << ex.what() << "\n";
  } catch(...) {
    antiword->getLogStream(LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  antiword->PrintObjCount();
  delete antiword;
  return 0;
}

