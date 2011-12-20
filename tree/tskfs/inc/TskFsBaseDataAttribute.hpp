#ifndef TSKFSBASEDATAATTRIBUTE_HPP
#define TSKFSBASEDATAATTRIBUTE_HPP
#include "treegraph.hpp"
#include "misc.hpp"
#include <string>
#include <tsk3/libtsk.h>
namespace ocfa {
  namespace tsk {
     //This is the common ancestor class for different types of data attributes.
     class TskFsBaseDataAttribute: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          TskFsBaseDataAttribute( TSK_FS_INFO *fs,const TSK_FS_ATTR *attribute,bool doslack,off_t realsize,bool verbosemeta);
          ~TskFsBaseDataAttribute();
          std::string getName();
          void takeMetaMap(std::map < std::string, misc::MetaValue * >**map);
          off_t getSize();
          bool hasContent();
	  std::string getCurrentSubEntityRelation() { return "slack";}
          void unlinkOnDestruct(){}
        protected:
          bool mVerboseMeta; // Generate verbose meta data or not.
          std::map < std::string, misc::MetaValue * > *mMeta; 
          const TSK_FS_ATTR *mAttribute;
          TSK_FS_INFO *mFs;
          bool mDoSlack;  // Indicates if the node should considder file slack as a sub node.
          off_t mRealSize; // The size of the data attribute.
          std::string mName;
     }; 
  }
}
#endif
