
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
#include <boost/tokenizer.hpp>
#include <fstream>
#include "../../../binaries.hpp"
#include "ExcelDissector.hpp"
using namespace ocfa;
using namespace ocfa::misc;
using namespace evidence;

void  ExcelDissector::processEvidence()
{
  FILE *command;
  char *line;
  size_t size;
  int rval;
  char cline[1024];
  DIR * wdir;
  
  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject();
  string wd = getWorkDir();
  getLogStream(LOG_DEBUG) << "workdir = " << wd << endl;
  if (evidence == 0)
    throw OcfaException("Evidence without evidence store", this);
  chdir(wd.c_str());
  snprintf(cline, 1023, "%s %s/sbin/exceldissect.pl %s 2>&1",BINARY_PERL,OcfaConfig::Instance()->getValue("ocfaroot").c_str(), evidence->getAsFilePath().c_str());
  delete evidence;
  cline[1023] = 0;
  
  getLogStream(LOG_INFO) << "cmd: '" << cline << endl;
  command = popen(cline, "r");
  if (command == 0)
    throw OcfaException("Unable to fork: sbin/exceldissect.pl %s\n");
  size = 0;
  line = 0;
  while (getline(&line, &size, command) != -1) {
    logEvidence(LOG_ERR,line);
    free(line);
    size = 0;
    line = 0;
  }
  rval=pclose(command);

  string szMetadata = getWorkDir() + "/metadata";
  getLogStream(LOG_INFO) << "metadatais " << szMetadata.c_str() << endl;
  ifstream metaDataStream(szMetadata.c_str());

  string theLine;
  
  boost::char_separator<char> sep ("=");
  while (!metaDataStream.fail() && !metaDataStream.eof()){

    char metaname[100];
    char metavalue[1000];
    getline(metaDataStream, theLine);
    boost::tokenizer<boost::char_separator<char> > tok(theLine, sep);
    boost::tokenizer<boost::char_separator<char> >::iterator iter = tok.begin();
    if (iter != tok.end()){

      string metaName = *iter;
      string metaValue = "";
      iter++;
      if (iter != tok.end()){

	metaValue = *iter;
      }
      setMeta(metaName, Scalar(metaValue));      
    }
  }
  metaDataStream.close();
  unlink(szMetadata.c_str());
  wdir=opendir(wd.c_str());
  dirent *fileEntry;
  if (wdir != 0) {
    while ((fileEntry=readdir(wdir))) {
      
      string fileName=string("/") + string(fileEntry->d_name);
      getLogStream(LOG_DEBUG) << "dir entry: " << fileName << "\n";
      if ((fileName != "/.")&&(fileName != "/..")) {
 	 getLogStream(LOG_DEBUG) << "Submitting " << fileName << "\n";
          Evidence *derived = derive(fileName, Scalar(fileName,"LATIN1"));
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
  ExcelDissector *excel=0;
  try {
    excel = new ExcelDissector();
  } catch(OcfaException &ex) {
    ex.logWhat();
    return 2;
  }
  try {
    excel->run();
  } catch(OcfaException &ex) {
    ex.logWhat();
    excel->getLogStream(LOG_CRIT) << "Ocfa Exeption cougth: " << ex.what() << "\n";
  } catch(...) {
    excel->getLogStream(LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  excel->PrintObjCount();
  delete excel;
  return 1;
}
