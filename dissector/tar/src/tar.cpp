
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
#include <tar.hpp>
#include <stdexcept>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include "../../../binaries.hpp"
using namespace ocfa::misc;
using namespace ocfa::evidence;
void  tarDissector::processEvidence()
{
  FILE *command;
  char *line;
  size_t size;
  int rval;
  char cline[1024];
  DIR * wdir;
  struct dirent *dirrec;
  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject();
  string wd = getWorkDir();
  if (evidence == 0)
    throw OcfaException("Evidence without evidence store", this);
  snprintf(cline, 1023, "%s -xf %s -C %s 2>&1",BINARY_TAR,
	   evidence->getAsFilePath().c_str(), wd.c_str());
  delete evidence;
  cline[1023] = 0;
  getLogStream(LOG_DEBUG) << "cmd: '" << cline << "'\n";
  command = popen(cline, "r");
  if (command == 0)
    throw OcfaException("Unable to fork: tar -xf %s -C %s\n");
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
      getLogStream(LOG_DEBUG) << "dir entry: " << subdir << "\n";
      if ((subdir != "/.")&&(subdir != "/..")) {
	getLogStream(LOG_DEBUG) << "Submitting " << subdir << "\n";
        Evidence *derived = derive(subdir, Scalar(subdir,"LATIN1"));
        submitEvidence(derived);
	delete derived;
      }
    }
  } else {
	  getLogStream(LOG_CRIT) << "Unable to open workingdir for reading\n";
  }
}

int main(int , char *[])
{
  tarDissector *tar=0;
  try {
    tar = new tarDissector();
  } catch(OcfaException &ex) {
    ex.logWhat();
    return 1;
  }
  try {
    tar->run();
  } catch(OcfaException &ex) {
    ex.logWhat();
    tar->getLogStream(LOG_CRIT) << "Ocfa Exeption cougth: " << ex.what() << "\n";
  } catch(...) {
    tar->getLogStream(LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  tar->PrintObjCount();
  delete tar;
  return 0;
}
