#ifndef MMLSTOPNODE_HPP
#define MMLSTOPNODE_HPP
#include "treegraph.hpp"
#include "misc.hpp"
#include <string>
#include <Poco/URI.h>
namespace ocfa {
  namespace webtree {
     class WebTreeNode: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          WebTreeNode(std::string url,std::string timesource,WebTreeNode *parent);
          ~WebTreeNode();
	  bool hasContent() ;
          off_t getSize() ; 
	  void openStream();
          void closeStream();
          size_t streamRead(char *buf, size_t count) ;
          std::string getSoftLinkablePath(ocfa::misc::DigestPair **) ;
          std::string getHardLinkablePath(std::string targetbasepath,ocfa::misc::DigestPair **) ;
          bool hasSubEntities();
          void resetSubEntityIterator();
          bool nextSubEntity();
          void getCurrentSubEntity(TreeGraphNode ** subent) ;
          std::string getCurrentSubEntityRelation() ;
          std::string getName() ;
          void takeMetaMap(std::map < std::string, misc::MetaValue * >**map) ;
          void unlinkOnDestruct(){/* do nothing */ }       
          bool hasParentUrl(std::string url);    
        private:
          Poco::URI  mURI;
          std::string mTimeSource;
          std::map < std::string, misc::MetaValue * > *mMeta;
          std::vector<std::string> mChildren;
          size_t mCurrentChildIndex;
          size_t mSize;
     }; 
  }
}
#endif
