
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

#include <Pkr.hpp>
#include <stdexcept>
#include <vector>
using namespace ocfa::misc;
using namespace ocfa::evidence;
using namespace ocfa::store;
#include "../../../binaries.hpp"

void PkrExtractor::processEvidence()
{
  FILE *command;
  char *line;
  size_t size;
  char cline[1024];
  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject();
  if (evidence == 0)
    throw OcfaException("Evidence without evidence store", this);
  snprintf(cline, 1023, "%s --list-keys --with-colons --batch  --keyring=%s",
	   BINARY_GPG ,
	   evidence->getAsFilePath().c_str());
  delete evidence;
  cline[1023] = 0;
  command = popen(cline, "r");
  if (command == 0) {
     getLogStream(LOG_NOTICE) << "RETURN VAL != 0 for: gpg --list-keys --with-colons --batch  --keyring=" << evidence->getAsFilePath().c_str();
  }
  
  ArrayMetaValue *headers = new ArrayMetaValue(4);
  // why does ArrayMetavalue only accept reference. This is much more work!
  Scalar headerid("id");
  Scalar headerproto("proto");
  Scalar headerbits("bits");
  Scalar headerowner("owner");
  
  headers->addMetaValue(headerid);
  headers->addMetaValue(headerproto);
  headers->addMetaValue(headerbits);
  headers->addMetaValue(headerowner);
  TableMetaValue pubKeyList(&headers);
  size = 0;
  string owner = "";
  line=0;
  while ((feof(command) == 0) && (getline(&line, &size, command))) {
    int i = 0;
    int s = 0;
    vector <string> tokens;
    while (line[i] != 0) {
      if (line[i] == ':') {
	line[i] = 0;
        tokens.push_back(line+s);
	s=i+1;
      }
      i++;
    }
    if (tokens.size() >9) {
      logModule(LOG_INFO, "Valid line");
      if (tokens[0] == "pub") {
        owner=tokens[9]; 
      }  
      ArrayMetaValue *arrayMetaValue = new ArrayMetaValue(4);
      Scalar id(tokens[4]) ;
      Scalar proto(atoi(tokens[3].c_str())) ;
      Scalar bits(atoi(tokens[2].c_str()))  ;
      Scalar theOwner(owner) ;
      arrayMetaValue->addMetaValue(id);
      arrayMetaValue->addMetaValue(proto);
      arrayMetaValue->addMetaValue( bits);
      arrayMetaValue->addMetaValue(theOwner);
      pubKeyList.addRow(&arrayMetaValue);
    }
    free(line);
    line=0;
  }
  pclose(command);
  setMeta("pubkeylist", pubKeyList);
  return;
}

int main(int , char *[])
{
  PkrExtractor *pkr =0;
  try {
	  pkr=new PkrExtractor();
  } catch(OcfaException &ex) {
    ex.logWhat();
    return 2;
  }
  try {
    pkr->run();
  } catch(OcfaException &ex) {
    ex.logWhat() ;
  } catch(...) {
    pkr->getLogStream(LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  pkr->PrintObjCount();
  delete pkr;
  return 0;
}
