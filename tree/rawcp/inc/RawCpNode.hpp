#ifndef RAWCPNODE_HPP
#define RAWCPNODE_HPP
#include "treegraph.hpp"
#include <time.h>
#include <string>
namespace ocfa {
  namespace rawcp {
     class RawCpNode: public ocfa::treegraph::TreeGraphNode,public ocfa::OcfaObject {
        public:
          RawCpNode(std::string path);
          ~RawCpNode();
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
//       void fillFileNames(std::string path);
//       bool fileExists(std::string path);
         std::map < std::string, misc::MetaValue * > *mMeta;
         int mRawHandle;
         off_t mReadOffset;
         std::string mCarvPath;
         off_t mSize;
         time_t mStartTime;
         time_t mDoneTime;
//         char * mFilenames[26*26*22+100];
//         u_int16_t mFilesCount;
     }; 
  }
}
#endif
