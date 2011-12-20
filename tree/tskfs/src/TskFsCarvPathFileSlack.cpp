#include "TskFsCarvPathFileSlack.hpp"
namespace ocfa {
  namespace tsk {
          TskFsCarvPathFileSlack::TskFsCarvPathFileSlack(std::string path,size_t size,bool verbosemeta):OcfaObject("TskFsCarvPathFileSlack","tskfs"),mVerboseMeta(verbosemeta),mSize(size),mCarvPath(path){
             mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();
             ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("slack")));
          }
          TskFsCarvPathFileSlack::~TskFsCarvPathFileSlack() {
               //Delete meta info if not fetched.
               if (mMeta) {
                  map < string, misc::MetaValue * >::const_iterator p;
                  for (p = mMeta->begin(); p != mMeta->end(); ++p) {
                    delete p->second;
                  }
                  delete mMeta;
                  mMeta=0;
               }
          }
          void TskFsCarvPathFileSlack::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
            if (*map == 0) {
                  if (mMeta == 0) {
                     throw ocfa::misc::OcfaException("takeMetaMap invoked for the second time",this);
                  }
                  *map=mMeta;
                  mMeta=0;
             } else {
                throw ocfa::misc::OcfaException("takeMetaMap invoked with non NULLed map handle.",this);
             }
          }
  }
}
