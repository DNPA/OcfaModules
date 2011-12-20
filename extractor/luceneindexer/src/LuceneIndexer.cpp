
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

#include "LuceneIndexer.hpp"
#include <CLucene/analysis/standard/StandardAnalyzer.h>
#include <CLucene/util/Reader.h>
#include <evidence/JobIterator.hpp>
#include <evidence/MetaIterator.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "WashAnalyzer.hpp"
using namespace ocfa::module;
using namespace ocfa::misc;
using namespace ocfa::evidence;
using namespace ocfa;
using namespace std;

CL_NS_USE(document)
CL_NS_USE(analysis::standard)
CL_NS_USE(util)


const int max_dataid_length = 30;
LuceneIndexer::LuceneIndexer() : EvidenceFileAccessor("indexer", "default"), mMaxSize(0),  mIndexWriter(0), mCurrentMetaDataId(""){

  string thePath;
  thePath = getConfEntry("indexdir");
  mAnalyzer = new WashAnalyzer();
  // if no index exists, create a new one.
  string segmentsFile = thePath + "/segments";
  struct stat statInfo;  
  //The max size is 1MB more than the chopsize of strings.
  mMaxSize=(1 + atoi(getConfEntry("chopsize").c_str()))*1024*1024;
  if (mMaxSize < 2*1024*1024)
         mMaxSize=1024*1024*65;  //The default is 65 MB, but a better default should be in the config.
  if (stat(segmentsFile.c_str(), &statInfo) != 0){
  
    if (errno != ENOENT){

      getLogStream(LOG_WARNING) << "couldn't stat " << segmentsFile 
				<< "with unclear error code." << errno << endl;
    }
    // if no index existed, create a new one.
    mIndexWriter = new IndexWriter(thePath.c_str(), mAnalyzer, true, true);
  }
  else {
  // else append the new index to existing one.
    mIndexWriter = new IndexWriter(thePath.c_str(), mAnalyzer, false, true);
  }
  // set max field length, so long files are indexed as well.
  mIndexWriter->setMaxFieldLength(10 * 1000 * 1000);
}


void LuceneIndexer::processEvidence(){

  Document *document = new Document();
  ocfa::store::EvidenceStoreEntity* evidence = fetchEvidenceStoreObject();
  string path = evidence->getAsFilePath();
  if (stat(path.c_str(),&mStat) == -1) {
      throw OcfaException(string("Unable to stat ") + path , this);
  }
  if (mStat.st_size > mMaxSize) {
      getLogStream(LOG_WARNING) << "File to big for clucene indexer " << path << " size=" << mStat.st_size << " skipping file !\n";
      //return; use mark method
  }
  TCHAR dataid[max_dataid_length];
  STRCPY_AtoT(dataid, evidence->getHandle().c_str(),  max_dataid_length);
  Field *docId = new Field(_T("docid"), dataid, true, false, false);
  document->add(*docId);

  // add metadataid for better searching.
  TCHAR metadataid[max_dataid_length];
  STRCPY_AtoT(metadataid, mCurrentMetaDataId.c_str(), max_dataid_length);
  Field *metadataField = new Field(_T("metadataid"), metadataid, true, false, false);
  document->add(*metadataField);

  FileReader *myReader = _CLNEW lucene::util::FileReader(path.c_str(),"ASCII");
  myReader->mark(mMaxSize);

  if (myReader == 0){
    throw OcfaException(string("couldn't get reader  for ") + path, this);
  }


  Field *contents;
  try {
     contents = Field::Text(_T("content"), myReader);
  } catch (...) {
     throw ocfa::misc::OcfaException("Problem creating contents from reader",this);
  }
  try {
     document->add(*contents);
  } catch (...) {
     throw ocfa::misc::OcfaException("Problem assigning contents to document object.",this);
  }
     mIndexWriter->addDocument(document);
  /*} catch (Exception &e) {
     throw ocfa::misc::OcfaException("Problem adding document to the index",this);
  } */
  try {
    _CLDELETE(document);
  } catch (...) {
     throw ocfa::misc::OcfaException("Problem deleting document object",this);
  }
  //delete contents;
  //delete reader;
}

/**
 * Not used at this moment, becaure encoding recognition sucks at the moment in ocfa
 */
bool LuceneIndexer::evidenceHasUtf16(){

  bool hasUtf16 = false;
  JobIterator *iter = getEvidence()->getJobIterator();
  while (iter->next() && !hasUtf16){

    MetaIterator *metaIter = iter->getMetaIterator();
    
    string mime;
    if (metaIter != 0){
      while (metaIter->next() && !hasUtf16){
	
	
	if (metaIter->getName() == "mimetype"){
	  
	  ScalarMetaValue *value = dynamic_cast<ScalarMetaValue*>(metaIter->getMetaVal());
	  if (value == 0){
	    
	    getLogStream(LOG_ERR) << "retrieved non-scalar metavalue for mimetype" 
				  << endl;
	    throw OcfaException("retrieved non-scalar metavalue for mimetype", this);
	  }
	  // cerr << "luceneindexer: checking value " << value->asScalar().asUTF8() << endl;  
	  if (value->asScalar().asUTF8() == "text/x-ocfa-utf16le"){
	    
	    hasUtf16 = true;
	  }
	}
      }
    }
    
  }
  return hasUtf16;
  

}
LuceneIndexer::~LuceneIndexer(){

  if (mIndexWriter != 0){
    getLogStream(LOG_DEBUG) << "deleting indexwriter " << endl; 
    delete mIndexWriter;
  }
  mIndexWriter = 0;
  if (mAnalyzer  != 0){

    getLogStream(LOG_DEBUG) << "deleting analyzer" << endl;
    delete mAnalyzer;
  }
  mAnalyzer = 0;
}


void LuceneIndexer::processEvidenceMessage(const ocfa::message::Message &message){

  mCurrentMetaDataId = message.getContent();
  EvidenceFileAccessor::processEvidenceMessage(message);
  mCurrentMetaDataId = "";

}


int main(int argc, char *argv[]){

    
  LuceneIndexer *luceneIndexer = 0;
  try {
    luceneIndexer = new LuceneIndexer();
  } catch (OcfaException &exn) {
    exn.logWhat();
    return 2;
  } 
  catch(CLuceneError &e){

    OcfaLogger::Instance()->syslog(LOG_ERR, "indexer.default") << "LuceneError: " << e.what() << endl;
    return 3;
  }
  try {
    luceneIndexer->run();
    luceneIndexer->getLogStream(LOG_DEBUG) << "Finished running" << endl;
  } catch (OcfaException &exn) {
    exn.logWhat();
    luceneIndexer->getLogStream(LOG_ERR) << "LuceneIndexerIndexer module: OCFA exception" << exn.what() << endl;
  } catch (CLuceneError &e){

    luceneIndexer->getLogStream(LOG_ERR) 
      << "LuceneIndexerIndexer module: LuceneError" << e.what() << endl;
  }
  
  catch (...) {
    luceneIndexer->getLogStream(LOG_ERR) 
      << "LuceneIndexerIndexer module: unknown exception" << endl;
  }
  if (luceneIndexer != 0){

    delete luceneIndexer;
  }
}
