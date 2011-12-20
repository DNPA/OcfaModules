
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

#include "../inc/marktag.hpp"
#include "../../../binaries.hpp"
using namespace std;
using namespace ocfa::misc;
using namespace ocfa::evidence;

/**
 * MarkTag constructor
 */
MarkTag::MarkTag() 
         : ocfa::facade::EvidenceFileAccessor("marktag","default") {

}

/**
 * MarkTag destructor
 */
MarkTag::~MarkTag() {
}

/**
 * MarkTag processEvidence
 */
void MarkTag::processEvidence(){
  
  ocfa::store::EvidenceStoreEntity* evidence = fetchEvidenceStoreObject();
  if(evidence != 0) {
     setMeta("tag", ocfa::misc::Scalar("set"));
     delete evidence;
  } //evidence!=0
  else {
     getLogStream(LOG_WARNING) << "No evidence found" ;
  }
} //processEvidence



int main(int, char *[]) {
  MarkTag *e=0;
  try {
     e = new MarkTag();
     e->run();
  }
  catch (OcfaException &ex){

    ex.logWhat();
    ocfa::misc::OcfaLogger::Instance()->syslog(LOG_ERR, "ERROR") << ex.what() << endl;

 }	
  catch(...) {
    ocfa::misc::OcfaLogger::Instance()->syslog(LOG_ERR, "ERROR") << "Ging fout!\n";
  }
  delete e;
  return 1;

}

