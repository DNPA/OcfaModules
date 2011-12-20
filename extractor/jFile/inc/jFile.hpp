
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

#ifndef INCLUDED_JFILE_H
#define INCLUDED_JFILE_H

#include <facade/EvidenceFileAccessor.hpp>
#include<magic.h>
#include <unistd.h>

using namespace ocfa::misc;

namespace ocfa {
 namespace evidence {
   class jFile : public ocfa::facade::EvidenceFileAccessor {
     public:
       jFile():EvidenceFileAccessor("file","default"), d_magic_text(0), d_magic_mime(0){
         string ocfaRoot = OcfaConfig::Instance()->getValue("ocfaroot");
	 if (ocfaRoot == ""){
             throw OcfaException("No ocfaroot value found", this);
         }
	 
	 //try to load the compiled version of the magic mime file
	 string mimeFile = ocfaRoot + "/etc/magic";
	 string mimeFileAcc = mimeFile + ".mime";
	 if(access(mimeFileAcc.c_str(), R_OK) < 0) {
	     throw OcfaException("No read access on magic.mime file", this);
	 }
       
         d_magic_mime = magic_open(MAGIC_MIME | MAGIC_SYMLINK | MAGIC_CHECK);
	 if (d_magic_mime == NULL){
           throw OcfaException("Magic MIME cookie intialization failed.",this);
	 }
	 if (magic_load(d_magic_mime,mimeFile.c_str()) != 0){
           throw OcfaException("Loading of magic MIME file failed.",this); 
	 }
	 d_magic_text = magic_open(MAGIC_CHECK | MAGIC_SYMLINK);
	 if (d_magic_text == NULL){
	    throw OcfaException("Magic TEXT cookie intialization failed.",this);
         }
	 if (magic_load(d_magic_text,mimeFile.c_str()) != 0){
	    throw OcfaException("Loading of magic TEXT file failed.",this);
	 }	  

       };
       virtual ~jFile(){
	 if (d_magic_mime){      
           magic_close(d_magic_mime);
	 }
	 if (d_magic_text){
	   magic_close(d_magic_text);
	 }
       };
       void   processEvidence();
     //   void   processMessage(message::Message &msg);

     protected:

       int trim(char *instring);
       int breakafterspace(string &instring); 
       int GetFileName (const string fqfilenaam, string &fname);

       int GetFileExt (const string filenaam, string &xtens);

       int GetFullFileInfo (const string filenaam, string &fullfilename);
       int GetMimeInfo(const string filenaam, string &mtop, string &mtype, string &minfo);
       bool getOleType(string filename, string &oletype);
     private:
       magic_t d_magic_text, d_magic_mime;
       
   }; // class
 } // namespace evidence
} // namespace ocfa

#endif
