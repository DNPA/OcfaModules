
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
#include <string>

using namespace std;

string filenaam;

int trim(char *instring);

int breakafterspace(string &instring);
int GetFileExt (string *filenaam, string *xtens);
int GetFileName (string *fq_filenaam, string *fnaam);
int GetFullFileInfo (string *filenaam, string *fullfilename); 
int GetMimeInfo(string *filenaam, string *mtop, string *mtype, string *minfo);

string mimetop;
string mimetype;
string mimeinfo; 
string fullfileinfo;
string extensie;
string filenaampje;
string filename;
 
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
    filenaampje = regel;
    // here starts the action :
    
    geluk = GetFileExt (&filenaampje,&extensie);
    geluk = GetFileName (&filenaampje,&filename);
    geluk = GetFullFileInfo(&filenaampje, &fullfileinfo);
    geluk = GetMimeInfo(&filenaampje,&mimetop, &mimetype,&mimeinfo);
   
    if (mimetop == "text") {
      cout << " ------------------------------------:" << "\n"; 
      //cout << "regel     " << regel << "\n";
      cout << "mto       " << mimetop << "\n";  
      cout << "mty       " << mimetype << "\n";
      // cout << "min       " << mimeinfo << "\n";
      //cout << "full      " << fullfileinfo << "\n";
      // cout << "filename  " << filename << "\n";
      // cout << "ext =     " << extensie << "\n";
    }
    
  } // while
} //main

#include "./functies.cpp"
