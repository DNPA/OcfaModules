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

#include "ocfarar.hpp"
#include <stdexcept>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <regex.h>
#include <store/EvidenceStoreEntity.hpp>
#include "../../../binaries.hpp"

using namespace ocfa;
using namespace ocfa::evidence;
using namespace ocfa::misc;
using namespace ocfa::evidence;
using namespace ocfa::store;

Ocfarar::Ocfarar():EvidenceDeriveAccessor("ocfarar","default")
{
   // put thirdparty-library initializations here ...
   regcomp(&reg_rar1,"^(.+)\\.part[0-9]*$",REG_EXTENDED);
   regcomp(&reg_rar2,"^(.+)\\.rar$",REG_EXTENDED);
   regcomp(&reg_rar3,"^(.+)\\.r[0-9]{2}$",REG_EXTENDED);
   //Get Temp rar dir from conf file
   mtmpDir = OcfaConfig::Instance()->getValue("rartmpdir");
}
Ocfarar::~Ocfarar()
{
    // clean up here ...
   regfree(&reg_rar1);
   regfree(&reg_rar2);
   regfree(&reg_rar3);
   
}
/**
 * Het idee is als volgt:
 * Haal van evidence de basisnaam van het rar bestand.
 * Samen met de casename vormt dit een unique directory voor alle
 * delen van de multipart rar. Sla het evidence in deze directory op
 * en probeer rar hierop uit te voeren. Indien dit lukt, is de set 
 * compleet en kan de directory opgeruimd worden, anders blijven de 
 * delen voor betreffende case en basisnaam van de rar bestaan.
 * 
 * De delen worden opgeslagen in:
 * /var/ocfa/<casename>/tmp_rar/<casename>__<rarbasename>
 * createcase.pl maakt deze directory aan en wordt uit de <casename>.conf gehaald.
 */ 
void  Ocfarar::processEvidence()
{
  EvidenceStoreEntity * evidence = fetchEvidenceStoreObject();
  string filename = evidence->getAsFilePath();
  getLogStream(LOG_DEBUG) << "Processing " << filename << endl;

   // get name of evidence and check wether a workingdir is present
  string EvidenceName = getEvidenceName().asASCII();
  getLogStream(LOG_DEBUG) << "EvidenceName=" + EvidenceName;

   // your workdir can be found by getWorkDir()
   string workdir = getWorkDir();
   string rootrarname = getRootRarName(EvidenceName);
   if(rootrarname.compare("") != 0) {
      string tmpworkdir = mtmpDir + "/" + getCase() + "__" + rootrarname;
      getLogStream(LOG_DEBUG) << "tmpworkdir=" + tmpworkdir;
      struct stat statbuf;
      stat(tmpworkdir.c_str(),&statbuf);
      if(!S_ISDIR(statbuf.st_mode)) {
         mkdir(tmpworkdir.c_str(), 0755);
      }
      string filename = tmpworkdir + "/" + EvidenceName;
      getLogStream(LOG_DEBUG) << "filename=" + filename;
      if(link((evidence->getAsFilePath()).c_str(),filename.c_str()) != 0) {
         getLogStream(LOG_ERR) << "Not possible to link " + filename;
      } else {
         // try rar on file, if succesful, curworkdir can be removed
         // test rar archive
         string cmd = string(BINARY_RAR) + " t " + filename;
         int status = system(cmd.c_str());
         if(WEXITSTATUS(status) == 0) {
            // RAR == OK
            string outputdir = workdir + "/RAREXTRACT" ;
            mkdir(outputdir.c_str(), 0755);
            cmd = string(BINARY_RAR) + " x " + filename + " " + outputdir;
            status = system(cmd.c_str());
            //all OK, so remove all parts and commit evidence
            setMeta("rarcomplete", ocfa::misc::Scalar(1));
            Evidence *derived = derive("RAREXTRACT", Scalar("RAREXTRACT"));
            submitEvidence(derived);
            delete derived;
            cmd = "rm -rf " + tmpworkdir;
            status= system(cmd.c_str());
            if(WEXITSTATUS(status) != 0) {
               getLogStream(LOG_ERR) << "ERROR " + cmd ;
            }
         } else {
            getLogStream(LOG_DEBUG) << "NOT COMPLETE YET " + cmd;
         }
      }
   } else {
      getLogStream(LOG_WARNING) << "Found no rootrarname for evidence " + EvidenceName;
   }
   delete evidence;
}

/**
 * Get the rootname from:
 * - rootname.rar or
 *   rootname.partXXX.rar
 * - rootname.rYY
 * (X are numbers, Y are numbers)
 */
string Ocfarar::getRootRarName(string RarName) {
   string rootname="";
   
   
   // test for "(rootname).rar" or "(rootname.part012).rar"
   if (regexec (&reg_rar2, RarName.c_str(), 2, pmatch, 0) == 0) {
      int i =1;
      string match = RarName.substr(pmatch[i].rm_so,pmatch[i].rm_eo - pmatch[i].rm_so);
      // sub-test for "(rootname).part012"
      if (regexec (&reg_rar1, match.c_str(), 2, pmatch, 0) == 0) {
         string submatch = match.substr(pmatch[i].rm_so,pmatch[i].rm_eo - pmatch[i].rm_so);
         //it was (rootnaam).part012.rar
         return submatch;
      } else {
         //it was (rootnaam).rar
         return match;
      }
   } else {
      // test for "(rootnaam).rYY"
      if (regexec (&reg_rar3, RarName.c_str(), 2, pmatch, 0) == 0) {
         int i =1;
         string match = RarName.substr(pmatch[i].rm_so,pmatch[i].rm_eo - pmatch[i].rm_so);
         //it was (rootnaam).rYY
         return match;
      }
   }
   return rootname;
}




int main(int , char *[])
{
  Ocfarar *pOcfarar;
  try {
    pOcfarar = new Ocfarar();
    pOcfarar->run();
  } catch(OcfaException &ex) {
    ex.logWhat();
    pOcfarar->getLogStream(LOG_CRIT) << "Ocfa Exeption cougth: " << ex.what() << "\n";
  } catch(...) {
    pOcfarar->getLogStream(LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  delete pOcfarar;
  return 0;
}
