#ifndef E01NODE_HPP
#define E01NODE_HPP
#include "treegraph.hpp"
#include <libewf.h>
#include <string>
#include <time.h>
namespace ocfa {
  namespace e01cp {
     class E01CpNode: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          E01CpNode(std::string path);
          ~E01CpNode();
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
         void fillFileNames(std::string path);
         bool fileExists(std::string path);
         std::map < std::string, misc::MetaValue * > *mMeta;
         LIBEWF_HANDLE *mEwfHandle;
         size64_t mReadOffset;
         std::string mCarvPath;
         size64_t mSize;
         char * mFilenames[26*26*22+100];
         u_int16_t mFilesCount;
         time_t mStartTime;
         time_t mDoneTime;
     }; 
  }
}
#endif
