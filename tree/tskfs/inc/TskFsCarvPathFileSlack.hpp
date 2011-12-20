#ifndef TSKFSCARVPATHFILESLACK_HPP
#define TSKFSCARVPATHFILESLACK_HPP
#include "treegraph.hpp"
#include "misc.hpp"
#include <tsk3/libtsk.h>
#include <string>
extern "C" {
#include <libcarvpath.h>
}
namespace ocfa {
  namespace tsk {
     //Files that are not exectly a whole time the filesystem block size have file slack.
     //This node represents file slack that can be represented as a CarvPath. 
     class TskFsCarvPathFileSlack: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          TskFsCarvPathFileSlack(std::string path,size_t size,bool verbosemeta);
          ~TskFsCarvPathFileSlack();
          std::string getName(){ return "fileslack";}
          void takeMetaMap(std::map < std::string, misc::MetaValue * >**map) ;
          bool hasContent() { return true;} 
          off_t getSize(){ return mSize; }
          std::string getSoftLinkablePath(ocfa::misc::DigestPair **) { return mCarvPath;}
          void unlinkOnDestruct(){}
        protected:
          bool mVerboseMeta;  // Generate verbose meta data or not.
          std::map < std::string, misc::MetaValue * > *mMeta;
          size_t mSize; // the size of the data attribute file slack.
          std::string mCarvPath; // the carvpath for the data attribute file slack.
     }; 
  }
}
#endif
