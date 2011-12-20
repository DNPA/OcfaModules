
// The Open Computer Forensics Architecture moduleset.
// Copyright (C) 2003..2006 KLPD  <ocfa@dnpa.nl>
// Copyright (C) 1997-2002 Sleepycat Software. (derived from berkeley db example)
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
//

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include<sstream>

#include "pgdigest.hpp"
#include <store/MetaStoreEntity.hpp>
using namespace ocfa;
using namespace ocfa::evidence;
using namespace ocfa::store;
using namespace ocfa::misc;

PgDigestModule::PgDigestModule():XMLAccessor("pgdigest","default"),mNumberSets(0){
   string dbname = ocfa::misc::OcfaConfig::Instance()->getValue("digestdb");
   if (dbname == "") {
       dbname="fivesdigest";
   }
   string dbhost = ocfa::misc::OcfaConfig::Instance()->getValue("digestdbhost");
   if (dbhost == "") {
       dbhost="127.0.0.1";
   }
   string dbuser = ocfa::misc::OcfaConfig::Instance()->getValue("digestdbuser");
   if (dbuser == "") {
        dbuser="fivesdigest";
   }
   string dbpasswd = ocfa::misc::OcfaConfig::Instance()->getValue("digestdbpasswd");
   if (dbpasswd == "") {
       dbpasswd="fivesdigest";
   } 
   std::string conninfo= std::string("dbname=") + dbname + " hostaddr=" + dbhost + " user=" + dbuser + " password=" + dbpasswd;
   ocfaLog(LOG_DEBUG, string("connectionString is ") + conninfo);
   mConnection = PQconnectdb(conninfo.c_str());
   if (PQstatus(mConnection) != CONNECTION_OK) {
        string errstr = "PgDigestModule initialization failed: " + conninfo;
        ocfaLog(LOG_ERR, errstr.c_str());
        throw DbConnectException(errstr, this);
   }

   const Oid stringparamtypes[] = {1043,1043} ;
  std::string sha1query("select country,categorydefinitions.name,colorcode,categorytable.name from categorydefinitions,categorytable,hashset where sha1=$1 and hashset.categoryid=categorytable.categoryid and categorytable.categorydefinitionsid=categorydefinitions.categorydefinitionsid");
  std::string md5query("select country,categorydefinitions.name,colorcode,categorytable.name from categorydefinitions,categorytable,hashset where md5=$1 and hashset.categoryid=categorytable.categoryid and categorytable.categorydefinitionsid=categorydefinitions.categorydefinitionsid");
  PGresult *pgres = PQprepare(mConnection, "bysha1", sha1query.c_str(), 1, stringparamtypes);
  if (PQresultStatus(pgres) != PGRES_COMMAND_OK){
         throw OcfaException("Prepare failed: " + sha1query + ":" + PQresultErrorMessage(pgres));
  }
  PQclear(pgres);
  pgres = PQprepare(mConnection, "bymd5", md5query.c_str(), 1, stringparamtypes);
  if (PQresultStatus(pgres) != PGRES_COMMAND_OK){
         throw OcfaException("Prepare failed: " + md5query + ":" + PQresultErrorMessage(pgres));
  }
  PQclear(pgres);

  // preparation for "Fives Design specification" pseudocode
  std::string sourcescategories( "select * from categorizeditemsdigest where md5=$2 or sha1=$1 ");
  pgres = PQprepare(mConnection, "sourcescategories", sourcescategories.c_str(), 2, stringparamtypes);
  if (PQresultStatus(pgres) != PGRES_COMMAND_OK){
         throw OcfaException("Prepare failed: " + sourcescategories + ":" + PQresultErrorMessage(pgres));
  }
  PQclear(pgres);

  std::string qnmbrsets("select count(*) as nmbr_sets from externalsource");
  pgres = PQexec(mConnection, qnmbrsets.c_str());
  if (PQresultStatus(pgres) == PGRES_TUPLES_OK)
  { 
    int colnum = PQfnumber(pgres, "nmbr_sets");
    mNumberSets = atoi(PQgetvalue(pgres, 0, colnum));  
    PQclear(pgres);
  } else {
    throw OcfaException(std::string("Could not get number of sources: ")+std::string(PQresultErrorMessage(pgres)));    
  }
}


PgDigestModule::~PgDigestModule() {
	getLogStream(LOG_NOTICE) << "done with closing databases\n";
}

void   PgDigestModule::processEvidence() {
 
  logModule(LOG_DEBUG, "Getting sha1 and md5");
  string sha1=getDigestSHA1();
  string md5=getDigestMD5();
  getLogStream(LOG_DEBUG) << "sha1 " << sha1 << endl;
  getLogStream(LOG_DEBUG) << "md5 " << md5 << endl;
  const char *params[2];
  params[0] = sha1.c_str();
  params[1] = md5.c_str();
  const char *params2[1];
  params2[0] = md5.c_str();

  // as per "Fives Design specification" pseudocode
  PGresult *pgres = PQexecPrepared(mConnection, "sourcescategories", 2, params, 0, 0, 0);
  if (PQresultStatus(pgres) == PGRES_TUPLES_OK) {
     if (PQntuples(pgres) > 0){
       for (int hashset=0; hashset < mNumberSets; hashset++){
          std::ostringstream colname;
          colname << "categoryname_" << hashset;
          int colnum = PQfnumber(pgres, colname.str().c_str());
          char *entry = PQgetvalue(pgres, 0, colnum);
          if (strcmp(entry,"")!=0){
            std::ostringstream meta;
            meta << "digestresult_" << hashset;
            setMeta(meta.str(), ocfa::misc::Scalar(entry)); 
          }
       }
     }
  }
  PQclear(pgres);

  // until the end of this function, this code might be obsolete
  pgres = PQexecPrepared(mConnection, "bysha1", 1, params, 0, 0, 0);
  if (PQresultStatus(pgres) == PGRES_TUPLES_OK) {
     if (PQntuples(pgres) == 0) {
        PQclear(pgres);
        pgres = PQexecPrepared(mConnection, "bymd5", 1, params2, 0, 0, 0);
     }
  }
  if (PQresultStatus(pgres) == PGRES_TUPLES_OK) {
     if (PQntuples(pgres) > 0) {
         ArrayMetaValue *headers = new ArrayMetaValue(4);
         Scalar headercy("country");
         Scalar headername("name");
         Scalar headercol("col");
         Scalar headercat("cat");
         headers->addMetaValue(headercy);
         headers->addMetaValue(headername);
         headers->addMetaValue(headercol);
         headers->addMetaValue(headercat);
         TableMetaValue classifications(&headers);
         for (int myrow=0; myrow < PQntuples(pgres); myrow++) {
            ArrayMetaValue *classification = new ArrayMetaValue(4);
            for (int mycol=0; mycol < 4; mycol++) {
               string celdata = PQgetvalue(pgres, myrow, mycol);      
               ocfa::misc::Scalar sceldata(celdata);
               classification->addMetaValue(sceldata);
            }
            classifications.addRow(&classification);
         }
         setMeta("classifications", classifications);    
         setMeta("classified", ocfa::misc::Scalar("true","LATIN1"));
     } else {
         setMeta("classified", ocfa::misc::Scalar("false","LATIN1"));
     }
  } else {
    OcfaLogger::Instance()->syslog(LOG_ERR, "PgDigestModule::processEvidence ") << "Problem with sql execution" << std::endl;
  }
  return ;
}

int main(int ,char **) {
   PgDigestModule *digest;
 try { digest=new PgDigestModule();
 } catch(OcfaException &ex) {
   ex.logWhat();
   return 2;
 }
 catch (...) {
   OcfaLogger::Instance()->syslog(LOG_ERR, "main ") << "Exception in digest constructor\n";
   return 1;
 }
 try {
   digest->run();
   OcfaLogger::Instance()->syslog(LOG_ERR, "main ") << "module returned, what's that?\n";
 } catch(OcfaException &ex) {
   digest->logModule(LOG_CRIT, string("Ocfa Exeption cougth: ") + ex.what());
 } catch(...) {
   digest->logModule(LOG_CRIT, "Non Ocfa Exeption cougth");
 }
 delete digest;
 return 1;
}

