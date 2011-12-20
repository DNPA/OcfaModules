#ifndef TSKFSCARVPATHDATAATTRIBUTE_HPP
#define TSKFSCARVPATHDATAATTRIBUTE_HPP
#include "treegraph.hpp"
#include "misc.hpp"
#include <tsk3/libtsk.h>
#include <string>
extern "C" {
#include <libcarvpath.h>
}
#include <TskFsBaseDataAttribute.hpp>
namespace ocfa {
  namespace tsk {
     //Data attributes are the content carying part(s) of files.
     //This class represents a non resident data attribute that can be represented as a CarvPath. 
     class TskFsCarvPathDataAttribute: public TskFsBaseDataAttribute {
        public:
          TskFsCarvPathDataAttribute( TSK_FS_INFO *fs,carvpath_entity *cp,const TSK_FS_ATTR *attribute,bool doslack,off_t realsize,bool verbosemeta);
          ~TskFsCarvPathDataAttribute();
          std::string getSoftLinkablePath(ocfa::misc::DigestPair **);
          void resetSubEntityIterator(){}
          bool hasSubEntities();
          bool nextSubEntity() { return false; }
          void getCurrentSubEntity(TreeGraphNode ** subent) ;
        private:
          //This static method is used as callback in invocation of the libtsk API.
          static TSK_WALK_RET_ENUM streamdata_action(TSK_FS_FILE *file, TSK_OFF_T offset, TSK_DADDR_T daddr, char *buf , size_t size, TSK_FS_BLOCK_FLAG_ENUM flags, void *ptr);
          std::string mCarvPath;   //A carvpath string for the data attribute.
          std::string mSlackCarvPath; // A carvpath string for the slack portion of the data attribute.
          size_t mSlackSize; // The size of the slack portion of the data attribute.
     }; 
  }
}
#endif
