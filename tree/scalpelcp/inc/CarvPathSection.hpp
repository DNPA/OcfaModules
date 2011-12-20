#ifndef CARVPATHSECTION_HPP
#define CARVPATHSECTION_HPP
#include "treegraph.hpp"
#include "misc.hpp"
#include <string>
#include <vector>
extern "C" {
#include <libcarvpath.h>
}
namespace ocfa {
  namespace scalpel {
     class CarvPathSection: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          CarvPathSection(carvpath_entity *cptop,std::string scalpelline);
          ~CarvPathSection();
	  bool hasContent() {return (mSize >0);}
          off_t getSize() { return mSize;}
          std::string getSoftLinkablePath(ocfa::misc::DigestPair **){ 
              return mCarvPath;
          }
          std::string getName() { return mName;}
          void takeMetaMap(std::map < std::string, misc::MetaValue * >**map) ;
          void unlinkOnDestruct(){/* do nothing */ }           
        private:
          std::map < std::string, misc::MetaValue * > *mMeta;
          std::string mCarvPath;
          std::string mName;
          off_t mSize;
     }; 
  }
}
#endif
