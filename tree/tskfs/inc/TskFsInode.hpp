#ifndef MMLSTOPNODE_HPP
#define MMLSTOPNODE_HPP
#include "treegraph.hpp"
#include "misc.hpp"
#include <tsk3/libtsk.h>
#include <string>
extern "C" {
#include <libcarvpath.h>
}
namespace ocfa {
  namespace tsk {
     enum attributetype {ATTR_NODATA,ATTR_DATA,ATTR_PRIMARY};
     //This node represents the generic part of both files and directories.
     class TskFsInode: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          TskFsInode( TSK_FS_INFO *fs,carvpath_entity *cp,TSK_INUM_T inum,std::string name,TSK_FS_NAME *nameinfo,std::string timesource,bool doslack,bool verbosemeta);
          ~TskFsInode();
          std::string getName() ;
          void takeMetaMap(std::map < std::string, misc::MetaValue * >**map) ;
          bool hasContent(); 
          off_t getSize();
          void streamToOutput(ocfa::misc::AbstractWriteFacet &writefacet);
          std::string getSoftLinkablePath(ocfa::misc::DigestPair **);
          bool hasSubEntities();
          void resetSubEntityIterator();
          bool nextSubEntity() ;
          void getCurrentSubEntity(TreeGraphNode ** subent) ;
          std::string getCurrentSubEntityRelation() ;
          void unlinkOnDestruct(){}
        private:
	  bool mVerboseMeta;  //Indicates if we should produce verbose meta data.
          void processNameInfoMeta(TSK_FS_NAME* nameinfo); //Helper function for constructor.
          void processFileInfoMeta(TSK_FS_META*, std::string); //Helper function for constructor.
          void processNtfsStandardInfo(const TSK_FS_ATTR*); //Helper function for constructor.
          attributetype processAttribute(const TSK_FS_ATTR*, std::string); //Helper function for constructor.
          std::map < std::string, misc::MetaValue * > *mMeta; 
          TSK_FS_INFO *mFs;  //The sleuthkit filesystem structure.
          carvpath_entity *mCp;  //The parent carvpath of the filesystem.
          TSK_FS_FILE *mTskFile;  //The sleuthkit structure for this inode.
          size_t mValidDataStreamCount;  //The number of valid alternate data streams in this filesystem entity.
          size_t mFirstValidStreamIndex; //The index of the first valid alternate data stream.
          size_t mLastValidStreamIndex;  //The index of the last valid alternate data stream.
          size_t mStreamIndex;           //The index of the current (alternate stream) data attribute.
          size_t mMainStreamIndex;       //The index of the main data attribute of this file.
          ocfa::treegraph::TreeGraphNode *mMainStream; //The main data attribute of this file.
          std::string mName; // The name for this inode.
          bool mDoSlack;     // Indicates that file slack should be produced as a sub node.
          bool mSlackProcessed; // Indicates that if there was any slack to be produced, it has been produced, and further sub nodes are alternate streams.
	  bool mDeleted; //The node is a deleted node.
     }; 
  }
}
#endif
