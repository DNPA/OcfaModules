
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

#include <qemuimg.hpp>
#include <stdexcept>
#include <vector>
#include <errno.h>
#include<string.h>
#include "../../../binaries.hpp"
using namespace ocfa::misc;
using namespace ocfa::evidence;
void qemuimgDissector::processEvidence()
{
  char cline[1024];

  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject();
  string wd = getWorkDir();
  if (evidence == 0)
    throw OcfaException("Evidence without evidence store", this);
  std::string infil=evidence->getAsFilePath();
  delete evidence;

  snprintf(cline, 1023, "%s convert -O raw %s %s/output.dd 2>/dev/null",BINARY_QEMUIMG, infil.c_str(),wd.c_str());
  cline[1023] = 0;
  if (system(cline) == -1) {
     getLogStream(LOG_ERR) << "call of command failed: " << cline << "\n";
     return;
  }
  //FIXME: check if output.dd exists.
  Evidence *derived = derive("/output.dd", Scalar("output.dd"));
  submitEvidence(derived);
  delete derived;
  return;
}

int main(int , char *[])
{
  //Ocfa::setDebug();
  qemuimgDissector *qemuimg=0;
  try {
    qemuimg = new qemuimgDissector();
  } catch(OcfaException &ex) {
    ex.logWhat();
    return 1;
  }
  try {
    qemuimg->run();
  } catch(OcfaException &ex) {
    ex.logWhat();
    qemuimg->getLogStream(LOG_CRIT) << "Ocfa Exeption cougth: " << ex.what() << "\n";
  } catch(...) {
    qemuimg->getLogStream(LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  qemuimg->PrintObjCount();
  delete qemuimg;
  return 0;
}
