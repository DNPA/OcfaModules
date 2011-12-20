#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "TskFsDuplicateDirectory.hpp"
namespace ocfa {
  namespace tsk {

     TskFsDuplicateDirectory::TskFsDuplicateDirectory(TSK_INUM_T inum,std::string name,bool verbosemeta):ocfa::OcfaObject("TskFsDuplicateDirectory","tsk"),mVerboseMeta(verbosemeta),mName(name){
         mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();
         ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("dir")));
         ocfa::misc::metautil::addMetaToMap(mMeta,"specialmarker", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("filesystem-loop")));
         ocfa::misc::metautil::addMetaToMap(mMeta,"isdeleted", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(1)));
         ocfa::misc::metautil::addMetaToMap(mMeta,"valid", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(0)));
         ocfa::misc::metautil::addMetaToMap(mMeta,"duplicate", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(1)));
         ocfa::misc::metautil::addMetaToMap(mMeta,"inode_no", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long int) inum)));
     }

     TskFsDuplicateDirectory::~TskFsDuplicateDirectory(){
         if (mMeta) {
            map < string, misc::MetaValue * >::const_iterator p;
            for (p = mMeta->begin(); p != mMeta->end(); ++p) {
               delete p->second;
            }
            delete mMeta;
            mMeta=0;
         }
        return;
     }

     void TskFsDuplicateDirectory::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
         if (*map == 0) {
           if (mMeta == 0) {
              throw ocfa::misc::OcfaException("takeMetaMap incoked for the second time",this);
           }
           *map=mMeta;
           mMeta=0;
         } else {
                throw ocfa::misc::OcfaException("takeMetaMap invoked with nonn NULLed map handle.",this);
         }
     }

  }
}
