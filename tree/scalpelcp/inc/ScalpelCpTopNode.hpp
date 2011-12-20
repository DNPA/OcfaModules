#ifndef SCALPELTOPNODE_HPP
#define SCALPELTOPNODE_HPP
#include "treegraph.hpp"
#include "misc.hpp"
#include <string>
#include <vector>
extern "C" {
#include <libcarvpath.h>
}
#ifndef CARVPATH_ERR_MAXERRNO
#error Unable to build with old version of libcarvpath.Please upgrade libcarvpath to a current version.
#endif
namespace ocfa {
  namespace scalpel {
     class ScalpelCpTopNode: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          ScalpelCpTopNode(std::string path,carvpath_library *cplib,std::string workdir);
          ~ScalpelCpTopNode();
          bool hasSubEntities() ;
          void resetSubEntityIterator() ;
          bool nextSubEntity() ;
          void getCurrentSubEntity(TreeGraphNode ** subent) ;
          std::string getCurrentSubEntityRelation() { return "filedirentry";} 
          std::string getName() { return "topnode";}
          void takeMetaMap(std::map < std::string, misc::MetaValue * >**map) ;
          void unlinkOnDestruct(){/* do nothing */ }           
        private:
          carvpath_entity *mCarvpathTop;
          std::string mWorkDir;
          std::vector< std::string > mCarvLines;
          size_t mActiveLine;
     }; 
  }
}
#endif
