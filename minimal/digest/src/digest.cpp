
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

#include "digest.hpp"
#include <store/MetaStoreEntity.hpp>
using namespace ocfa;
using namespace ocfa::evidence;
using namespace ocfa::store;
using namespace ocfa::misc;

DigestModule::DigestModule():XMLAccessor("digest","default"){
	//extern int optind;
	int ret;
	const char *database = "proddb";
	const char *progname = "digest";		/* Program name. */
	/* Accept optional database name. */
	//database = "proddb";
        
	/* Create and initialize database object, open the database. */
	if ((ret = db_create(&productdb, NULL, 0)) != 0) {
		fprintf(stderr,
		    "%s: db_create: %s\n", progname, db_strerror(ret));
		throw OcfaException("Problem creating db handle",this);
	}
	if ((ret = db_create(&digestdb, NULL, 0)) != 0) {
		fprintf(stderr,
		    "%s: db_create: %s\n", progname, db_strerror(ret));
		throw OcfaException("Problem creating db handle",this);
	}
	if ((ret = db_create(&adinfodb, NULL, 0)) != 0) {
		fprintf(stderr,
		    "%s: db_create: %s\n", progname, db_strerror(ret));
		throw OcfaException("Problem creating db handle",this);
	}
	productdb->set_errfile(productdb, stderr);
	digestdb->set_errfile(digestdb, stderr);
	adinfodb->set_errfile(adinfodb, stderr);
	productdb->set_errpfx(productdb, progname);
	digestdb->set_errpfx(digestdb, progname);
	adinfodb->set_errpfx(adinfodb, progname);
	string dbdir=getConfEntry("digestdbdir");
	if (dbdir == "") {
          throw OcfaException("Digest module needs digestdbdir in onfa.conf",this);
	}
	string dbpath=dbdir + "/proddb";
	if ((ret = productdb->open(productdb,
	    NULL, dbpath.c_str(), NULL, DB_BTREE,0, 0664)) != 0) {
		productdb->err(productdb, ret, "%s: open", database);
		getLogStream(LOG_ERR) << "Unable to open database " << dbpath << "\n";
		throw OcfaException(string("Problem opening proddb database ")+dbpath ,this);
	}
	dbpath=dbdir + "/digestdb";
	if ((ret = digestdb->open(digestdb,
	   NULL, dbpath.c_str(), NULL, DB_BTREE,0, 0664)) != 0) {
		digestdb->err(digestdb, ret, "%s: open", database);
		getLogStream(LOG_ERR) << "Unable to open database " << dbpath << "\n";
		throw OcfaException("Problem opening digestdb database",this);
	}
	dbpath=dbdir + "/adinfodb";
	if ((ret = adinfodb->open(adinfodb,
	   NULL, dbpath.c_str(), NULL, DB_BTREE,0, 0664)) != 0) {
		adinfodb->err(adinfodb, ret, "%s: open", database);
		getLogStream(LOG_ERR) << "Unable to open database " << dbpath << "\n";
		throw OcfaException("Problem opening adinfodb database",this);
	}
	dbdir="";
	dbdir=getConfEntry("digestdbdirvar");
	if (dbdir == "") {
		throw OcfaException("Digest module needs digestdbdirvar in onfa.conf",this);
	}
}


DigestModule::~DigestModule() {
	getLogStream(LOG_NOTICE) << "Closing productdb\n";
	(void)productdb->close(productdb, 0);
	getLogStream(LOG_NOTICE) << "Closing digestdb\n";
	(void)digestdb->close(digestdb, 0);
	getLogStream(LOG_NOTICE) << "Closing adinfodb\n";
	(void)adinfodb->close(adinfodb, 0);
	getLogStream(LOG_NOTICE) << "done with closing databases\n";
}

void   DigestModule::processEvidence() {
 
  logModule(LOG_DEBUG, "Getting sha1 and md5");
  string sha1=getDigestSHA1();
  string md5=getDigestMD5();
  getLogStream(LOG_DEBUG) << "sha1 " << sha1 << endl;
  getLogStream(LOG_DEBUG) << "md5 " << md5 << endl;
  
  DBT key, data;
  int rval;
  char buf[1024];
  int size;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));
  //FIXME: this is nasty !!
  key.data = reinterpret_cast<void *>(const_cast<char *>( sha1.c_str()));
  key.size=sha1.length();
  rval=digestdb->get(digestdb,NULL,&key,&data,0);
  string digest=sha1;
  string srcandfilename;
  if (rval > -1) { // sha1 found
    size=data.size;
    if (size > 1023) size=1023;
    strncpy(buf,reinterpret_cast<char *>(data.data),size);
    buf[size]=0;
    srcandfilename=buf;
  } else { // sha1 not found, try md5
    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));
    //FIXME: this is nasty !!
    key.data = const_cast<char *>(md5.c_str());
    key.size=md5.length();
    rval=digestdb->get(digestdb,NULL,&key,&data,0);
    if (rval > -1) { // md5 found
       size=data.size;
       if (size > 1023) size=1023;
       strncpy(buf,reinterpret_cast<char *>(data.data),size);
       buf[size]=0;
       srcandfilename=buf;
    } else { // sha1 and md5 not found
       setMeta("digestsource",Scalar("UNDEF"));
       return ;
    }
  }
  string digestsource="UNDEF";
  string filename="";
  int i = srcandfilename.find_last_of(":");
  int l = strlen(srcandfilename.c_str());
  if (l > 0 && i >= 0){
       filename  = srcandfilename.substr(i+1, l-i-1);
       if (i > 0) {
	       digestsource=srcandfilename.substr(0,i);
	       logModule(LOG_NOTICE,"digestsource=" + digestsource);
	       setMeta("digestsource", Scalar(digestsource));
       }
       setMeta("referencename",Scalar(filename));
  }
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));
  string skey=digestsource + ":" + digest;
  //FIXME: this is nasty !!
  key.data=reinterpret_cast<void *>(const_cast<char *>( skey.c_str()));
  key.size=skey.length();
  rval=adinfodb->get(adinfodb,NULL,&key,&data,0);
  if (rval > -1) {
    //if (digestsource == string("NIST")) {
      size=data.size;
      if (size > 1023) size=1023;
      strncpy(buf,reinterpret_cast<char *>(data.data),size);
      buf[size]=0;
      string product=buf;
      memset(&key, 0, sizeof(DBT));
      memset(&data, 0, sizeof(DBT));
      string pkey=product + ":name";
      //FIXME: this is nasty !!
      key.data = reinterpret_cast<void *>(const_cast<char *>(pkey.c_str()));
      key.size=pkey.length();
      rval=adinfodb->get(productdb,NULL,&key,&data,0);
      if (rval > -1) {
	 if (data.size > 0)
           setMeta("nistproduct",Scalar(reinterpret_cast<char *>(data.data),data.size));
      }
      memset(&key, 0, sizeof(DBT));
      memset(&data, 0, sizeof(DBT));
      pkey=product + ":type";
      //FIXME: this is nasty !!
      key.data = reinterpret_cast<void *>(const_cast<char *>( pkey.c_str()));
      key.size=pkey.length();
      rval=adinfodb->get(productdb,NULL,&key,&data,0);
      if (rval > -1) {
	 if (data.size > 0)
	   setMeta("nisttype",Scalar(reinterpret_cast<char *>(data.data),data.size));
      }
      //We could also add 'version', 'os', 'mf' , and 'lang' but these seem not realy relevant at this point.
    //} else {
    //   setMeta("additionalinfo",Scalar((char *)(data.data),data.size));
    //}
  }
  else {
    logModule(LOG_ERR, "OOPS, adinfo not found");
  }

  return ;
}

int main(int ,char **) {
   DigestModule *digest;
 try { digest=new DigestModule();
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

