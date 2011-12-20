
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

#include "db.h"
#include <facade/XMLAccessor.hpp>

class SelfDigestModule : public ocfa::facade::XMLAccessor {
public:
  SelfDigestModule();
  ~SelfDigestModule();
  void   processEvidence();
  //void   processMessage(ocfa::message::Message &msg){}
private:
  bool updateDynDB(string caseid,string xmlid,string sha1);
  string getReference();
  int getCount() {return count;}
  DB *dyncasedb;
  DB *dynsrcdb;
  int count;
  string dkey;
};
