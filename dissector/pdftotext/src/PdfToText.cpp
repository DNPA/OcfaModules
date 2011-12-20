
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

#include "PdfToText.hpp"
#include <stdexcept>
#include <vector>
#include <iostream>
#include "../../../binaries.hpp"
using namespace ocfa::facade;
using namespace ocfa::evidence;
using namespace ocfa::misc;
PdfToText::PdfToText(): EvidenceDeriveAccessor("pdftotext", "default"){
}
  

void PdfToText::processEvidence()
{
  FILE *command;
  char *line;
  size_t size;
  char cline[1024];
  static char errorBuffer[1024];
  int bufLeft = 1023;
  errorBuffer[0] = '\0';

  getLogStream(LOG_DEBUG) << "received a file" << endl;
  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject();
  string wd = getWorkDir();
  if (evidence == 0)
    throw OcfaException("Evidence without evidence store", this);
  snprintf(cline, 1023, "%s %s %s 2>&1",BINARY_PDFTOTEXT, evidence->getAsFilePath().c_str(),
	   (wd + "/output").c_str());
  cline[1023] = 0;
  delete evidence;
  getLogStream(LOG_INFO) << " going to exectute " << cline << endl;
  command = popen(cline, "r");
  if (command == 0)
    throw OcfaException("Unable to fork: pdftotext %s \n");
  size = 0;
  line = 0;
  while (getline(&line, &size, command) != -1 && bufLeft > 0) {

    strncat(errorBuffer, line, bufLeft);
    bufLeft -= strlen(errorBuffer);
    free(line);
    line = 0;
    size = 0;
  }
  pclose(command);
  FILE *outputFile = fopen((wd + "/output").c_str(), "r");
  getLogStream(LOG_DEBUG) << " outputfile is " << outputFile << endl; 
  if (outputFile != 0){
   
    fclose(outputFile);
    string newFileName = "pdfoutput.txt";
    getLogStream(LOG_DEBUG) << "going to derive " << newFileName << endl;
    Evidence *derived = derive("output", Scalar(newFileName));
    ScalarMetaValue mimetype(Scalar("text/plain"));
    ScalarMetaValue mimetop(Scalar("text"));
    ScalarMetaValue encoding(Scalar("utf-8"));
    derived->getActiveJob()->setMeta("mimetype", &mimetype);
    derived->getActiveJob()->setMeta("mimetop", &mimetop);
    derived->getActiveJob()->setMeta("charset", &encoding);

    getLogStream(LOG_INFO) << " goign to submit " << derived << endl;
    submitEvidence(derived);
    delete derived;
  }
  if (strlen(errorBuffer) == 0){

    getLogStream(LOG_DEBUG) << "remarks were 0" << endl;
    setMeta("remarks", Scalar("none"));
  }
  else {
    setMeta("remarks", Scalar(string(errorBuffer)));
    logEvidence(LOG_ERR,string(errorBuffer));
    getLogStream(LOG_ERR) << "remarks were " << string(errorBuffer) << endl;
  }
  return ;
}

int main(int , char *[])
{
  PdfToText *pdftotext;
  try {
    pdftotext = new PdfToText();

  }catch(OcfaException &ex) {
	  ex.logWhat();
	  cerr << "Ocfa Exeption cougth in module constructor: " << ex.what() << "\n";
	  return 2;
  }
  try {
    pdftotext->run();
  } catch(OcfaException &ex) {
    ex.logWhat();
    pdftotext->getLogStream(LOG_CRIT) << "Ocfa Exeption cougth: " << ex.what() << "\n";
  } catch(...) {
    pdftotext->getLogStream(LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  pdftotext->PrintObjCount();
  delete pdftotext;
  return 1;
}
