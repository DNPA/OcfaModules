
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

#include <gzip.hpp>
#include <stdexcept>
#include <vector>
#include <errno.h>
#include<string.h>
#include "../../../binaries.hpp"
using namespace ocfa::misc;
using namespace ocfa::evidence;
void gzipDissector::processEvidence()
{
  char cline[1024];

  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject();
  string wd = getWorkDir();
  string infil= wd + "/output.gz";
  if (evidence == 0)
    throw OcfaException("Evidence without evidence store", this);

  if (symlink(evidence->getAsFilePath().c_str(),infil.c_str()) != 0){
    getLogStream(LOG_CRIT) << "Symlink " << infil.c_str() << " to "  << evidence->getAsFilePath().c_str() << " failed."
	 << "errstr: " << strerror(errno) << endl;
    delete evidence;
    return;
  } 
  delete evidence;
  snprintf(cline, 1023, "%s -f %s 2>/dev/null",BINARY_GUNZIP, infil.c_str());
  cline[1023] = 0;
  if (system(cline) == -1) {
     unlink(infil.c_str());
     getLogStream(LOG_ERR) << "call of command failed: " << cline << "\n";
     return;
  }
  if (unlink(infil.c_str()) == -1) { 
    // if this fails we know that gunzip did the job because it will remove
    // the sourcefile. If linking failed in the first place we would not have
    // arrived here.
    Evidence *derived = derive("/output", Scalar("output"));
    submitEvidence(derived);
    delete derived;
    setMeta("validformat", Scalar(1));
    return;
  } else {
    setMeta("validformat", Scalar(0)); 
    return;
  }
}

int main(int , char *[])
{
  //Ocfa::setDebug();
  gzipDissector *gzip=0;
  try {
    gzip = new gzipDissector();
  } catch(OcfaException &ex) {
    ex.logWhat();
    return 1;
  }
  try {
    gzip->run();
  } catch(OcfaException &ex) {
    ex.logWhat();
    gzip->getLogStream(LOG_CRIT) << "Ocfa Exeption cougth: " << ex.what() << "\n";
  } catch(...) {
    gzip->getLogStream(LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  gzip->PrintObjCount();
  delete gzip;
  return 0;
}
