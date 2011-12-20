
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

#include <Smtp.hpp>
#include <stdexcept>
#include <vector>
#include <regex.h>
#include "../../../binaries.hpp"
#include <fs/FileSystemSelector.hpp>
#include <misc/Exception.hpp>
using namespace
  ocfa::facade;
using namespace
  ocfa::evidence;
using namespace
  ocfa::misc;

Smtp::Smtp():EvidenceDeriveAccessor("smtp","default") {

  ocfa::fs::FileSystemSelector::selectAndInit(getWorkDir(), "UTF8");

}


void Smtp::processEvidence ()
{
  FILE *command;
  char *line;
  size_t size;
  char cline[1024];

  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject ();
  string wd = getWorkDir ();
  if (evidence == 0)
    throw OcfaException ("Evidence without evidence store", this);
  snprintf (cline, 1023,
	    "%s %s/sbin/smtpdissect.pl %s %s 2>/dev/null",
	    BINARY_PERL ,OcfaConfig::Instance()->getValue("ocfaroot").c_str() , evidence->getAsFilePath().c_str(), wd.c_str());
  delete evidence;
  cline[1023] = 0;
  getLogStream (LOG_DEBUG) << "Command to execute: " << cline << endl;
  command = popen (cline, "r");
  if (command == 0)
    throw OcfaException ("Unable to fork: smtpdissect\n");
  size = 0;
  line = 0;
  Evidence *output=0;
  while (getline (&line, &size, command) != -1)
    {
      if (line)
	{
	  free (line);
	}
      line = 0;
      size = 0;
    }
  output=derive("output",Scalar("output","LATIN1"),"content");
  submitEvidence(output); 
  delete(output);
  pclose (command);

  return;
}


int
main (int, char *[])
{
  Smtp *smtp = 0;
  try
  {
    smtp = new Smtp ();
  } catch (OcfaException &ex)
  {
    cerr << "Ocfa Exeption cougth in module constructor: " << ex.
      what () << "\n";
    return 2;
  }
  try
  {
    smtp->run ();
  }
  catch (OcfaException &ex)
  {
    smtp->getLogStream (LOG_CRIT) << "Ocfa Exeption cougth: " << ex.
      what () << "\n";
  }
  catch (...)
  {
    smtp->getLogStream (LOG_CRIT) << "Non Ocfa Exeption cought\n";
  }
  smtp->PrintObjCount ();
  delete smtp;
  return 1;
}
