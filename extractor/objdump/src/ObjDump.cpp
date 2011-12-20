
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

#include <ObjDump.hpp>
#include <stdexcept>
#include <vector>
using namespace ocfa::misc;
using namespace ocfa::evidence;
using namespace ocfa::store;
#include "../../../binaries.hpp"

void ObjDumpExtractor::processEvidence()
{
  FILE *command;
  char *line;
  size_t size;
  char cline[1024];
  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject();
  if (evidence == 0)
    throw OcfaException("Evidence without evidence store", this);
  snprintf(cline, 1023, "%s  -b efi-app-ia32 -h %s",
	   BINARY_OBJDUMP ,
	   evidence->getAsFilePath().c_str());
  delete evidence;
  cline[1023] = 0;
  command = popen(cline, "r");
  if (command == 0)
    throw
      OcfaException
      ("Unable to fork: objdump -h <filename>\n");
  ArrayMetaValue sectionNames;  
  size = 0;
  line=0;
  bool packed=0;
  bool winzip=0;
  bool brokensegname=false;
  while ((feof(command) == 0) && (getline(&line, &size, command))) {
      if (line) {
        if (strlen(line) > 20) {
          if (line[4] == ' ') {
            if (strstr(line,"CODE")) {
               if (! ( strstr(line,"READONLY"))) {
                  packed=1;
	       }
	    }
	  } else if (line[0] == ' ') {
            char segname[15];
	    for (int x=0;x<15;x++) {
	      segname[x]=0;
	    }
	    strncpy(segname,line+4,14);
	    for (int x=0;x<15;x++) {
               if (segname[x]==' ') {
                  segname[x]=0;
	       }
               //hack to fix invalid xml problem with controll characters.
               if ((segname[x]) && ((segname[x] < 32)||(segname[x] > 126 ))) {
                  segname[x]='_';
                  brokensegname=true;
               }
               
	    }
            if (strcmp(segname,"_winzip_")==0) {
               winzip=1;
            }
	    Scalar SegName(segname,"LATIN1");
	    sectionNames.addMetaValue(SegName);
	  }
	}
      }
  }
  pclose(command);
  setMeta(string("sectionnames"), sectionNames);
  if (packed) {
     ScalarMetaValue paCked(Scalar(1));
     setMeta(string("packed"), paCked);
  }
  if (winzip) {
     ScalarMetaValue winZipped(Scalar("application/x-zip"));
     setMeta(string("mimetype"), winZipped);
  }
  if (brokensegname) {
      ScalarMetaValue inv(Scalar(1));
      setMeta(string("invalidsegname"),inv);
  }
  return;
}

int main(int , char *[])
{
  ObjDumpExtractor *pkr =0;
  try {
	  pkr=new ObjDumpExtractor();
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
