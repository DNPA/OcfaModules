#ifndef TSKFSDIR_HPP
#define TSKFSDIR_HPP
#include "treegraph.hpp"
#include "misc.hpp"
#include <tsk3/libtsk.h>
#include "TskFsInode.hpp"
#include "LoopDetector.hpp"
#include <string>
#include <map>
extern "C" {
#include <libcarvpath.h>
}
namespace ocfa {
  namespace tsk {
     //This node represents a directory on the filesystem.
     class TskFsDirectory: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          TskFsDirectory(TSK_FS_INFO *fs,carvpath_entity *cp,TSK_INUM_T inum,std::string name,TSK_FS_NAME *tskname,AbstractLoopDetector &parentdetector,std::string timesource,bool doslack,bool verbosemeta);
          ~TskFsDirectory();
          void streamToOutput(ocfa::misc::AbstractWriteFacet &writefacet);
          bool hasSubEntities();
          void resetSubEntityIterator();
          bool nextSubEntity() ;
          void getCurrentSubEntity(TreeGraphNode ** subent) ;
          std::string getCurrentSubEntityRelation() ;
          std::string getName();
          void takeMetaMap(std::map < std::string, misc::MetaValue * >**map) ;
          void unlinkOnDestruct(){}           
        private:
          bool mVerboseMeta; // Generate verbose meta data or not.
          std::map < std::string, misc::MetaValue * > *mMeta;
          TSK_FS_INFO *mFileSystem;          
          carvpath_entity *mCarvPathTop;
          std::string mName; // The name of the directory.
          size_t mEntryIndex; // The index of the current directory-entry sub entity. 
          TSK_FS_DIR *mDirectory; // The sleuthkit lib directory structure.
          TskFsInode *mAsFile; // The inode part of the directory.
          LoopDetector mLoopDetector; //Loop detector for detecting child inode's that are also ancestors.
          std::string mTimeSource; //The unique time source for this filesystem.
          bool mDoSlack; //Flag indicating if we should produce seperate node's for file slack.
          bool mAltStreamMode;        // Boolean indicating that directory child nodes have all been given, and alternate streams child node's are now being returned. 
     }; 
  }
}
#endif


