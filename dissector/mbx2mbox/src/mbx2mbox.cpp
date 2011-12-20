
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

#include <error.h>        
#include "../../../binaries.hpp"
#include "mbx2mbox.hpp"
#include <stdexcept>
#include <vector>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
using namespace ocfa::facade;
using namespace ocfa::evidence;
using namespace ocfa::misc;


Mbx2mbox::Mbx2mbox() : EvidenceDeriveAccessor("mbx2mbox", "default"){
}

void Mbx2mbox::processEvidence() {

  char cline[1024];
  size_t size;
  char *line;
  FILE *command;

  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject();
  string wd=getWorkDir();  
  if (evidence == 0){
    throw OcfaException("Evidence without evidence store",this);
  }
  
  string outputFile = wd + "/mail";
  string linkFile = wd + "/mail.mbx";
  string inputFilePath = evidence->getAsFilePath();
  delete evidence;

  if(symlink(inputFilePath.c_str(),linkFile.c_str()) != 0) {
	getLogStream(LOG_CRIT) << "Not possible to link mail.mbx to repository path";
	throw OcfaException("Not possible to link mail.mbx to repository path", this);
  }
  
  
  int retval = snprintf(cline,1023,"%s %s",BINARY_MBX2MBOX, linkFile.c_str());

  if (retval < 0 || retval > 1023){
    getLogStream(LOG_CRIT) << "Commandstring does not fit in allocated buffer or an error occurred\nCmd: " 
	<< cline << endl;
    throw OcfaException("an error occurred while creating the command line argument", this);
  }

  cline[1023]=0; 
  getLogStream(LOG_DEBUG) << "Command is " << cline << endl;
  command=popen(cline,"r");
  if (command==0) 
    throw OcfaException("Unable to fork process \n");

  size=0;
  line=0;
  while (getline(&line, &size, command) != -1) {
    if((line != 0) && (size !=0)) {
       free(line);
    }
    size = 0;
    line = 0;
  }

  /* Try close pipe to command string */
  if( (pclose(command)) == -1) {
	throw OcfaException("Unable to close pipe to 7z command");
	return;
  }

  //All done, so unlink
  unlink(linkFile.c_str());


  //Derive mail
  Evidence *derived = derive("mail", Scalar("mail"));
  submitEvidence(derived);
  delete derived;

  return ;
}

int main(int argc, char *argv[]){
  Mbx2mbox *module=0;
  try {
    module= new Mbx2mbox();
  } catch(OcfaException ex) {

    module->getLogStream(LOG_ERR) <<  "Ocfa Exeption cougth in module construction: " << ex.what() << "\n";
    return 2;
  }
  try {
     module->run();
  } catch(OcfaException ex) {
    module->getLogStream(LOG_CRIT) << "Ocfa Exeption cougth: " << ex.what() << "\n";
  } catch(...) {
    module->getLogStream(LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  module->PrintObjCount();
  delete module ;
  return 1;
}

