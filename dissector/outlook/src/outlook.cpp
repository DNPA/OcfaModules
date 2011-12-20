
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

//#include <ocfa.hpp>
#include <stdexcept>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <fstream>

#include "outlook.hpp"

using namespace ocfa::facade;
using namespace ocfa::evidence;
using namespace ocfa::misc;

outlookDissector::outlookDissector() : EvidenceDeriveAccessor("outlook","default"),d_outlookworkdir(""), d_outlookresultdir(""){

  d_outlookworkdir = getConfEntry("outlookworkdir");
  d_outlookresultdir = getConfEntry("outlookresultdir");
  d_outlookinfile = getConfEntry("outlookinfile");

  if (d_outlookworkdir == "" || d_outlookresultdir == "" || d_outlookinfile == ""){
    throw OcfaException("Conf-error: outlookorkdir, outlookresultdir or outlookinfile missing",this);
  }
  getLogStream(LOG_DEBUG) << "Workdir " << d_outlookworkdir << endl; 

} 

void outlookDissector::processEvidence()
{
  char cline[1024];
  DIR * wdir = 0;
  struct dirent *dirrec = 0;
  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject();
  
  if (evidence == 0)
    throw OcfaException("Evidence without evidence store", this);
  
  // copy evidence file to working directory
  string EvidenceBasename = ocfa::store::Filename(evidence->getAsFilePath()).getBasename();
  char workfile[1024];
  snprintf(workfile, 1023, "%s/%s", d_outlookworkdir.c_str(), EvidenceBasename.c_str());
  snprintf(cline, 1023,"cp %s %s",evidence->getAsFilePath().c_str(), workfile);
  cline[1023] = 0;
  getLogStream(LOG_DEBUG) << "Copying evidence to workfile. Cmd = " << cline << endl;
  system(cline);
  chmod(workfile, 0666);
  
  // Create or truncate outlook.in for writing
  ofstream out_file;
  string outfilename = d_outlookworkdir +  "/" + d_outlookinfile;
  getLogStream(LOG_DEBUG) << "Evidence list file for Thormodule: " << outfilename << endl; 
  out_file.open(outfilename.c_str(), ios::out|ios::trunc);

  if (out_file.bad() ) {
    throw OcfaException("Unable to write to file outlook.in \n");
  }
  //write outlook source file to outlook infile
  out_file << EvidenceBasename;
  out_file.close() ;
 
  // Wait for file outlook.in to disappear. This should indicate the Thor module
  // finished unpacking the outlook file. 
  int i = 0;
  while (fileExists(outfilename)){ 
    //do nothing
    sleep(1);
    if ((++i % 10) == 0){
         aliveAndKicking(0,0);
	 getLogStream(LOG_DEBUG) << "Waiting " << i << " secs for outlook.in to disappear." << endl;
       }
  }

    // move the files generated by outlok module to workdirectory of this module
    sprintf(cline, "mv %s/* %s", d_outlookresultdir.c_str(), getWorkDir().c_str());
    getLogStream(LOG_DEBUG) << "Copying results files back to working directory. Cmd = " << cline  << endl;
    system(cline);
    // remove output directory to be sure nothing is left for the next time.
    if (rmdir(d_outlookresultdir.c_str()) != 0){
      //rmdir returned "not succesfull"
      // if for some reason the outut dir was not created, mark it as an error.
      if (!fileExists(d_outlookresultdir)){
	
	getLogStream(LOG_ERR) << " could not removde output dir because it did not exist " << endl;
	logEvidence(LOG_ERR, "not output dir found!");
      }
      else {
	// couldn't removce output directory. This means following rounds
	// are garanteed to deliver corrupt data.
	logEvidence(LOG_CRIT, d_outlookresultdir + "couldn be removed, thus, killing the module");
	getLogStream(LOG_CRIT) << "Cannot remove " << d_outlookresultdir << endl;
	throw OcfaException("Unable to remove d_thorresultdir, consult error log",this);
      }
    }

  // Submit all files as evidence from the working directory
  wdir=opendir(getWorkDir().c_str());
  if (wdir != 0) {
    while ((dirrec=readdir(wdir))) {
      string subdir=string("/") + string(dirrec->d_name);
      if ((subdir != "/.")&&(subdir != "/..")) {
	getLogStream(LOG_DEBUG) << "Submitting " << subdir << endl;
        Evidence *derived = derive(subdir, Scalar(subdir,"LATIN1"));
        submitEvidence(derived);
	delete derived;
      }
    }
  closedir(wdir);
  } else {
    throw OcfaException("Could not read result directory for submitting new evidence.", this);
  }

  delete evidence;

}

bool outlookDissector::fileExists(const string filename) const {
  struct stat buf;
  if (stat(filename.c_str(),&buf) == 0){
    return(S_ISREG(buf.st_mode) != 0);
  }
  else
    return false;
}


int main(int , char *[])
{
  outlookDissector *outlook = 0; 
  try {
    outlook=new outlookDissector();
  } catch(OcfaException &ex) {
	  ex.logWhat();
	  cerr << "Ocfa Exeption cougth in module constructor: " << ex.what() << "\n";
	  return 2;
  }
  try {
    outlook->run();
  } catch(OcfaException &ex) {
    ex.logWhat();
    outlook->getLogStream(LOG_CRIT) << "Ocfa Exeption cougth: " << ex.what() << "\n";
  } catch(...) {
    outlook->getLogStream(LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  outlook->PrintObjCount();
  delete outlook;
  return 1;
}
