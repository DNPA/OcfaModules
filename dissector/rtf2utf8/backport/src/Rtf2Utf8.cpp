
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
#include <rtf2utf8.hpp>
#include <stdexcept>
#include <vector>
using namespace ocfa;
using namespace evidence;
int Rtf2Utf8Dissector::processEvidence() {
  FILE *command;
  char *line;
  size_t size;
  char cline[1024];

  ocfa::store::EvidenceStoreEntity * evidence = createEvidenceStoreObject();
  string wd=getWorkDir();  
  if (evidence == 0) {

    throw OcfaException("Evidence without evidence store",this);
  }
  
  snprintf(cline,1023,"java -cp $OCFAROOT/tools/ ProcessGuardian RTF2SFM.plx -o %s/output %s",wd.c_str(), evidence->getAsFilePath().c_str());
  cline[1023]=0;

  command=popen(cline,"r");
  if (command==0){ 
    throw OcfaException("Unable to fork: RTF2SFM.plx %s \n");
  }
  
  size=0;
  line=0;
  while (getline(&line,&size,command) != -1) {

    free(line);
    line=0;
    size=0;
  }
  pclose(command);

  DerivedEvidence *derived=derive("output",Scalar("output"));
  derived->pushBackMeta("encoding",Scalar("utf-8"));
  derived->submit();
  delete derived;

  return 1;
}
void Rtf2Utf8Dissector::processMessage(msg::Message & imsg){
  
}
int main(int argc, char *argv[]){
  Rtf2Utf8Dissector *module=0;
  try {
     
    module = new Rtf2Utf8Dissector();
  
  } catch(OcfaException ex) {
    cerr <<  "Rtf2Utf8:Ocfa Exeption cougth in module construction: " << ex.what() << "\n";
    return 2;
  }
  try {
     module->run();
  } catch(OcfaException ex) {
    module->syslog(LOG_CRIT) << "Ocfa Exeption cougth: " << ex.what() << "\n";
  } catch(...) {
    module->syslog(LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  module->PrintObjCount();
  delete module ;
  return 1;
}

