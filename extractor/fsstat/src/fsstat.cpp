
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

#include "../inc/fsstat.hpp"
#include "../../../binaries.hpp"
#include <boost/tokenizer.hpp>
#include <string>
using namespace std;
using namespace ocfa::misc;
using namespace ocfa::evidence;

FsStat::FsStat() 
         : ocfa::facade::EvidenceFileAccessor("fsstat","default"),
	   szCommand(BINARY_FSSTAT),
           mFsmetamode(true) {

  if(access(szCommand.c_str(),X_OK)!=0) {
	
     ocfaLog(LOG_ERR,"fsstat executable not found on system");
     throw OcfaException("fsstat executable not found on system", this);
  }
}

FsStat::~FsStat() {
}

void FsStat::processEvidence(){
  string         szDoIt;
  FILE          *fpFsStat;
  char           szLine[MAX_LINE_LENGTH];
  int            rval=0; //return value of pclose
  mFsmetamode=true; 
  ocfa::store::EvidenceStoreEntity* evidence = fetchEvidenceStoreObject();
  if(evidence != 0) {
     string szFilePath = evidence->getAsFilePath();
     szDoIt = szCommand + " " + szFilePath + " 2>&1";
     bool nolines=true;
     if( (fpFsStat = popen(szDoIt.c_str(), "r") ) != NULL) {
        while(fgets(szLine, MAX_LINE_LENGTH-1, fpFsStat) !=NULL){ 
	   /* read while not end of pipe */
           processLine(szLine);
           nolines=false;
        };
        rval = pclose(fpFsStat);
     }
     else {
        getLogStream(LOG_ERR) << "Not able to spawn " << szDoIt << "\n";
     }
     delete evidence;
  } //evidence!=0
  else {
     getLogStream(LOG_ERR) << "No evidence found" ;
  }
} //processEvidence


void FsStat::processLine(char *inLine)
{
  if (strncmp(inLine,"Cannot determine file system type (",35) == 0) {
     setMeta("FileSystemType", ocfa::misc::Scalar("multi"));
     setMeta("fsstatinfo", ocfa::misc::Scalar(inLine));
     return;
  }
  std::string fulline(inLine);
  if (fulline.find("METADATA INFORMATION") != std::string::npos) {
      mFsmetamode=false;
  }
  if ((mFsmetamode) && (fulline.find(":") != std::string::npos)) { 
    boost::char_separator<char> sep(":\r\n");
    boost::tokenizer<boost::char_separator<char> > tokens(fulline, sep);
    size_t tokenno=0;
    std::string name;
    std::string value;
    for (boost::tokenizer<boost::char_separator<char> >::iterator tok_iter = tokens.begin();
       tok_iter != tokens.end(); ++tok_iter) {
      if (tokenno == 0) {
        name=*tok_iter;
      }
      if (tokenno == 1) {
        value=*tok_iter;
      }
      tokenno++;
    }
    std::string strippedkey;
    if (name != "") {
      boost::char_separator<char> sep2(" \t");
      boost::tokenizer<boost::char_separator<char> > tokens2(name, sep2);
      for (boost::tokenizer<boost::char_separator<char> >::iterator tok_iter2 = tokens2.begin();
         tok_iter2 != tokens2.end(); ++tok_iter2) {
         strippedkey += *tok_iter2;
      }
    }
    while ((value.length()>1) && ((value.c_str()[0] == ' ') || (value.c_str()[0] == '\t'))) {
        value.erase(0,1);
    }
    while((value.length()>1) && ((value.c_str()[value.length()-1] == ' ') || (value.c_str()[value.length()-1] == '\t'))) {
        value.erase(value.length()-1,1);
    }
    if ((strippedkey != "") && (value != "") && (strippedkey.c_str()[0] != '*')) {       
       setMeta(strippedkey, ocfa::misc::Scalar(value,"LATIN1"));
    }
  }
}

int main(int, char *[]) {
  FsStat *fsstatmodule=0;
  try {
     fsstatmodule = new FsStat();
     fsstatmodule->run();
  }
  catch (OcfaException &ex){

    ex.logWhat();
    ocfa::misc::OcfaLogger::Instance()->syslog(LOG_ERR, "ERROR") << ex.what() << endl;

 }	
  catch(...) {
    ocfa::misc::OcfaLogger::Instance()->syslog(LOG_ERR, "ERROR") << "Unexpected exception!\n";
  }
  delete fsstatmodule;
  return 1;

}

