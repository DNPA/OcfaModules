#ifndef TSKFSTOPDIR_HPP
#define TSKFSTOPDIR_HPP
#include "treegraph.hpp"
#include "misc.hpp"
#include <tsk3/libtsk.h>
#include <string>
#include "LoopDetectorAnchor.hpp"
extern "C" {
#include <libcarvpath.h>
}
#ifndef CARVPATH_ERR_MAXERRNO
#error Unable to build with old version of libcarvpath.Please upgrade libcarvpath to a current version.
#endif
namespace ocfa {
  namespace tsk {
     //This class represents the top directory of the filesystem.
     class TskFsTopDir: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          TskFsTopDir(std::string path,std::string timesource,bool dorawslack=false,bool docpslack=true,bool verbosemeta=true , carvpath_library *cplib=0);
          ~TskFsTopDir();
	  bool hasContent();
          off_t getSize(); 
          void streamToOutput(ocfa::misc::AbstractWriteFacet &writefacet);
          bool hasSubEntities() ;
          void resetSubEntityIterator() ;
          bool nextSubEntity() ;
          void getCurrentSubEntity(TreeGraphNode ** subent) ;
          std::string getCurrentSubEntityRelation() ;
          std::string getName() ;
          void takeMetaMap(std::map < std::string, misc::MetaValue * >**map) ;
          void unlinkOnDestruct(){}           
        private:
          bool mVerboseMeta; //Indicates if we should produce verbose meta data.
          LoopDetectorAnchor mAnchor; //The anchor for our loop detection subsystem.
          std::map < std::string, misc::MetaValue * > *mMeta;
          TSK_IMG_INFO *mImageHandle; //The sleuthkit lib structure for the image file information.
          TSK_FS_INFO *mFileSystem; //The sleuthkit lib structure for the file system information.
          carvpath_entity *mCarvpathTop; // The catvpath (if any) of the filesystem partition image. 
          TreeGraphNode *mTopDir; //The generic directory object for this top directory.
     }; 
  }
}
#endif
