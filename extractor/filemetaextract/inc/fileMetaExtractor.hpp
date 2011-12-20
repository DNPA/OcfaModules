#ifndef INCLUDED_FILEMETAEXTRACTOR_H
#define INCLUDED_FILEMETAEXTRACTOR_H

#include<facade/EvidenceFileAccessor.hpp>
#include<extractor.h>



using namespace ocfa::facade;
namespace ocfa {
namespace module {
  class fileMetaExtractor : public EvidenceFileAccessor {
    private:
        EXTRACTOR_ExtractorList *_extractors;
        
    public:
	fileMetaExtractor();
	~fileMetaExtractor();
	virtual void processEvidence();
  }; 
}

}

#endif

