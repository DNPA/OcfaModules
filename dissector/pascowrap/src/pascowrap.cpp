
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

#include "pascowrap.hpp"
#include <stdexcept>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <store/EvidenceStoreEntity.hpp>
// if you wrap existing executables it might be wise to put them in the include file below
#include "../../../binaries.hpp"
using namespace ocfa::facade;
using namespace ocfa::evidence;
using namespace ocfa::misc;
//using namespace ocfa::evidence;
//using namespace ocfa::store;

Pascowrap::Pascowrap():EvidenceDeriveAccessor("pascowrap","default")
{
    // put thirdparty-library initializations here ...
}

Pascowrap::~Pascowrap()
{
    // clean up here ...
}

void  Pascowrap::processEvidence()
{
  char 		cline[1024];
  FILE 		*fpCommand;
  size_t 	size 	;
  char  	*line;
  int   	rval = 0;

  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject();
  string filename = evidence->getAsFilePath();
  string wd=getWorkDir();
  getLogStream(LOG_DEBUG) << "Processing " << filename << endl;

   // do whatever you want to do with the evidence

   // your workdir can be found by getWorkDir()
   string filepad = getWorkDir() + "/extractedUrls";

   snprintf(cline, 1023, "%s %s > %s",BINARY_PASCO, filename.c_str(),filepad.c_str());

   cline[1023] = 0;

   getLogStream(LOG_INFO) << "cmd: '" << cline << endl;
   
     size = 0;
     line = 0;

     if( (fpCommand = popen(cline, "r") ) != NULL) {
        while(getline(&line, &size, fpCommand) !=-1){
           /* read while not end of pipe */
	   if(line)
   	   	free(line);
	   size = 0;
	   line = 0;
           
        };
        rval = pclose(fpCommand);

        if(rval == -1) {
	   getLogStream(LOG_WARNING) << "ERROR return value pclose" ;
           //setMeta("brokenmodule", Scalar(-1));
        }
     }
     else {
        getLogStream(LOG_WARNING) << "Not able to spawn " << cline << "\n";
     }




  Evidence *derived= derive("extractedUrls",Scalar("extractedUrls"));
  ScalarMetaValue mimetype(Scalar("text/plain"));
  ScalarMetaValue mimetop(Scalar("text"));
  ScalarMetaValue encoding(Scalar("utf-8"));
  derived->getActiveJob()->setMeta("mimetype", &mimetype);
  derived->getActiveJob()->setMeta("mimetop", &mimetop);
  derived->getActiveJob()->setMeta("charset", &encoding);
  submitEvidence(derived);
  delete derived;
  //delete evidence;
}


int main(int , char *[])
{
  Pascowrap *pPascowrap;
  try {
    pPascowrap = new Pascowrap();
    pPascowrap->run();
  } catch(OcfaException &ex) {
    ex.logWhat();
    pPascowrap->getLogStream(LOG_CRIT) << "Ocfa Exeption cougth: " << ex.what() << "\n";
  } catch(...) {
    pPascowrap->getLogStream(LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  delete pPascowrap;
  return 0;
}
