#include "TskFsResidentDataAttribute.hpp"
#include "TskFsFileSlack.hpp" 
#include <string.h>

namespace ocfa {
  namespace tsk {
          TskFsResidentDataAttribute::TskFsResidentDataAttribute( TSK_FS_INFO *fs,const TSK_FS_ATTR *attribute,bool doslack,off_t realsize,bool verbosemeta):TskFsBaseDataAttribute(fs,attribute,doslack,realsize,verbosemeta) {
             updateTypeName("TskFsResidentDataAttribute");
             ocfa::misc::metautil::addMetaToMap(mMeta,"resident", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(1)));
          }

          TskFsResidentDataAttribute::~TskFsResidentDataAttribute(){
          }
          
          void TskFsResidentDataAttribute::getCurrentSubEntity(TreeGraphNode ** subent) {
             *subent = new TskFsFileSlack(std::string((char *) (mAttribute->rd.buf+mRealSize),1024-mRealSize),1024-mRealSize,mVerboseMeta);
             return;
          }
 
          std::string TskFsResidentDataAttribute::getSoftLinkablePath(ocfa::misc::DigestPair **) {
             //FIXME: currently we seem unable to extract the location of the resident data from the image using the libtsk3 API. Need to look for a way arround, or wait for a fix in the libtsk3 API.
             return "";
          }

          bool TskFsResidentDataAttribute::hasSubEntities(){
               if (mDoSlack && (mRealSize < 1024)) {
                 return true;
               }
               return false;
          }
      
          size_t TskFsResidentDataAttribute::streamRead(char *buf, size_t count) {
            size_t remainingsize= mRealSize - mStreamindex; //First look how much of data remains unread.
            if (remainingsize > 0) {
               size_t readsize = (remainingsize > count) ? count : remainingsize; //Determine how much we will try to read.
               void *start=mAttribute->rd.buf + mStreamindex;  //Determine where our read should start in the read buffer.
               memcpy((void *) buf, start, readsize); //Copy the data to the designated buffer.
               mStreamindex += readsize; //Update our read pointer.
               return readsize; //Return the amount of data read.
            } else {
               return 0; //Nothing left to read.
            }  
          }
  }
}
