#ifndef TSKFSDUPDIR_HPP
#define TSKFSDUPDIR_HPP
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
     //This node is a node that indicates the detection of a loop in the filesystem.
     class TskFsDuplicateDirectory: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          TskFsDuplicateDirectory(TSK_INUM_T inum,std::string name,bool verbosemeta);
          ~TskFsDuplicateDirectory();
          std::string getName() {return mName;}
          void takeMetaMap(std::map < std::string, misc::MetaValue * >**map) ;
          void unlinkOnDestruct(){}           
        private:
          bool mVerboseMeta;
          std::map < std::string, misc::MetaValue * > *mMeta;
          std::string mName;
     }; 
  }
}
#endif
