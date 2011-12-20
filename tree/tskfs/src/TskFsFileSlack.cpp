#include "TskFsFileSlack.hpp"
namespace ocfa {
  namespace tsk {
          //The simple constructor from slack data.
          TskFsFileSlack::TskFsFileSlack(std::string slackdata,size_t slacksize,bool verbosemeta):OcfaObject("TskFsFileSlack","tskfs"),mVerboseMeta(verbosemeta),mSlackData(slackdata),mSize(slacksize),mStreamIndex(0){
             mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();
             ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("slack")));
             ocfa::misc::metautil::addMetaToMap(mMeta,"size", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(mSize)));
          }
          //The special constructor.
          TskFsFileSlack::TskFsFileSlack(TSK_FS_INFO *fs , const TSK_FS_ATTR *attribute,off_t streamsize):OcfaObject("TskFsFileSlack","tskfs"),mSize(0),mSlackData(""),mStreamIndex(0){
             mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();
             ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("slack")));
             //Fetch the fileslack itself by tediously walking the whole file its data blocks up to the last one. 
             TSK_FS_ATTR_RUN *run=attribute->nrd.run;
             bool goon=(run != 0);
             off_t blocksize=attribute->fs_file->fs_info->block_size;  //use a local variable for the filesystem block size as to get a shorthand for it.
             off_t slacksize=(blocksize - (streamsize % blocksize)) % blocksize; //Determine the size of the slack.
             if (goon) { 
                while (goon) { //step over all but the last data block.
                  if ((run != attribute->nrd.run_end)&& (run->next)) {
                    run=run->next;
                  } else {
                    goon=false;
                  }
                }
                mSize=slacksize; //set the slack size.
                ocfa::misc::metautil::addMetaToMap(mMeta,"size", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(mSize)));
                if (slacksize > 0) {
                  //Fill the mSlackData string with slack from the last block.
                  off_t address=run->addr*blocksize+run->len*blocksize-slacksize;
                  char slackData[4096];
                  tsk_fs_read(fs,address,slackData,slacksize);
                  mSlackData=std::string(slackData,slacksize);
                }
             }
          }

          TskFsFileSlack::~TskFsFileSlack() {
             if (mMeta) {
               map < string, misc::MetaValue * >::const_iterator p;
               for (p = mMeta->begin(); p != mMeta->end(); ++p) {
                 delete p->second;
               }
               delete mMeta;
               mMeta=0;
             }
          }
          void TskFsFileSlack::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
             if (*map == 0) {
                  if (mMeta == 0) {
                     throw ocfa::misc::OcfaException("takeMetaMap invoked for the second time",this);
                  }
                  *map=mMeta;
                  mMeta=0;
             } else {
                 throw ocfa::misc::OcfaException("takeMetaMap invoked with nonn NULLed map handle.",this);
             }
          }
          size_t TskFsFileSlack::streamRead(char *buf, size_t count) { 
             size_t remainingsize= mSize - mStreamIndex; 
             if (remainingsize > 0) {
               size_t readsize = (remainingsize > count) ? count : remainingsize;
               std::string data=mSlackData.substr(mStreamIndex,readsize);
               memcpy((void *) buf,data.c_str() , readsize);
               mStreamIndex += readsize;
               return readsize;
             } else {
               return 0;
             }           
          }
  }
}
