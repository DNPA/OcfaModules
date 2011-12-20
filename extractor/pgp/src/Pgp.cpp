
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

#include <store/EvidenceStoreEntity.hpp>        
#include <Pgp.hpp>
#include <stdexcept>
#include <vector>
#include "../../../binaries.hpp"
using namespace ocfa::store;
using namespace ocfa::evidence;
using namespace ocfa::misc;
void PgpExtractor::processEvidence() {
  FILE *command;
  char *line;
  size_t size;
  char cline[1024];

  EvidenceStoreEntity * evidence = fetchEvidenceStoreObject();
  if (evidence == 0) throw OcfaException("Evidence without evidence store",this);
  snprintf(cline,1023,"%s -v --list-packets --batch %s", BINARY_GPG  ,evidence->getAsFilePath().c_str());
  delete evidence;
  cline[1023]=0;
  command=popen(cline,"r");
  if (command==0) {
     getLogStream(LOG_NOTICE) << "RETURN VAL != 0 for: gpg -v --list-packets --batch " << evidence->getAsFilePath().c_str();
  }
  size=0;
  line=0;
  ArrayMetaValue keyIds;
  ArrayMetaValue signatures;
  while (getline(&line,&size,command) > 0) {
    vector <string> tokens;
    int i=0;
    int  s=0;
    int p=0;
    while (line[i] != 0) {
	if (line[i] == ' ') {
          p=i+1;
	}	
	if (line[i] == '\n') {
           line[i]=0;
	}
        if ((line[i] == ':')||(line[i] == ',')) {
	    line[i] = 0;
	    tokens.push_back(line+s);
	    s=i+1;
	}
        i++;			      
    }
    if ((tokens.size() > 1) &&  tokens[1] == "pubkey enc packet") {
      Scalar keyid(line + p);
      keyIds.addMetaValue(keyid);
    }
    if ((tokens.size() > 1) &&  tokens[1] == "signature packet") {
      Scalar signature(line+p);
      signatures.addMetaValue(signature);
    }
    free(line);
    line=0;
    size=0;
  }
  pclose(command);
  setMeta("keyid", keyIds);
  setMeta("signature", signatures);
  
}

int main(int , char *[]){
  PgpExtractor *pgp =0;
  try {
	  pgp=new PgpExtractor();
  } catch(OcfaException &ex) {
    ex.logWhat();
    return 2;
  }
  try {
     pgp->run();
  } catch(OcfaException &ex) {
    pgp->getLogStream(LOG_CRIT) << "Ocfa Exeption cougth: " << ex.what() << "\n";
  } catch(...) {
    pgp->getLogStream(LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  pgp->PrintObjCount();
  delete pgp;
  return 1;
}

