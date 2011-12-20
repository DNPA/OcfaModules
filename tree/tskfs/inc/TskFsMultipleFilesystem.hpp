#ifndef TSKFSMULTI_HPP
#define TSKFSMULTI_HPP
#include "treegraph.hpp"
#include "misc.hpp"
#include <tsk3/libtsk.h>
#include <string>
#include "LoopDetectorAnchor.hpp"
#include "TskFsTopDir.hpp"
extern "C" {
#include <libcarvpath.h>
}

namespace ocfa {
  namespace tsk {
     //This class represents the top directory of the filesystem.
     class TskFsMultipleFilesystem: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          TskFsMultipleFilesystem(TSK_IMG_INFO *img,carvpath_entity *cptop,int fscount,AbstractLoopDetector &parentdetector,std::string timesource,bool doslack,bool verbosemeta);
          ~TskFsMultipleFilesystem();
	  bool hasContent(){ return false;}
          off_t getSize(){ return 0;}
          void streamToOutput(ocfa::misc::AbstractWriteFacet &writefacet){}
          bool hasSubEntities() { return 1;}
          void resetSubEntityIterator() {mActiveFsNum=0;}
          bool nextSubEntity();
          void getCurrentSubEntity(TreeGraphNode ** subent) ;
          std::string getCurrentSubEntityRelation() { return "partitionentry";}
          std::string getName() { return "multiple-filesystems";}
          void takeMetaMap(std::map < std::string, misc::MetaValue * >**map) ;
          void unlinkOnDestruct(){}           
        private:
          TSK_IMG_INFO *mImageHandle; //The sleuthkit lib structure for the image file information.
          carvpath_entity *mCarvpathTop;
          int mFsCount;
          int mActiveFsNum;
          AbstractLoopDetector &mAnchor; //The anchor for our loop detection subsystem.
          std::string mTimeSource;
          bool mDoSlack;
          bool mVerboseMeta; //Indicates if we should produce verbose meta data.
          std::map < std::string, misc::MetaValue * > *mMeta;
          TSK_FS_INFO *mFileSystem;
     }; 
  }
}
#endif
