
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

#include "selfdigest.hpp"
#include <store/MetaStoreEntity.hpp>
using namespace ocfa;
using namespace ocfa::evidence;
using namespace ocfa::store;
using namespace ocfa::misc;

SelfDigestModule::SelfDigestModule():XMLAccessor("selfdigest","default"){
	//extern int optind;
	int ret;
	const char *progname = "selfdigest";		/* Program name. */

	if ((ret = db_create(&dyncasedb, NULL, 0)) != 0) {
	       fprintf(stderr,
		    "%s: db_create: %s\n", progname, db_strerror(ret));
	       throw OcfaException("Problem creating db handle",this);
	}
	if ((ret = db_create(&dynsrcdb, NULL, 0)) != 0) {
		fprintf(stderr,
		    "%s: db_create: %s\n", progname, db_strerror(ret));
		throw OcfaException("Problem creating db handle",this);
	}
	dyncasedb->set_errfile(dyncasedb, stderr);
	dynsrcdb->set_errfile(dynsrcdb, stderr);
	dyncasedb->set_errpfx(dyncasedb, progname);
	dynsrcdb->set_errpfx(dynsrcdb, progname);
	std::string dbdir="";
	dbdir=getConfEntry("digestdbdirvar");
	if (dbdir == "") {
		throw OcfaException("Digest module needs digestdbdirvar in onfa.conf",this);
	}
	std::string dbpath=dbdir + "/casedb";
	if ((ret = dyncasedb->open(dyncasedb,
	    NULL, dbpath.c_str(), NULL, DB_BTREE,DB_CREATE, 0664)) != 0) {
		dyncasedb->err(dyncasedb, ret, "casedb : open");
		getLogStream(LOG_ERR) << "Unable to open database " << dbpath << "\n";
		throw OcfaException("Problem opening dyncasedb database",this);
	}
	dbpath=dbdir + "/srcdb";
	if ((ret = dynsrcdb->open(dynsrcdb,
	    NULL, dbpath.c_str(), NULL, DB_BTREE,DB_CREATE, 0664)) != 0) {
		dynsrcdb->err(dynsrcdb, ret, "srcdb : open");
		getLogStream(LOG_ERR) << "Unable to open database " << dbpath << "\n";
		throw OcfaException("Problem opening dynsrcdb database",this);
	}
}


SelfDigestModule::~SelfDigestModule() {
	getLogStream(LOG_NOTICE) << "Closing dyncasedb\n";
	(void)dyncasedb->close(dyncasedb, 0);
	getLogStream(LOG_NOTICE) << "Closing dynsrcdb\n";
	(void)dynsrcdb->close(dynsrcdb, 0);
	getLogStream(LOG_NOTICE) << "done with closing databases\n";
}

bool SelfDigestModule::updateDynDB(string caseid,string xmlid,string sha1) {
  dkey=caseid + ":" + sha1;
  string dval=xmlid;
  DBT key,data;
  int rval;
  count=0;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));
  //FIXME: this is nasty !!
  key.data = reinterpret_cast<void *>(const_cast<char *>(dkey.c_str()));
  key.size=dkey.length();
  rval=dyncasedb->get(dyncasedb,NULL,&key,&data,0);
  if (rval > -1) {
     if (data.size == sizeof(int)) {
       count=(reinterpret_cast<int *>(data.data))[0];
     }
  } 
  count++;
  data.data=&count;
  data.size=sizeof(int);
  //FIXME: this is nasty !!
  key.data = reinterpret_cast<void *>(const_cast<char *>(dkey.c_str()));
  key.size=dkey.length();
  if (dyncasedb->put(dyncasedb,NULL,&key,&data,0)!=0) {
  }
  if (count == 1) { 
     memset(&key, 0, sizeof(DBT));
     memset(&data, 0, sizeof(DBT));
     //FIXME: this is nasty !!
     key.data = reinterpret_cast<void *>(const_cast<char *>( dkey.c_str()));
     key.size=dkey.length();
     //FIXME: this is nasty !!
     data.data = reinterpret_cast<void *>(const_cast<char *>( dval.c_str()));
     data.size=dval.length();
     dynsrcdb->put(dynsrcdb,NULL,&key,&data,0);
     return true;
  }
  return false;
}

string SelfDigestModule::getReference() {
   DBT key,data;
   int rval;
   int size;
   char buf[4096];
   string rbuf="NOREF";
   memset(&key, 0, sizeof(DBT));
   memset(&data, 0, sizeof(DBT));
   //FIXME: this is nasty !!
   key.data = reinterpret_cast<void *>(const_cast<char *>( dkey.c_str()));
   key.size=dkey.length();
   rval=dynsrcdb->get(dynsrcdb,NULL,&key,&data,0);
   if (rval > -1) { 
      size=data.size;
      if (size > 4095) size=4095;
      strncpy(buf,reinterpret_cast<char *>(data.data),size);
      buf[size]=0;
      rbuf=buf;
   }
   return rbuf;   
}

void   SelfDigestModule::processEvidence() {
 
  logModule(LOG_DEBUG, "Getting sha1");
  string sha1=getDigestSHA1();
  getLogStream(LOG_DEBUG) << "sha1 " << sha1 << endl; 
  DBT key, data;
  int rval;
  char buf[1024];
  int size;
  string caseid=getInvestigationID();
  ocfa::store::MetaStoreEntity * xmlfile = getMetaXMLStoreHandle();
  if (xmlfile == 0){
	 logModule(LOG_ERR, "no xmlfile in this evidence ");
	 throw OcfaException("0 xmlfile given");
  }
  OcfaHandle xml = xmlfile->getHandle();
  //Note, updateDynDB returns true if the digest is new to the case
  if (updateDynDB(caseid,xml,sha1)) {
	 logModule(LOG_DEBUG, "SHA1 not found. Returning UNDEF for.");
	 setMeta("digestfirst",Scalar("0"));
	 setMeta("digestcount",Scalar(1));
	 logModule(LOG_DEBUG, "New SHA1 that was not found in this case earlier.");
  } else {
	 setMeta("digestfirst",Scalar(getReference()));
	 setMeta("digestcount",Scalar(getCount()));
	 logModule(LOG_DEBUG, "SHA1 found in this case earlier.");
	 
  }
  return ;
}

int main(int ,char **) {
   SelfDigestModule *digest;
 try { digest=new SelfDigestModule();
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

