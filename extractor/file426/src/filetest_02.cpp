
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

#include <stdio.h>
#include <iostream.h>
#include <regex.h>
#include <string>

using namespace std;

string filenaam;

int trim(char *instring);
int GetFileExt (char *filenaam, string *xtens);
int GetFullFileInfo (char *filenaam, string *fullfilename); 
int GetMimeInfo(char *filenaam, string *mtop, string *mtype, string *minfo);

string mimetop;
string mimetype;
string mimeinfo; 
string fullfileinfo;
string extensie;
 
int main() {
  const char fname[] = "./../input.txt";
  FILE *infile;
  infile = fopen(fname,"r");

  char *result;
  //  string regel;
  char regel[512];
  int geluk ; 
 
  // lees de regels uit de in-file een voor een in....
  while (1) {
    //  cout << "----------------------------------------" << "\n";

    result = fgets(regel,sizeof(regel)-1,infile);
    if (result  == NULL) { 
      break;
    } 
    // strip de laatste CR uit de regel 
    trim(regel);
    // tot hier is dus voor testdoeleinden.

    // here starts the action :

    geluk = GetFileExt (regel,&extensie);
    geluk = GetFullFileInfo(regel, &fullfileinfo);
    geluk = GetMimeInfo(regel,&mimetop, &mimetype,&mimeinfo);

    cout << "regel " << regel << "\n";
    cout << "mto = " << mimetop << "\n";  
    cout << "mty = " << mimetype << "\n";
    cout << "min = " << mimeinfo << "\n";
    cout << "full = " << fullfileinfo << "\n";
    cout << "ext = " << extensie << "\n";
  } // while
} //main

//***********************************************************************************
int trim(char *instring)
{ 
  instring[strlen(instring)-1] = '\0';
  return(0);
} // trim
//***********************************************************************************
int GetFileExt (char *filenaam, string *xtens)
{
  string fnaam = filenaam;
  string hulp;

  string::size_type namelength = fnaam.length();
  string::size_type lastslashpos = fnaam.rfind("/",namelength ); 
  string::size_type lastdotpos = fnaam.rfind(".",namelength ); 

  if ((lastslashpos >= lastdotpos) || (lastdotpos >= namelength)) {
    hulp = "";
  } 
  else {
    hulp = fnaam.substr( lastdotpos + 1 , namelength-lastdotpos);  
  };

  *xtens = hulp;
  return(1);
}       // GetFileExt
//***********************************************************************************   
int GetFullFileInfo (char *filenaam, string *fullfilename)
{
  FILE *fp; 
  char retval[1024];
  string fnaam = filenaam;
  string commando = "file -b \"" + fnaam + "\"";

  fp = popen (commando.c_str(),"r");
  fgets(retval,sizeof(retval)-1,fp);
  fclose(fp); 
  // strip de laatste CR uit de regel 
  trim(retval); 
 //  cout << filenaam;
  fullfileinfo = retval;
  return(1);
}  // GetFullFileInfo
//***********************************************************************************
int GetMimeInfo(char *filenaam, string *mtop, string *mtype, string *minfo)

{
  FILE *fp; 
  char retval[1024];
  string retv;
  string fnaam = filenaam;
  string commando = "file -ib \"" + fnaam + "\"";
  string::size_type semcolpos;

  fp = popen (commando.c_str(),"r");
  fgets(retval,sizeof(retval)-1,fp);
  fclose(fp); 
  // strip de laatste CR uit de regel 
  retval[strlen(retval)-1] = '\0';

    retv = retval;
    if (retv.substr(0,10) == "can't stat") {
      *mtop =  "ocfa_undef";
      *mtype = "ocfa_undef";  
      *minfo = "ocfa_undef";
      return (1);
    }
    if (retv.substr(0,10) == "can't read") {
      *mtop  = "ocfa_undef";
      *mtype = "ocfa_undef";  
      *minfo = "ocfa_undef";
      return(1);
    }
    if (retv.substr(0,22) == "executable, can't read") {
      *mtop = "executable";
      *mtype = "ocfa_undef";  
      *minfo = "ocfa_undef";
      return(1);
    }

    if (retv.substr(0,4) == "text") {
      *mtop = "text";
      // if ; found, then rest is mimeinfo else 
      // format : mimetop/mimetype;mimeinfo
      semcolpos = retv.find(";",0);

      if (semcolpos == retv.npos) {  // ";" not found...
        *mtype = retv.substr(5,retv.length()-5); 
        *minfo = "ocfa_undef";
      } else                         // ";" found
      {
        *mtype = retv.substr(5,semcolpos-5);
        *minfo = retv.substr(semcolpos+2,retv.npos-semcolpos); 
      }  
     return(1);
    }
  // all other types are similar : all before the first slash = mimetop
  // form :   mimetop/mimetype
  string::size_type slashpos = retv.find("/",0);
  *mtop = retv.substr(0, slashpos);
  *mtype = retv.substr(slashpos+1,retv.length()- slashpos+1 );
  *minfo = "ocfa_undef";
  return(1);
} // GetMimeInfo
//***********************************************************************************
