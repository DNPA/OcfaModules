#include<string>
#include"store/EvidenceStoreEntity.hpp"
#include"misc/OcfaLogger.hpp"
#include "fileMetaExtractor.hpp"  
	 
using namespace ocfa::module;
using namespace ocfa::misc;

fileMetaExtractor::fileMetaExtractor(): EvidenceFileAccessor("file","core")
{
   
   _extractors = EXTRACTOR_loadDefaultLibraries();
   
}
     
fileMetaExtractor::~fileMetaExtractor()
{
  EXTRACTOR_removeAll(_extractors);
}

void fileMetaExtractor::processEvidence()
{
   ocfa::store::EvidenceStoreEntity* evidence = fetchEvidenceStoreObject();
   std::string filename = evidence->getAsFilePath();
   getLogStream(LOG_NOTICE) << "Processing " << filename << endl;

   for (EXTRACTOR_KeywordList *klist = EXTRACTOR_getKeywords (_extractors, filename.c_str()); klist != 0; klist = klist->next){
        setMeta(EXTRACTOR_getKeywordTypeAsString(klist->keywordType), klist->keyword);
        //getLogStream(LOG_NOTICE) << "Extracted " << klist->keyword << endl;
   }

   

}

int main(int argc, char *argv[])
{
  fileMetaExtractor *properties;
  try {
    properties = new fileMetaExtractor();
    properties->run();
  } catch(...){
    return 1;
  }
  delete properties;
  return 0;
} 
