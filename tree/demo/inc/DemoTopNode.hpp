#ifndef DEMOTOPNODE_HPP
#define DEMOTOPNODE_HPP
#include "treegraph.hpp"
namespace ocfa {
  namespace demo {
     class DemoTopNode: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          DemoTopNode(std::string path);
          ~DemoTopNode();
	  bool hasContent() ;
          off_t getSize() ;
	  void openStream() ;
          void closeStream() ;
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
          void unlinkOnDestruct();           
        private:
         std::string mPath;
         bool mIterator;
         std::map < std::string, misc::MetaValue * > *mMeta;
         bool mDeleted;
     }; 
  }
}
#endif
