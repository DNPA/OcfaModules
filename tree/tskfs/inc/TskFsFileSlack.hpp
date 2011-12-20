#ifndef TSKFSFILESLACK_HPP
#define TSKFSFILESLACK_HPP
#include "treegraph.hpp"
#include "misc.hpp"
#include <string>
#include <tsk3/libtsk.h>
namespace ocfa {
  namespace tsk {
     class TskFsFileSlack: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          TskFsFileSlack(std::string slackdata,size_t slacksize,bool verbosemeta);
          TskFsFileSlack(TSK_FS_INFO *fs , const TSK_FS_ATTR *attribute,off_t attrsize);
          ~TskFsFileSlack();
          std::string getName(){ return "fileslack";}
          void takeMetaMap(std::map < std::string, misc::MetaValue * >**map) ;
          bool hasContent() { return true;} 
          off_t getSize(){ return mSize; }
          void openStream(){mStreamIndex=0;}
          void closeStream(){}
          size_t streamRead(char *buf, size_t count);
          void unlinkOnDestruct(){}
        protected:
          bool mVerboseMeta; //Indicates if our meta data should be verbose.
          std::map < std::string, misc::MetaValue * > *mMeta;
          size_t mSize; //Size of the basic data attribut file slack.
          std::string mSlackData; //The raw fileslack content.
          off_t mStreamIndex; //The index for reading the slack data.
     }; 
  }
}
#endif
