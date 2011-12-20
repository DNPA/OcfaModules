
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

#include <Strings.hpp>
#include <stdexcept>
#include <vector>
#include <stdio.h>
#include <store/EvidenceStoreEntity.hpp>
#include "../../../binaries.hpp"
#include <boost/lexical_cast.hpp>
using namespace ocfa;
using namespace ocfa::evidence;
using namespace ocfa::misc;
using namespace ocfa::evidence;
using namespace ocfa::store;

Strings::Strings():EvidenceDeriveAccessor("strings","default"),mChopSize(0) {
  mChopSize=atoi(getConfEntry("chopsize").c_str())*1024*1024;
  if (mChopSize < 1024*1024) 
     mChopSize=1024*1024*64;  //The default is 64 MB, but a better default should be in the config.
}

bool Strings::isAlpha(char testchar) {
  if (((testchar >= '0') && (testchar <= '9')) ||
      ((testchar >= 'A') && (testchar <= 'Z')) ||
      ((testchar >= 'a') && (testchar <= 'z'))) {
     return true;
  }
  return false;
}

void Strings::tokenizerHack(char *line){
  size_t index;
  size_t tokensize=0;
  bool intoken=false;
  for (index=0;line[index]!=0;index++) {
     if (intoken == true) {
        if (isAlpha(line[index])) {
           tokensize++;
	   if (tokensize >= 100) {
              line[index]=' ';
	      intoken=false;
	   }
	} else {
           intoken=false;
	}
     } else {
        if (isAlpha(line[index])) {
            tokensize=1;
	    intoken=true;
	}
     }
  }
}



std::string Strings::getOutoutFileName(size_t count) {
   std::string rval= "/output" + boost::lexical_cast<std::string>(count);
   return rval;
}

std::string Strings::newOutoutFileName(size_t *countp) {
   (*countp)++;
   std::string rval=getOutoutFileName(*countp);
   return rval;
}

bool Strings::needChop(char *line,size_t *outsize) {
  (*outsize) += strlen(line);
  if ((*outsize) >= mChopSize) 
     return true;
  return false;
}

void  Strings::processEvidence()
{
  FILE *command;
  char *line;
  size_t size;
  bool hascontent;
  char cline[1024];
  size_t outcount=0;
  size_t outsize=0;
  EvidenceStoreEntity * evidence = fetchEvidenceStoreObject();
  string of = getWorkDir() +  newOutoutFileName(&outcount);
  //string of = getWorkDir() + "/output";
  if (evidence == 0)
    throw OcfaException("Evidence without evidence store", this);
  vector <string> v;
  v.push_back("s");
  if (getJobArgument("endian")==string("big")) {
    getLogStream(LOG_DEBUG) << "BIG ENDIAN\n";
    v.push_back("b");
    v.push_back("B");
  } else {
    getLogStream(LOG_DEBUG) << "LITLE ENDIAN\n";
    v.push_back("l");
    v.push_back("L");
  }
  FILE *ofil=fopen(of.c_str(),"w");
  getLogStream(LOG_DEBUG) << " opened file "<< of << endl;
  if (ofil == 0) {
    throw OcfaException("Unable to open output file in own workdir",this);
  }
  hascontent=false;
  for (size_t index=0;index < v.size();index++) {
    snprintf(cline, 1023, "%s -e %s %s 2>/dev/null",BINARY_STRINGS,v[index].c_str(),evidence->getAsFilePath().c_str());
    cline[1023] = 0;
    getLogStream(LOG_DEBUG) << "CMD: " << cline << "\n";
    command = popen(cline, "r");
    if (command == 0)
      throw OcfaException("Unable to fork: /usr/bin/strings -e %s %s\n");
    size = 0;
    line = 0;
    while (getline(&line, &size, command) != -1) {

      getLogStream(LOG_DEBUG) << "retrieving line " << line << endl;
      if ((line != 0) && (size != 0)) {
        if (strlen(line) > 0) {
          hascontent=true;
	}
	tokenizerHack(line);
	getLogStream(LOG_DEBUG) << "fprinting line  " << endl;
        fprintf(ofil,"%s", line);
	if (needChop(line,&outsize)) {
	    fclose(ofil);
	    Evidence *derived = derive(getOutoutFileName(outcount), Scalar(getOutoutFileName(outcount)));
	    ScalarMetaValue value(Scalar("text/x-ocfa-strings"));
	    derived->getActiveJob()->setMeta("mimetype", &value);
	    ScalarMetaValue value2(Scalar("text"));
	    derived->getActiveJob()->setMeta("mimetop",&value2);
	    submitEvidence(derived);
            delete derived;	    
	    of = getWorkDir() +  newOutoutFileName(&outcount);
            ofil=fopen(of.c_str(),"w");
	    hascontent=false;
	    outsize=0;
	}
	getLogStream(LOG_DEBUG) << "Freeing line " << endl;
        free(line);
	line = 0;
      }
       else {
           getLogStream(LOG_ERR) << "getline returned strangeness \n"; 
      }
     if (line != 0){

	free(line);
      }
      line = 0;
      size = 0;
    }
    if (line) {

      free(line);
    }
    pclose(command);
  }
  fclose(ofil);
  Evidence *derived = derive(getOutoutFileName(outcount), Scalar(getOutoutFileName(outcount)));
  if (derived->getActiveJob() == 0){

    getLogStream(LOG_ERR) << "Cannot find an active job on a derived evidence " << endl;
  }
  if (hascontent) {
    ScalarMetaValue value(Scalar("text/x-ocfa-strings"));
    derived->getActiveJob()->setMeta("mimetype", 
  				   &value);
    ScalarMetaValue value2(Scalar("text"));
    derived->getActiveJob()->setMeta("mimetop",&value2);
  } else {
    ScalarMetaValue value(Scalar("application/x-empty"));
    derived->getActiveJob()->setMeta("mimetype",&value);
    ScalarMetaValue value2(Scalar("application"));
    derived->getActiveJob()->setMeta("mimetop",&value2);
  }
  submitEvidence(derived);
  delete evidence;
  delete derived;
}


int main(int , char *[])
{
  Strings *strings=0 ;
  try {
	  strings= new Strings();
  } catch(OcfaException &ex) {
	  ex.logWhat();
	  return 2;
  }
  try {
    strings->run();
  } catch(OcfaException &ex) {
    ex.logWhat();
    strings->getLogStream(LOG_CRIT) << "Ocfa Exeption cougth: " << ex.what() << "\n";
  } catch(...) {
    strings->getLogStream(LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  strings->PrintObjCount();
  delete strings;
  return 1;
}
