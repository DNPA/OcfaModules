#ifndef MMLSTABLE_HPP
#define MMLSTABLE_HPP
#include "treegraph.hpp"
#include "misc.hpp"
#include <tsk3/libtsk.h>
#include <string>
extern "C" {
#include <libcarvpath.h>
}
namespace ocfa {
  namespace tsk {
     class MmlsTable: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          MmlsTable(carvpath_entity *topcpent,TSK_VS_INFO *vshandle,int table_num,std::string description,off_t start,off_t length,TSK_PNUM_T pnum);
          ~MmlsTable();
	  bool hasContent() { return false; } //NOTE!! We currently don't export raw table data as such.
          off_t getSize()  { return 0; }
	  void openStream() { throw ocfa::misc::OcfaException("MmlsTable does not implement a stream interface.",this);} 
          void closeStream()  { throw ocfa::misc::OcfaException("MmlsTable does not implement a stream interface.",this);}
          size_t streamRead(char *buf, size_t count)  { throw ocfa::misc::OcfaException("MmlsTable does not implement a stream interface.",this);}
          std::string getSoftLinkablePath(ocfa::misc::DigestPair **) { return "";}
          std::string getHardLinkablePath(std::string targetbasepath,ocfa::misc::DigestPair **) { return ""; }
          bool hasSubEntities() ;
          void resetSubEntityIterator() ;
          bool nextSubEntity() ;
          void getCurrentSubEntity(TreeGraphNode ** subent) ;
          std::string getCurrentSubEntityRelation() { return "partitionentry"; }
          std::string getName() { return mName;}
          void takeMetaMap(std::map < std::string, misc::MetaValue * >**map) ;
          void unlinkOnDestruct() { /* do nothing */ }
        private:
          std::map < std::string, misc::MetaValue * > *mMeta;
          std::string mPath;
          std::string mName;
          TSK_IMG_INFO *mImageHandle;
          TSK_VS_INFO *mVsHandle;
          TSK_PNUM_T mActivePartitionIndex;
          TSK_PNUM_T mFirstPartitionIndex;
          int mTableNum;
          carvpath_entity *mCarvpathTop;
     }; 
  }
}
#endif
