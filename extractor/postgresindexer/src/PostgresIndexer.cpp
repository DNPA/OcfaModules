
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

#include <sys/types.h>
#include <pwd.h>

#include <iostream>
#include <sstream>
#include <redi/pstream.h>
#include "PostgresIndexer.hpp"

using namespace std;
using namespace redi;
using namespace ocfa::misc;
using namespace ocfa::facade;
string PostgresIndexer::mExtractionCommand = "tr -c '[:alnum:].@' ' ' | tr -s ' ' | tr ' ' '\n' | sort | uniq";
PostgresIndexer::PostgresIndexer() 
  : EvidenceFileAccessor("indexer", "default"), mConnection(0){
  
  openDatabase();

}

PostgresIndexer::~PostgresIndexer(){
  
  if (mConnection != 0){

    PQfinish(mConnection);
    mConnection = 0;
  }
  
}
void PostgresIndexer::openDatabase(){
 
  string dbName = getConfEntry("storedbname");
  string dbhost = getConfEntry("storedbhost");
  string dbuser = getConfEntry("storedbuser");
  char connectionString[255];
  if (dbuser == ""){
    
    struct passwd *pwent = getpwuid(getuid());
    dbuser = pwent->pw_name;
  }
  string dbpasswd = getConfEntry("storedbpasswd");
  sprintf(connectionString, "dbname=%s hostaddr=%s user=%s password=%s", 
	  dbName.c_str(), dbhost.c_str(), dbuser.c_str(), dbpasswd.c_str());
  
  mConnection = PQconnectdb(connectionString);
  if (PQstatus(mConnection) != CONNECTION_OK) {
    
    string errstr = "PgRepository initialization failed: " 
      + string(PQerrorMessage(mConnection));
    //printf("%s",errstr.c_str());
    ocfaLog(LOG_ERR, errstr);
    throw OcfaException(errstr, this);
  } 
  // PQsetNoticeProcessor(mConnection, DSMlogNotices, 0);
  
}


void PostgresIndexer::throwDatabaseException(string command, PGresult *inResult){
  
  ocfaLog(LOG_ERR, string("error executing ").append(command));
  ocfaLog(LOG_ERR, string("error was " ).append(PQresStatus(PQresultStatus(inResult))));
  ocfaLog(LOG_ERR, string("result status was ", PQresultStatus(inResult)));
  //  ocfaLog(LOG_ERR, string("primary description: ")
  //  + PQresultErrorField(inResult, PG_DIAG_MESSAGE_PRIMARY));	
  //ocfaLog(LOG_ERR, string((PQresultErrorField(inResult, PG_DIAG_SQLSTATE))));
  throw OcfaException(string("Error: ").append(PQresStatus(PQresultStatus(inResult))), this);
}    

void PostgresIndexer::processEvidence(){

  getLogStream(LOG_DEBUG) << "entering processEvidence()" << endl;
  ocfa::store::EvidenceStoreEntity* evidence = fetchEvidenceStoreObject();
  if(evidence != 0) {
    
    string word;
    ostringstream sqlQueryStream;
    string filePath = evidence->getAsFilePath();
    string theCommand = string("cat ") + filePath + " | " + mExtractionCommand;
    ipstream theWordStream(theCommand);
    while (!theWordStream.eof()){
      
      sqlQueryStream.str("");
      theWordStream >> word; 
      if (word.size() < 200){
      
	transform(word.begin(), word.end(), word.begin(), ::tolower);
	
	sqlQueryStream << "select insertword('" << word << "', " << evidence->getHandle() 
		       << ");" ;
	PGresult *pgres;
	PQexec(mConnection, sqlQueryStream.str().c_str());
	if (PQresultStatus(pgres) != PGRES_TUPLES_OK){

	  getLogStream(LOG_ERR) <<  string ("throwinf database exception on ") <<  PQresStatus(PQresultStatus(pgres)) << endl;
	  throwDatabaseException(sqlQueryStream.str(), pgres);
	}
	PQclear(pgres);
      }
      
    }
  } else {

    getLogStream(LOG_ERR) << "no storeevidence for " << getEvidenceID()<< endl;
    logEvidence(LOG_ERR, "no storeevidence, although it was needed for this module");
    
  }
}


int main(int, char *[]){

  PostgresIndexer *indexer;
  try {

    indexer = new PostgresIndexer();
  } catch (OcfaException &exn) {
		exn.logWhat();
		return 2;
  } 
  try {
    indexer->run();
  } catch (OcfaException &exn) {
    exn.logWhat();
    indexer->getLogStream(LOG_ERR) << "MidasIndexer module: OCFA exception" << exn.what() << endl;
  } catch (...) {
    indexer->getLogStream(LOG_ERR) << "MidasIndexer module: unknown exception" << endl;
  }
  delete indexer;
  return 1;
  
}
