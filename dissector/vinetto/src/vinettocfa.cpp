
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

#include "vinettocfa.hpp"
#include <stdexcept>
#include <vector>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <store/EvidenceStoreEntity.hpp>
// if you wrap existing executables it might be wise to put them in the include file below
#include "../../../binaries.hpp"
using namespace ocfa;
using namespace ocfa::evidence;
using namespace ocfa::misc;
using namespace ocfa::evidence;
using namespace ocfa::store;

Vinettocfa::Vinettocfa():EvidenceDeriveAccessor("vinettocfa","default")
{
    // put thirdparty-library initializations here ...
}

Vinettocfa::~Vinettocfa()
{
    // clean up here ...
}

void  Vinettocfa::processEvidence()
{
  EvidenceStoreEntity * evidence = fetchEvidenceStoreObject();
  string filename = evidence->getAsFilePath();
  getLogStream(LOG_NOTICE) << "Processing " << filename << endl;

   // do whatever you want to do with the evidence
   // your workdir can be found by getWorkDir()
   string workdir = getWorkDir();
   string outputdir = workdir + string("/ExtractedImages");
   mkdir(outputdir.c_str(), 0755);
   string cmd = string(BINARY_VINETTO) + string(" ") + filename + string(" -o ") + outputdir;
   getLogStream(LOG_NOTICE) << "Vinetto cmd: " <<  cmd << endl;  
   FILE *f = popen(cmd.c_str(), "r");
   pclose(f); 

   // Store metadata about the evidence, e.g.
   // setMeta("MetadataName", MetadataValue);

   // get a handle on the derived evidence (assuming the evidence filename is stored in derivedfilename) 

   Evidence *derived = derive("ExtractedImages", Scalar("ExtractedImages"));

   // optionally, add metadata about the derived evidence, if any 
   // ScalarMetaValue value(Scalar("text/x-ocfa-strings"));
   // derived->getActiveJob()->setMeta("mimetype", &value);

   // finally, submit the derived evidence to the router  
   submitEvidence(derived);
   // you need to delete the created objects

   delete derived;
   delete evidence;
}


int main(int , char *[])
{
  Vinettocfa *pVinettocfa;
  try {
    pVinettocfa = new Vinettocfa();
    pVinettocfa->run();
  } catch(OcfaException &ex) {
    ex.logWhat();
    pVinettocfa->getLogStream(LOG_CRIT) << "Ocfa Exeption cougth: " << ex.what() << "\n";
  } catch(...) {
    pVinettocfa->getLogStream(LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  delete pVinettocfa;
  return 0;
}
