
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

#ifndef __POSTGRES_INDEXER_HPP__
#define __POSTGRES_INDEXER_HPP__


#include <facade/EvidenceFileAccessor.hpp>
#include <pgsql/libpq-fe.h>
class PostgresIndexer : public ocfa::facade::EvidenceFileAccessor {

public:
  PostgresIndexer();
  ~PostgresIndexer();
  void processEvidence();

protected:
  void openDatabase(); 
  void throwDatabaseException(std::string command, PGresult *inResult);
private:
  /**
   * connection to the database.
   */
  PGconn *mConnection;
  static std::string mExtractionCommand;
};

#endif
