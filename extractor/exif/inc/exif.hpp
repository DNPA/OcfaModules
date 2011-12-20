
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

#ifndef __EXIF_HPP__
#define __EXIF_HPP__

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <facade/EvidenceFileAccessor.hpp>

#define MAX_LINE_LENGTH    1024
#define DELIMITER            58

class Exif : public ocfa::facade::EvidenceFileAccessor {
 
  public:
                    Exif();
                   ~Exif();
    void            processEvidence();
    void            processMessage(ocfa::message::Message &msg);
  private:
    string          szCommand;
    
    // private methods
    void            processLine(char *inLine);
};

#endif
