
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

#include <7z.hpp>
#include <stdexcept>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include "../../../binaries.hpp"
using namespace ocfa::misc;
using namespace ocfa::evidence;
using namespace ocfa::facade;

zipDissector::zipDissector() : EvidenceDeriveAccessor("7z","default") {}

void zipDissector::processEvidence()
{
  FILE *command;
  char *line;
  size_t size;
  int rval;
  char cline[1024];
  DIR * wdir;
  struct dirent *dirrec;
  string strInput="";
  int bLinked = 0;

  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject();
  string wd = getWorkDir() + "/";
  if (evidence == 0) {
    throw OcfaException("Evidence without evidence store", this);
  }

  /* Fix nasty 7z bug on filenames ending with 'aa' */
  if(evidence->getAsFilePath().substr(evidence->getAsFilePath().length()-2,2) == "aa") {
      strInput = wd + string("bugfixxedlink");
      /*printf("JOCHEN_7z_DEBUG::AA-einde::%s\n",evidence->getAsFilePath().c_str());*/
      if(link(evidence->getAsFilePath().c_str(),strInput.c_str())!=0) {
        throw OcfaException("Unable to evidence path to bugfixxedlink\n");
        return;

      }
      bLinked = 1;
  } else {
    strInput = evidence->getAsFilePath();
  }


  /* Construct command line for popen */
  snprintf(cline, 1023, "%s e -o%s %s 2>&1",BINARY_7Z,
           wd.c_str(),strInput.c_str());
  cline[1023] = 0;
  getLogStream(LOG_DEBUG) << "cmd: " << cline << "\n";
  delete evidence;

  /* Try to open pipe to command string 7z */
  if ( (command = popen(cline, "r")) == 0){
    string errstr = string("Unable to fork: ") + cline; 	  
    throw OcfaException("Unable to fork: 7z e -o workdir\n");
    return;
  }

  /* read the stdout from the commandstring */
  size = 0;
  line = 0;
  while (getline(&line, &size, command) != -1) {
    if((line != 0) && (size !=0)) {
       logEvidence(LOG_ERR,line);
       /*	printf("JOCHEN_7z_DEBUG::%s",line);*/
       free(line);
    }
    size = 0;
    line = 0;
  }

  /* Try close pipe to command string */
  rval=-1;
  if( (rval=pclose(command)) == -1) {
	throw OcfaException("Unable to close pipe to 7z command");
	return;
  }

  if(bLinked) {
     if(unlink(strInput.c_str())!=0) {
	logEvidence(LOG_ERR,"Unable to unlink temp link 7z");
     }
  }

  /* Try to derive evidence from unzipped content */
  wdir=opendir(wd.c_str());
  if (wdir != 0) {
    /* Treewalk throught result dir */
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
    /* Unable to open working dir */
    logModule(LOG_ERR, "could not open working directory");
    logEvidence(LOG_ERR, "could not open working directory");
  }

  /* Process return value from command string*/
  switch (WEXITSTATUS(rval)) {
     case 0:
	     setMeta("validformat", Scalar(1));
             break;
     case 1:
	     setMeta("validformat", Scalar(1));
	     setMeta("Warning", Scalar("Some files not read from archive"));
	     logEvidence(LOG_WARNING, "Some files not read from archive");
	     break;
     case 2:
	     setMeta("validformat", Scalar(0));
	     setMeta("Error", Scalar("Fatal Error","LATIN1"));
	     logEvidence(LOG_ERR, "Fatal Error");
	     break;
     case 7:
	     setMeta("validformat", Scalar(0));
	     setMeta("Error", Scalar("Bad command line parameters","LATIN1"));
	     logEvidence(LOG_ERR, "Bad command line parameters");
	     break;
     case 8:
	     setMeta("validformat", Scalar(0));
	     setMeta("Error",Scalar("Not enought memory for operation","LATIN1"));
	     logEvidence(LOG_ERR, "Not enought memory for operation");
     case 255:
	     setMeta("validformat", Scalar(0));
	     setMeta("Error",Scalar("User stopped process with ctrl-C","LATIN1"));
             logEvidence(LOG_ERR, "User stopped process with ctrl-C");
     default: 
	     setMeta("validformat", Scalar(0));
             setMeta("Error",Scalar("unknown error processing 7z","LATIN1"));
             logModule(LOG_WARNING, string("unknown error processing")
		 + getEvidence()->getEvidenceName().asASCII()) ;
       
             logEvidence(LOG_ERR, string("unknown error: ") + Scalar(rval).asASCII());
 
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
