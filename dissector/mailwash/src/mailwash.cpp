
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

#include <mailwash.hpp>
#include <stdexcept>
#include <vector>
#include <boost/tokenizer.hpp>
#include "../../../binaries.hpp"
using namespace
  ocfa::facade;
using namespace
  ocfa::evidence;
using namespace
  ocfa::misc;
void
mailwashDissector::processEvidence ()
{
  FILE *command;
  char *line;
  size_t size;
  char cline[1024];

  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject ();
  string wd = getWorkDir ();
  if (evidence == 0)
    throw OcfaException ("Evidence without evidence store", this);
  snprintf (cline, 1023,
	    "%s %s/sbin/dissectmail2.pl %s %s",
	    BINARY_PERL ,OcfaConfig::Instance()->getValue("ocfaroot").c_str() , evidence->getAsFilePath ().c_str (), wd.c_str ());
  delete evidence;
  cline[1023] = 0;
  getLogStream (LOG_DEBUG) << "Command to execute: " << cline << endl;
  command = popen (cline, "r");
  if (command == 0)
    throw OcfaException ("Unable to fork: mailwash\n");
  size = 0;
  line = 0;
  int count = 0;
  Evidence *bodypart=0;
  Evidence *headerfile=0;
  bool headerok=false;
  std::map<std::string,ocfa::misc::ArrayMetaValue *> *metamap=0;
  while (getline (&line, &size, command) != -1)
    {
      if (line)
	{
          std::string strline(line,strlen(line)-1);
          free(line);
          line=0;
          size=0;
	  getLogStream (LOG_DEBUG) << "OUTPUT: " << strline;
          boost::char_separator<char> collon(":");
          boost::tokenizer< boost::char_separator<char> > tok(strline,collon);
          bool goodline=false;
          std::string command="";
          std::string key="";
          std::string val="";
          std::string extendedval="";
          std::string src="";
          boost::tokenizer< boost::char_separator<char> >::iterator iter = tok.begin();
          if (!(iter == tok.end()) && (*iter == "MAILWASH")) {
             iter++;
             if (iter != tok.end()) {
                command=*iter;
                iter++;
                if (!(iter == tok.end())) {
                   key=*iter;
                   val="";
                   src="";
                   iter++;
                   goodline=true;
                   if (!(iter == tok.end())) {
                      val=*iter;
                      extendedval=val;
                      iter++;
                      if (!(iter == tok.end())) {
                        src=*iter;
                        extendedval += ":";
                        extendedval += src;
                        iter++;
                        while(!(iter == tok.end())) {
                           extendedval += ":";
                           extendedval += *iter;
                           iter++;
                        }
                      }
                   }
                }
             }
          }         
          if (goodline) {
              count++;
	      if (command == "START") {
                 getLogStream (LOG_DEBUG) << "start of mail message: " << strline;
		 if (metamap) {
		   std::map<std::string,ocfa::misc::ArrayMetaValue *>::iterator iter;
		   for (iter = metamap->begin(); iter != metamap->end(); ++iter) {
                       delete iter->second;
		   }
                   delete metamap;
		 }
		 metamap=new  std::map<std::string,ocfa::misc::ArrayMetaValue *>();
	      }
	      else if (command == "END") {
                 getLogStream (LOG_DEBUG) << "end of mail message: " << strline;
		 if (bodypart) {
                     submitEvidence (bodypart);
		     delete bodypart;
		     bodypart=0;
		 }
                 std::map<std::string,ocfa::misc::ArrayMetaValue *>::iterator iter;
                 if (metamap) {
                    std::map<std::string,ocfa::misc::ArrayMetaValue *>::iterator iter;
                    for (iter = metamap->begin(); iter != metamap->end(); ++iter) {
                       setMeta(iter->first,*(iter->second));
                       delete iter->second;
                    } 
                    delete metamap;
                    metamap=0;
                 }
	      }
	      else if (command == "HEADERS") {
		 headerfile=derive(key,Scalar(key,"LATIN1"),"headers");
		 ScalarMetaValue *mv=new ScalarMetaValue(Scalar("text/x-ocfa-mailheaders"));
                 headerfile->getActiveJob()->setMeta(string("mimetype"),mv);
		 delete mv;
		 mv=new ScalarMetaValue(Scalar("text"));
		 headerfile->getActiveJob()->setMeta(string("mimetop"),mv);
		 delete mv;
                 submitEvidence (headerfile);
                 delete headerfile;
                 headerfile=0; 
	      }
	      else if (command == "BODY") {
                 if (bodypart) {
		    submitEvidence (bodypart);
		    delete bodypart;
		    bodypart=0;
		 }
		 bodypart=derive(key,Scalar(key,"LATIN1"),"filedirentry");
	      }
	      else if (command == "BODYSTRING") {
		 if (bodypart) {
	            ScalarMetaValue *mv=new ScalarMetaValue(Scalar(extendedval,"LATIN1"));
                    bodypart->getActiveJob()->setMeta(key,mv);
		    delete mv;
		 } else {
                    getLogStream (LOG_WARNING) << "BODYMIMETYPE with no current bodypart pressent\n";
		 }
	      }
	      else if (command == "STRING") {
		   ScalarMetaValue *mv=new ScalarMetaValue(Scalar(extendedval,"LATIN1"));
                   setMeta(key,*mv);
		   if (key == "From") 
		      headerok=true;
		   delete mv;
	      } 
              else if (command == "UTFSTRING") {
                   ScalarMetaValue *mv=new ScalarMetaValue(Scalar(extendedval));
                   setMeta(key,*mv);
		   if (key == "From")
		      headerok=true;
                   delete mv;
              }
	      else if (command == "ARRAY") {
		     Scalar mv(extendedval,"LATIN1");
                     if ((*metamap)[key] == 0) {
                        (*metamap)[key] = new  ArrayMetaValue();
		     }
		     (*metamap)[key]->addMetaValue(mv);
	      }
              else if (command == "UTFARRAY") {
                    Scalar mv(extendedval);
                    if ((*metamap)[key] == 0) {
                       (*metamap)[key] = new  ArrayMetaValue();
                    }
                    (*metamap)[key]->addMetaValue(mv);
              }
	      else if (command == "DATETIME") { 
	         
		 const DateTime *dt;
		 ScalarMetaValue *mv;
                 try {
		    dt=new DateTime(atol(val.c_str()),src);
		 } catch (...) {
		    getLogStream (LOG_CRIT) << "Problem with DateTime creation DateTime(" << val << "," << src << ")" << endl;
                    throw OcfaException("DATETIME : exception in constructor of DateTime",this);
		 }
		 try {
		   mv=new ScalarMetaValue(Scalar(&dt));
		 } catch (...) {
                   getLogStream (LOG_CRIT) << "Problem with ScalarMetaValue creation : (" << val << "," << src << ")" << endl;
		 }
		 try {
                   setMeta(key,*mv);
		 } catch (...) {
                   getLogStream (LOG_CRIT) << "Problem with setMeta : (" << val << "," << src << ")" << endl;
		 }
		 delete mv;
	      }
	      else {
                 getLogStream (LOG_WARNING) << "unparsed command: " << command << endl;
	      }
	}
     }
     size=0;
  }
  if (headerok == false) {
      ScalarMetaValue *mv=new ScalarMetaValue(Scalar("true","LATIN1"));
      setMeta("invalidheaders",*mv);
      delete mv;
  }
  if (metamap) {
	    std::map<std::string,ocfa::misc::ArrayMetaValue *>::iterator iter;
 	    for (iter = metamap->begin(); iter != metamap->end(); ++iter) {
		headerfile->getActiveJob()->setMeta(iter->first,iter->second);
                delete iter->second;
            }
  }
  if (bodypart) {
        submitEvidence (bodypart);
        delete(bodypart);
  }
  pclose (command);
  if (count == 0) {
        getLogStream (LOG_ERR) << "no output from command \n";
  }

  return;
}



mailwashDissector::~mailwashDissector ()
{
}

int
main (int, char *[])
{
  mailwashDissector *mailwash = 0;
  try
  {
    mailwash = new mailwashDissector ();
  } catch (OcfaException &ex)
  {
    cerr << "Ocfa Exeption cougth in module constructor: " << ex.
      what () << "\n";
    return 2;
  }
  try
  {
    mailwash->run ();
  }
  catch (OcfaException &ex)
  {
    mailwash->getLogStream (LOG_CRIT) << "Ocfa Exeption cougth: " << ex.
      what () << "\n";
  }
  catch (...)
  {
    mailwash->getLogStream (LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  mailwash->PrintObjCount ();
  delete mailwash;
  return 1;
}
