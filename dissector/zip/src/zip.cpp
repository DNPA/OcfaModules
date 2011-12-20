
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

#include <zip.hpp>
#include <stdexcept>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>
#include "../../../binaries.hpp"
using namespace ocfa::misc;
using namespace ocfa::evidence;
using namespace ocfa::facade;

zipDissector::zipDissector() : EvidenceDeriveAccessor("zip","default") {}

void zipDissector::processEvidence()
{
  FILE *command;
  char *line;
  size_t size;
  int rval=0;
  char cline[1024];
  DIR * wdir;
  struct dirent *dirrec;
  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject();
  string wd = getWorkDir();
  if (evidence == 0)
    throw OcfaException("Evidence without evidence store", this);
  snprintf(cline, 1023, "%s -P \"\" -n %s -d %s 2>&1",BINARY_UNZIP,
	   evidence->getAsFilePath().c_str(), wd.c_str());
  delete evidence;
  cline[1023] = 0;
  getLogStream(LOG_DEBUG) << "cmd: " << cline << "\n";
  command = popen(cline, "r");
  if (command == 0){
    string errstr = string("Unable to fork: ") + cline; 	  
    throw OcfaException("Unable to fork: unzip -P " " -n %s -d %s \n");
  }
  size = 0;
  line = 0;
  while (getline(&line, &size, command) != -1) {
    logEvidence(LOG_ERR,line);
    free(line);
    size = 0;
    line = 0;
  }
  rval=pclose(command);
  wdir=opendir(wd.c_str());
  if (wdir != 0) {
    while ((dirrec=readdir(wdir))) {
      string subdir=string("/") + string(dirrec->d_name);
      if ((subdir != "/.")&&(subdir != "/..")) {
	getLogStream(LOG_DEBUG) << "Submitting " << subdir << "\n";
        Evidence *derived = derive(subdir, Scalar(subdir,"LATIN1"));
        submitEvidence(derived);
	delete derived;
      }
    }
  } else {
    
    logModule(LOG_ERR, "could not open working directory");
    logEvidence(LOG_ERR, "could not open working directory");
  }
  
  if(rval == -1) {
     setMeta("brokenmodule", Scalar(-1));
  } else {
     switch (WEXITSTATUS(rval)) {
        case 0:
           setMeta("validformat", Scalar(1));
           break;
        case 1:
        case 82:
           setMeta("validformat", Scalar(1));
           setMeta("haspassword", Scalar(1));
           break;
        case 3:
           setMeta("validformat", Scalar(0));
           break;
        case 51:
           setMeta("incomplete", Scalar(1));
           break;
        default: 
           logModule(LOG_WARNING, string("unknown error processing ")
                 + getEvidence()->getEvidenceName().asASCII()) ;

           setMeta("brokenmodule", Scalar(1));
           logModule(LOG_ERR, string("unknown error: ") + Scalar(rval).asASCII());
 
     }
  }

}
int main(int , char *[])
{
  zipDissector *zip = 0;
  try {
    zip = new zipDissector();
  } catch(OcfaException &ex) {
	  ex.logWhat();
	  return 1;
  }
  try {
    zip->run();
  } catch(OcfaException &ex) {
    ex.logWhat();
    zip->getLogStream(LOG_CRIT) << "Ocfa Exeption cougth: " << ex.what() << "\n";
  } catch(...) {
    zip->getLogStream(LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  } 
  zip->PrintObjCount();
  delete zip;
  return 0;
}
