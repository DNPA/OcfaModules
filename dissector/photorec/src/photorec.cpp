
//  The Open Computer Forensics Architecture moduleset.
//  Copyright (C) 2003..2008 KLPD  <ocfa@dnpa.nl>
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

//#define _OCFA_BACKPORT

#include <sys/stat.h>
#include <sys/types.h>
#include <photorec.hpp>
#include <stdexcept>
#include <vector>
#include <dirent.h>
#include "../../../binaries.hpp"

#ifdef _OCFA_BACKPORT
#include <fs/FileSystemSelector.hpp>
#else 
#include <treegraph/TreeGraphModuleLoader.hpp>
#include <fs/FileSystemModuleInfoFactory.hpp>
#include <fs/FileSystemModuleInfo.hpp>
#endif

#include <misc/Exception.hpp>

using namespace
  ocfa::facade;
using namespace
  ocfa::evidence;
using namespace
  ocfa::misc;

Photorec::Photorec():EvidenceDeriveAccessor("carver","photorec") {
#ifdef _OCFA_BACKPORT
  ocfa::fs::FileSystemSelector::selectAndInit(getWorkDir(), "UTF8");
#else 
  ocfa::fs::FileSystemModuleInfo *utf8workdirinfo=ocfa::fs::FileSystemModuleInfoFactory::findProperModuleInfo(getWorkDir(), "UTF8");
  ocfa::treegraph::TreeGraphModuleLoader::selectAndInit(*utf8workdirinfo, *utf8workdirinfo);
  delete utf8workdirinfo;
#endif
}


void Photorec::processEvidence ()
{
  FILE *command;
  char *line;
  size_t size;
  char cline[1024];
  int rval;
  DIR *wdir;
  struct dirent *dirrec;

  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject ();
  string wd = getWorkDir() + "/";
  if (evidence == 0)
    throw OcfaException ("Evidence without evidence store", this);
  
  /*Construct command line for popen */
  snprintf(cline, 1023, "%s/sbin/ocfacarv.sh %s %s %s",
           OcfaConfig::Instance()->getValue("ocfaroot").c_str(),
           wd.c_str(), evidence->getAsFilePath().c_str(),
           BINARY_PHOTOREC_CLI );
  cline[1023] = 0;
  getLogStream (LOG_DEBUG) << "CMD:: " << cline << endl;
  delete evidence;
  
  //Try to open FILE with popen to execute command string
  if(system(cline) == -1) {
  //if((command = popen (cline, "r")) == 0) {
     getLogStream(LOG_ERR) << "Unsuccesful execution POPEN cammand" << endl;
     throw OcfaException ("Unable to fork: photorec\n");
     return;
  }
  
  /* Read the standard output from the command line execution
  size = 0;
  line = 0;
  Evidence *output=0;
  while (getline (&line, &size, command) != -1) {
     if ((line != 0) && (size !=0))	{
           //Possible do some command line response processing
           getLogStream(LOG_DEBUG) << line << endl;
	   free (line);
      }
      line = 0;
      size = 0;
} */
  
  /* Try to close pipe to command string
  rval = -1;
  if((rval = pclose(command)) == -1) {
     throw OcfaException("Unable to close pipe to photorec command");
     return;
} */
  
  /* Try to derive evidence from unzipped content */
  wdir=opendir(wd.c_str());
  if (wdir != 0) {
     /* Treewalk throught result dir */
     while ((dirrec=readdir(wdir))) {
        string subdir=string("/") + string(dirrec->d_name);
        if ((subdir != "/.")&&(subdir != "/..")) {
           getLogStream(LOG_DEBUG) << "Submitting " << subdir << "\n";
           Evidence *derived = derive(subdir, Scalar(subdir,"LATIN1"));
           submitEvidence(derived);
           delete derived;
        }
     }
  } else {
     /* Unable to open working dir */
     logModule(LOG_ERR, "could not open working directory");
     logEvidence(LOG_ERR, "could not open working directory");
  }
  
  
  return;
}


int
main (int, char *[])
{
  Photorec *prec = 0;
  try
  {
    prec = new Photorec ();
  } catch (OcfaException &ex)
  {
    cerr << "Ocfa Exeption cougth in module constructor: " << ex.
      what () << "\n";
    return 2;
  }
  try
  {
    prec->run ();
  }
  catch (OcfaException &ex)
  {
    prec->getLogStream (LOG_CRIT) << "Ocfa Exeption cougth: " << ex.
      what () << "\n";
  }
  catch (...)
  {
    prec->getLogStream (LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  prec->PrintObjCount ();
  delete prec;
  return 1;
}
