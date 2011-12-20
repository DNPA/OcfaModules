#ifndef MMLSTOPNODE_HPP
#define MMLSTOPNODE_HPP
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
     class MmlsTopNode: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          MmlsTopNode(std::string path,carvpath_library *cplib);
          ~MmlsTopNode();
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
          void unlinkOnDestruct(){/* do nothing */ }           
        private:
          void fillFileNames(std::string path);
          bool fileExists(std::string path);
          bool isOnSameDevice(std::string p1,std::string p2);
          std::map < std::string, misc::MetaValue * > *mMeta;
          TSK_IMG_INFO *mImageHandle;
          TSK_VS_INFO *mVsHandle;
          TSK_OFF_T mReadOffset;
          TSK_PNUM_T mActivePartitionIndex;
          char * mFilenames[26*26*22+100];
          u_int16_t mFilesCount;
          carvpath_entity *mCarvpathTop;
     }; 
  }
}
#endif
