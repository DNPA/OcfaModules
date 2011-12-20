#include "StreamPartition.hpp"
#include <boost/lexical_cast.hpp>
namespace ocfa {
  namespace tsk {
    StreamPartition::StreamPartition(TSK_IMG_INFO* image, off_t offset, off_t len, std::string description, bool allocated, bool meta,TSK_PNUM_T pnum):
                                                                                                                  ocfa::OcfaObject("StreamPartition","mmls"),
                                                                                                                  mMeta(0),
                                                                                                                  mImage(image),
                                                                                                                  mPartitionOffset(offset),
                                                                                                                  mSize(len),
                                                                                                                  mReadOffset(0),
														  mHasContent(!meta)
    {
         mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();
         ocfa::misc::metautil::addMetaToMap(mMeta,"offset", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long) offset)));
         ocfa::misc::metautil::addMetaToMap(mMeta,"partitioninfo", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(description)));
         ocfa::misc::metautil::addMetaToMap(mMeta,"size", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long) mSize)));
         ocfa::misc::metautil::addMetaToMap(mMeta,"datamode", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("stream")));
         if (allocated) {
           ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("diskpartition")));
         } else {
           if (meta) {
              ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("vtmeta")));
              mHasContent=false;
           } else {
               ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("unallocateddiskspace")));
           }
         }
         mName = boost::lexical_cast<std::string>(pnum) + "-" + description;
         mStartTime=time(NULL);
    }

    StreamPartition::~StreamPartition() {
       if (mStartTime) {
         time_t wasted=time(NULL) - mStartTime;
         if (wasted > 1) {
           getLogStream(ocfa::misc::LOG_INFO) << "Wasted "<< wasted << " seconds copying out data (+ calculating digest) that could simply have been designated using CarvFS." << std::endl;           
         }
       }
       if (mMeta) {
            map < string, misc::MetaValue * >::const_iterator p;
            for (p = mMeta->begin(); p != mMeta->end(); ++p) {
               delete p->second;
            }
            delete mMeta;
            mMeta=0;
       }
    }

    size_t StreamPartition::streamRead(char *buf, size_t count)   {
        off_t realoffset=mPartitionOffset+mReadOffset;
        //Make sure not to read past the end of the partition.
        size_t realcount=count;
        off_t dataleft = mSize - mReadOffset;
        if ((((off_t) count) > dataleft) ) {
            realcount= (size_t) dataleft;
        }
        if (realcount == 0) {
           return 0;
        } 
        size_t rdbytes= tsk_img_read(mImage, realoffset,buf,realcount);
        if (rdbytes == (size_t) -1) {
           getLogStream(ocfa::misc::LOG_ERR) << "tsk_img_read returned -1 while requesting for " << realcount << " bytes of data at offset " << realoffset << std::endl;
           return 0;
        }
        mReadOffset+= rdbytes;
        return rdbytes;        
    }   

    void StreamPartition::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
         *map=mMeta;
         mMeta=0;
     } 
  }
}
