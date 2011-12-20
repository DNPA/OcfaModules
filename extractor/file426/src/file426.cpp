
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

#include "../inc/file426.hpp"
#include "../inc/compoundbinaryfile.hpp"
#include <stdexcept>
#include <string.h>

using namespace ocfa;
using namespace message;
using namespace store;
using namespace evidence;
//using namespace xercesc;
using namespace std;


/* mandatory local implementation of the abstract processEvidence */
void
jFile::processEvidence ()
{
  string mimetop;
  string mimetype;
  string mimeinfo;
  string fullfileinfo;
  string fileextensie;
  string filename;
  string fqfilename;

  string filenaam;

  string local_fq_filename;
  string source_fq_filename;

  // obtain a file handle to the evidence
  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject ();
  // and the filepath of the evidence is...
  if (evidence == 0)
    {

      logModule (LOG_ERR, "no datafile was given");
      logEvidence (LOG_ERR, "no datafile given ");
      return;
    }

  logModule (LOG_DEBUG, "Store: " + evidence->getAsFilePath ());

  local_fq_filename = evidence->getAsFilePath ();
  try
  {
    //source_fq_filename  = getEvidenceLocation().asUTF8() ;
    source_fq_filename = getEvidenceName ().asUTF8 ();	// RJM: think the name is sufficient, dont need the full path
    logModule (LOG_DEBUG, "source_fq_filename is " + source_fq_filename);

  }
  catch (exception & ex)
  {
    logModule (LOG_CRIT, string ("asUTF8 Failed: ") + ex.what ());
    delete evidence;
  }
  GetFileName (source_fq_filename, filename);

  GetFileExt (source_fq_filename, fileextensie);


  GetFullFileInfo (local_fq_filename, fullfileinfo);

  GetMimeInfo (local_fq_filename, mimetop, mimetype, mimeinfo);


  setMeta ("mimetop", mimetop);

  setMeta ("mimetype", mimetype);
  setMeta ("mimeinfo", mimeinfo);

  if (mimetype == "application/msword")
    {
      string oletype;
      try
      {
	if (getOleType (local_fq_filename, oletype))
	  {
	    setMeta ("oletype", oletype);
          } else {
             setMeta ("oletype", "INVALID");
          }
      }
      catch (...)
      {
	logModule (LOG_ERR,
		   "Exception thrown on determining oletype of " +
		   source_fq_filename);
      }
    }

  setMeta ("fileinfo", fullfileinfo);
  setMeta ("fileextension", fileextensie);


  delete evidence;

}


int
main (int, char *[])
{
  try
  {
    jFile *test = 0;
    try
    {
      test = new jFile ();
    } catch (OcfaException & ex)
    {
      ex.logWhat ();
      return 2;
    }
    catch (string & ex)
    {

      test->logModule (LOG_CRIT, string ("string exception: ") + ex);
    }

    try
    {
      test->run ();
    }
    catch (OcfaException & ex)
    {
      ex.logWhat();
      test->logModule (LOG_CRIT,
		       string ("Ocfa Exeption cougth: ") + ex.what ());
    }
    catch (bad_alloc & ex)
    {
      test->logModule (LOG_CRIT,
		       string ("bad_alloc Exeption cougth: ") + ex.what ());
    }
    catch (bad_exception & ex)
    {
      test->logModule (LOG_CRIT,
		       string ("bad_exception Exeption cougth: ") +
		       ex.what ());
    }
    catch (bad_cast & ex)
    {
      test->logModule (LOG_CRIT,
		       string ("bad_cast Exeption cougth: ") + ex.what ());
    }
    catch (bad_typeid & ex)
    {
      test->logModule (LOG_CRIT,
		       string ("bad_typeid Exeption cougth: ") + ex.what ());
    }
    catch (out_of_range & ex)
    {
      test->logModule (LOG_CRIT,
		       string ("out_of_range Exeption cougth: ") +
		       ex.what ());
    }
    catch (overflow_error & ex)
    {
      test->logModule (LOG_CRIT,
		       string ("overflow_error Exeption cougth: ") +
		       ex.what ());
    }
    catch (ios_base::failure & ex)
    {
      test->logModule (LOG_CRIT,
		       string ("ios_base::failure Exeption cougth: ") +
		       ex.what ());
    }
    catch (length_error & ex)
    {
      test->logModule (LOG_CRIT,
		       string ("length_error Exeption cougth: ") +
		       ex.what ());
    }
    catch (domain_error & ex)
    {
      test->logModule (LOG_CRIT,
		       string ("domain_error Exeption cougth: ") +
		       ex.what ());
    }
    catch (invalid_argument & ex)
    {
      test->logModule (LOG_CRIT,
		       string ("invalid_argument Exeption cougth: ") +
		       ex.what ());
    }
    catch (logic_error & ex)
    {
      test->logModule (LOG_CRIT,
		       string ("logic_error Exeption cougth: ") + ex.what ());
    }
    catch (exception & ex)
    {
      test->logModule (LOG_CRIT, string ("Exeption cougth: ") + ex.what ());
    }
    catch (string & ex)
    {

      test->logModule (LOG_CRIT, string ("string exception: ") + ex);
    }
    catch (...)
    {
      test->logModule (LOG_CRIT,
		       string
		       ("Some infidel seems to be throwing  non-OcfaExeption exeptions"));
    }
    try {
         test->PrintObjCount ();
    } catch (OcfaException &ex) {
       ex.logWhat();
    }
    delete test;
    return 1;
  } catch (...) {
     return 3;
  }
}


//**********************************************************************

#include "./functies.cpp"

//***********************************************************************************
