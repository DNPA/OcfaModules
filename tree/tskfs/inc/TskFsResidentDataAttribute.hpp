#ifndef TSKFSRESIDENTDATAATTRIBUTE_HPP
#define TSKFSRESIDENTDATAATTRIBUTE_HPP
#include "treegraph.hpp"
#include "misc.hpp"
#include <tsk3/libtsk.h>
#include <string>
#include <TskFsBaseDataAttribute.hpp>
namespace ocfa {
  namespace tsk {
     class TskFsResidentDataAttribute: public TskFsBaseDataAttribute {
        public:
          TskFsResidentDataAttribute( TSK_FS_INFO *fs,const TSK_FS_ATTR *attribute,bool doslack,off_t realsize,bool verbosemeta);
          ~TskFsResidentDataAttribute();
          void openStream(){mStreamindex=0;}
          void closeStream(){}
          size_t streamRead(char *buf, size_t count);
          std::string getSoftLinkablePath(ocfa::misc::DigestPair **);
          void resetSubEntityIterator(){}
          bool hasSubEntities();
          bool nextSubEntity() {return false;}
          void getCurrentSubEntity(TreeGraphNode ** subent);
        protected:
          off_t mStreamindex; //The reading index of the open resident data attribute.
     }; 
  }
}
#endif
