
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

#include <ctype.h>
#include "file426.hpp"
#include "compoundbinaryfile.hpp"
using namespace ocfa::evidence;

//***********************************************************************************
int jFile::breakafterspace(string &instring)
{ 
  int firstchar = instring.find(" ",0);

  if (firstchar != static_cast<int>(string::npos)){

    string uitstring = instring.substr(0,firstchar);
  }
  else {

    logModule(LOG_NOTICE, string("did not find a space"));
  }
  //instring = uitstring;  

  
  return(0);
} // breakafterspace
//***********************************************************************************
int jFile::trim(char *instring)
{ 
  int firstchar;
  int lastchar;
  int lengte = strlen(instring);

  if (lengte > 1023){

    logModule(LOG_NOTICE, string("jFile::trim lengte is "));// + lengte);
  }
  char hulp[1024];
  hulp[0] = 0;  

  for (firstchar=0 ; firstchar < lengte; firstchar++) {
    if  (! isspace(instring[firstchar])) {
      break;
    }
  } //for
  
  for (lastchar=lengte-1 ; lastchar >= 0; lastchar--) {
    if  (! isspace(instring[lastchar])) {
      break;
    }
  } // for
  
  for (int i=0; i < lastchar-firstchar+1 && i < 1022; i++){
    hulp[i] = instring[i+firstchar];
    hulp[i+1] = '\0';
  } // for
 
  // check on string length
  if (strlen(hulp) > strlen(instring)){

    logModule(LOG_CRIT, string(hulp) + " greater than " + instring);
  }
    
  strncpy(instring,hulp, 1024);
  return(0);
} // trim

//***********************************************************************************
int jFile::GetFileExt (const string filenaam, string &xtens)
{
  int (*pf)(int)= tolower;
  string fnaam = filenaam;
  string hulp;

  string::size_type namelength = fnaam.length();
  string::size_type lastslashpos = fnaam.rfind("/",namelength ); 
  string::size_type lastdotpos = fnaam.rfind(".",namelength ); 
  // if no dot after last slash (so the filename has no extension) 
  //    or no dot in filename 
  if ((lastdotpos == string::npos       ) || ((lastslashpos != string::npos) && (lastslashpos > lastdotpos)) || (lastdotpos == namelength - 1)) {
    xtens = "NON";
    logModule(LOG_DEBUG, "returning NONE for " + fnaam);
    return 0;
  } 
  hulp = fnaam.substr( lastdotpos + 1 , namelength-lastdotpos);  
  xtens = hulp; // create enough space
  transform(hulp.begin(),hulp.end(),xtens.begin(),pf); // RJM: cast the output to lower case.
  if (xtens == "") {
    logModule(LOG_ERR, string("Tolower failed : ") +  hulp + " from " + fnaam);	  
     xtens="ERR"; //Note, as the real extensions are lower cased, "NON" does not clash with .non extensions !
  }
  // xtens = hulp;
  return(1);
}       // GetFileExt

//***********************************************************************************
int jFile::GetFileName (const string fqfilenaam, string &fnaam)
{
 string hulp;
 string fhulp = fqfilenaam;
 string::size_type namelength = fhulp.length();
 string::size_type lastslashpos = fhulp.rfind("/",namelength ); 
  
 if (lastslashpos >= namelength) {
    hulp = fhulp;
 }
  else {
    hulp = fhulp.substr( lastslashpos + 1 , namelength-lastslashpos);  
  };
  fnaam = hulp;
  return(1);
}       // GetFileName
//***********************************************************************************   
int jFile::GetFullFileInfo (const string filenaam, string &fullfilename)
{
  char retval[1024];
  const char *magic_str = 0;
  magic_str = magic_file(d_magic_text, filenaam.c_str());
  if (magic_str == 0){
    logModule(LOG_ERR, string(magic_error(d_magic_text)));	  
    fullfilename = ""; 
    return 0;
  }
  strncpy(retval, magic_str, 1023);
  logModule(LOG_DEBUG, string("retval = ") + retval);
  //string commando = "file -bL \"" + filenaam + "\"";

  // strip de laatste CR uit de regel 
  trim(retval); 
  fullfilename = retval;
  return(1);
}  // GetFullFileInfo
//***********************************************************************************
int jFile::GetMimeInfo(const string filenaam, string &mtop, string &mtype, string &minfo)
{
  char retval[1024];
  const char *magic_str = 0;
  string retv;
  string fnaam = filenaam;

  magic_str = magic_file(d_magic_mime, fnaam.c_str());
  if ((magic_str == 0)||(strlen(magic_str) == 0)){ //short circuit eval
       mtop =  "ocfa_undef";
       mtype = "ocfa_undef"; 
       minfo = "ocfa_undef";
       if (magic_str == 0) logModule(LOG_ERR, string("") +  magic_error(d_magic_mime));
       return (1);
  }
  strncpy(retval, magic_str, 1023);
  logModule(LOG_DEBUG, string("retval of fullFileInfo= ") + retval);
  //string commando = "file -ibL \"" + fnaam + "\"";
  string::size_type semcolpos;
  string::size_type commapos;

  // strip de laatste CR uit de regel 
  trim(retval);
  // EXCEPTIONS 
  retv = retval;
    // NORMAL FLOW 
    if (retv.substr(0,4) == "text") {            
      mtop = "text";
      
      // text/plain, English; charset=us-ascii
      // text/plain; charset=us-ascii

      // if ; or ,  found, then rest is mimeinfo else 
      // format : text/yyyyy,;zzzz
      // text        = mimetop
      // xxxxx/yyyyy = mimetype 
      // zzzzz       = mimeinfo
      semcolpos    = retv.find(";",0);
      commapos = retv.find(",",0);
      // if comma found first, this is the delimiter. else the semicolon is the delimiter
      if (commapos < semcolpos) {
        semcolpos = commapos;
      };
        
  
      if (semcolpos == retv.npos) {  // delimiter not found...
        mtype = retv;
        breakafterspace(mtype);
        minfo = "ocfa_undef";
      } else                         // delimiter found
	{ // alleen hier :   mtype =! retv
	mtype = retv.substr(0,semcolpos);
	breakafterspace(mtype);
        minfo = retv.substr(semcolpos+2,retv.npos-semcolpos); 
      }  
     return(1);
    }
  // all other types are similar : all before the first slash = mimetop
  // form :   mimetop/mimetype
  string::size_type slashpos = retv.find("/",0);
  mtop = retv.substr(0, slashpos);
  mtype = retv;
  breakafterspace(mtype);
  minfo = "ocfa_undef";
  return(1);
} // GetMimeInfo
//***********************************************************************************

bool jFile::getOleType(string filename, string &oletype){
  
  CompoundBinaryFile *cbf = 0;
  bool succes = false;
  
  try {
    
    cbf = new CompoundBinaryFile(filename);
    succes = cbf->getApplication(oletype);
    
  } catch (string &msg) {
    logModule(LOG_ERR, msg);
  } catch (...) {
    logModule(LOG_ERR, string("Error in cbf"));
  }
  delete cbf;
  return succes;
   
}


