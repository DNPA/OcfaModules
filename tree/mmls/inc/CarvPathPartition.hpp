#ifndef CARVPATHPARTITION_HPP
#define CARVPATHPARTITION_HPP
#include "treegraph.hpp"
#include "misc.hpp"
#include <tsk3/libtsk.h>
#include <string>
#include <time.h>
extern "C" {
#include <libcarvpath.h>
}
namespace ocfa {
  namespace tsk {
     class CarvPathPartition: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          CarvPathPartition(carvpath_entity *topcpent, off_t offset, off_t len, std::string description, bool allocated, bool meta,TSK_PNUM_T pnum);
          ~CarvPathPartition();
	  bool hasContent() { return (mPartitionCarvPath != "");}
          off_t getSize() { return mSize; }
          std::string getSoftLinkablePath(ocfa::misc::DigestPair **) { return mPartitionCarvPath;}
          std::string getName() { return mName; }
          void takeMetaMap(std::map < std::string, misc::MetaValue * >**map) ;
          void unlinkOnDestruct(){ /* do nothing */  }
        private:
          std::map < std::string, misc::MetaValue * > *mMeta;
          std::string mPartitionCarvPath;
          off_t mSize;
          std::string mName;
          time_t mStartTime;
     }; 
  }
}
#endif
