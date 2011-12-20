#ifndef SLNODE_HPP
#define SLNODE_HPP
#include "treegraph.hpp"
#include <string>
namespace ocfa {
  namespace sl {
     class SoftLinkNode: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          SoftLinkNode(std::string path);
          ~SoftLinkNode();
	  bool hasContent() ;
          off_t getSize() {return mSize;} 
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
         std::map < std::string, misc::MetaValue * > *mMeta;
         std::string mCarvPath;
         off_t mSize;
     }; 
  }
}
#endif
