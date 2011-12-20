
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

#ifndef INCLUDED_OCFARAR_H
#define INCLUDED_OCFARAR_H
#include <facade/EvidenceDeriveAccessor.hpp>

class Ocfarar : public ocfa::facade::EvidenceDeriveAccessor {
private:
   regex_t     reg_rar1,
               reg_rar2,
               reg_rar3;
   regmatch_t  pmatch[2];
   //temp Dir for rar multiparts
   string      mtmpDir;

   /**
    * Get the rootname from multipart rar files. Eg:
    * New multipart volume naming:
    *    "basename.part0123.rar" returns "basename"
    * Old multipart volume naming:
    *    "basename.rar" returns "basename"
    *    "basename.r12" returns "basename"
    */
   string      getRootRarName(string RarName);

   
public:
  Ocfarar();
  virtual ~Ocfarar();     
  virtual void processEvidence();
};
#endif

