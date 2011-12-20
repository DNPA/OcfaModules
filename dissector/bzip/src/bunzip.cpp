
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



#include <ocfa.hpp>
#include <bunzip.hpp>
#include <stdexcept>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include "../../../binaries.hpp"

using namespace ocfa;
using namespace evidence;
void bunzipDissector::processEvidence()
{
  FILE *command;
  char *line;
  size_t size;
  char cline[1024];

  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject();
  string wd = getWorkDir();
  if (evidence == 0)
    throw OcfaException("Evidence without evidence store", this);


  string EvidenceBasename = store::Filename(evidence->getAsFilePath()).getBasename();
  // copy evidence file to working directory
  char workfile[1024];
  snprintf(workfile, 1023, "%s/%s", wd.c_str(), EvidenceBasename.c_str());
  snprintf(cline, 1023,"cp -L %s %s",evidence->getAsFilePath().c_str(), workfile);
  cline[1023] = 0;
  getLogStream(LOG_DEBUG) << "Copying evidence to workfile. Cmd = " << cline << endl;
  system(cline);
  chmod(workfile, 0666);
 
  
  snprintf(cline, 1023, "%s %s", BINARY_BUNZIP2 , workfile);
  cline[1023] = 0;

  command = popen(cline, "r");
  if (command == 0)
    throw OcfaException("Unable to fork: bunzip %s \n");
  size = 0;
  line = 0;
  while (getline(&line, &size, command) != -1) {

    free(line);
    line = 0;
    size = 0;
  }
  pclose(command);


  // NOTE: by default bunzip will clean up after us, and only resulting file will be left in workdir
  string outname = EvidenceBasename + string(".out");
  Evidence *derived = derive(outname, Scalar("output","LATIN1"));
  submitEvidence(derived);
  delete derived;

  return;
}
int main(int , char **)
{
  
  bunzipDissector *bunzip = 0;
  try {
    bunzip = new bunzipDissector();
  } catch(OcfaException &ex) {
    ex.logWhat();
    return 2;
  }
  try {
    bunzip->run();
  }
  catch(OcfaException &ex) {
    ex.logWhat();
    bunzip->getLogStream(LOG_CRIT) << "Ocfa Exeption cougth: " << ex.what() << "\n";
  }
  catch(...) {
    bunzip->getLogStream(LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  bunzip->PrintObjCount();
  delete bunzip;
  return 1;
}
