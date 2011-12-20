#ifndef TSKFSSTREAMDATAATTRIBUTE_HPP
#define TSKFSSTREAMDATAATTRIBUTE_HPP
#include "treegraph.hpp"
#include "misc.hpp"
#include <tsk3/libtsk.h>
#include <string>
#include <TskFsBaseDataAttribute.hpp>
namespace ocfa {
  namespace tsk {
     //This is the data attribute type for non resident non carvpath data streams.
     class TskFsStreamDataAttribute: public TskFsBaseDataAttribute {
        public:
          TskFsStreamDataAttribute(TSK_FS_INFO *fs,const TSK_FS_ATTR *attribute,bool doslack,off_t realsize,bool verbosemeta);
          ~TskFsStreamDataAttribute();
          void streamToOutput(ocfa::misc::AbstractWriteFacet &writefacet);
          void resetSubEntityIterator(){}
          bool hasSubEntities();
          bool nextSubEntity() {
              return false;
          }
          void getCurrentSubEntity(TreeGraphNode ** subent) ;
          std::string getCurrentSubEntityRelation() { 
              return "slack";
          }
        private:
          static TSK_WALK_RET_ENUM streamdata_action(TSK_FS_FILE *file, TSK_OFF_T offset, TSK_DADDR_T daddr, char *buf , size_t size, TSK_FS_BLOCK_FLAG_ENUM flags, void *ptr);
     }; 
  }
}
#endif
