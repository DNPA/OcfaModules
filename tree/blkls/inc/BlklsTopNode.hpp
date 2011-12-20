#ifndef BLKLSTOPNODE_HPP
#define BLKLSTOPNODE_HPP
#include "treegraph.hpp"
#include "misc.hpp"
#include <tsk3/libtsk.h>
#include <string>
extern "C" {
#include <libcarvpath.h>
}
#ifndef CARVPATH_ERR_MAXERRNO
#error Unable to build with old version of libcarvpath.Please upgrade libcarvpath to a current version.
#endif
namespace ocfa {
  namespace tsk {

    typedef struct _walkstate {
      carvpath_entity *topentity;
      carvpath_entity *currentity;
      TSK_WALK_RET_ENUM returnedval;
      TSK_DADDR_T lastblockix;
    } walkstate;
     class BlklsTopNode: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          BlklsTopNode(std::string path);
          virtual ~BlklsTopNode();
	  bool hasContent() ;
          off_t getSize() ; 
	  void openStream();
          void closeStream();
          size_t streamRead(char *buf, size_t count) ;
          std::string getSoftLinkablePath(ocfa::misc::DigestPair **) ;
          std::string getHardLinkablePath(std::string targetbasepath,ocfa::misc::DigestPair **) ;
          bool hasSubEntities() ;
          void resetSubEntityIterator() ;
          bool nextSubEntity() ;
          void getCurrentSubEntity(TreeGraphNode ** subent) ;
          std::string getCurrentSubEntityRelation() ;
          std::string getName() ;
          void takeMetaMap(std::map < std::string, misc::MetaValue * >**map) ;
          void unlinkOnDestruct(){/* do nothing */ };           
        private:
          std::map < std::string, misc::MetaValue * > *mMeta;
	  TSK_FS_INFO *mFsInfo;
	  TSK_IMG_INFO *mImgInfo;
          std::string mRealPath;
          carvpath_library *mCpLib;
       // subentity state variables
       int mLastBlockIx;
       carvpath_entity *mCarvpathTop, *mCurrSubEntity ;
       unsigned int mFragIx;  
       TSK_WALK_RET_ENUM mLastWalkState;
       bool mPseudoCarvPath, mValidFormat;
     }; 
  }
}
#endif
