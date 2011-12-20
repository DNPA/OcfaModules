#ifndef STREAMPARTITION_HPP
#define STREAMPARTITION_HPP
#include "treegraph.hpp"
#include "misc.hpp"
#include <tsk3/libtsk.h>
#include <string>
#include <time.h>
namespace ocfa {
  namespace tsk {
     class StreamPartition: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          StreamPartition(TSK_IMG_INFO* image, off_t offset, off_t len, std::string description, bool allocated, bool meta,TSK_PNUM_T pnum);
          ~StreamPartition();
	  bool hasContent() { return mHasContent;}
          off_t getSize() { return mSize;}
	  void openStream() { mReadOffset=0; }
          void closeStream() { mReadOffset=0; }
          size_t streamRead(char *buf, size_t count) ;
          std::string getName() { return mName;}
          void takeMetaMap(std::map < std::string, misc::MetaValue * >**map) ;
          void unlinkOnDestruct(){ /* do nothing */ }
        private:
          std::map < std::string, misc::MetaValue * > *mMeta;
          TSK_IMG_INFO* mImage;
          off_t mPartitionOffset; 
          off_t mSize;
          std::string mName;
          off_t mReadOffset;
          bool mHasContent;
          time_t mStartTime;
     }; 
  }
}
#endif
