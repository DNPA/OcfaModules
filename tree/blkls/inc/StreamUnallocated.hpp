#ifndef STREAMUNALLOCATED_HPP
#define STREAMUNALLOCATED_HPP
#include "treegraph.hpp"
#include "misc.hpp"
#include <tsk3/libtsk.h>
#include <string>
namespace ocfa {
  namespace tsk {
     class StreamUnallocated: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          StreamUnallocated(std::string filename, off_t offset, off_t size, bool softlinkable);
          virtual ~StreamUnallocated();
	  bool hasContent() { return (mSize > 0) ;}
          off_t getSize() { return mSize;}
          std::string getSoftLinkablePath(ocfa::misc::DigestPair **); 
          std::string getHardLinkablePath(std::string targetbasepath,ocfa::misc::DigestPair **) { return ""; }
          bool hasSubEntities() { return false;}
          void resetSubEntityIterator() { /* do nothing */}
          bool nextSubEntity() { throw ocfa::misc::OcfaException("Current node does not have SubEntities.",this);  }
          void getCurrentSubEntity(TreeGraphNode ** subent) { throw ocfa::misc::OcfaException("Current node does not have SubEntities.",this);  }
          void streamToOutput(ocfa::misc::AbstractWriteFacet &writefacet);
          std::string getCurrentSubEntityRelation() { return "";}
          std::string getName(); 
          void takeMetaMap(std::map < std::string, misc::MetaValue * >**map) ;
          void unlinkOnDestruct(){ /* do nothing */ }
        private:
          std::map < std::string, misc::MetaValue * > *mMeta;
          off_t mSize, mOffset;
          std::string mName;
          bool mHasContent, mSoftlinkable;
          void initMetaMap();
     }; 
  }
}
#endif
