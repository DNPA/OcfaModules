#include "../inc/DemoSubNode.hpp"
namespace ocfa {
  namespace demo {
          DemoSubNode::DemoSubNode(std::string path,bool first):ocfa::OcfaObject("DemoSubNode","demo"),mPath(path),mFirst(first),mOffset(0) {
             mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();
             (*mMeta)["foo"]= new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("hohoho"));
          }


          DemoSubNode::~DemoSubNode(){}

	  bool DemoSubNode::hasContent(){ 
             getLogStream(ocfa::misc::LOG_ERR) << "Subnode has content" << std::endl;
             return true;
          } 

          off_t DemoSubNode::getSize() {
             return 4096;
          }

	  void DemoSubNode::openStream() {
              getLogStream(ocfa::misc::LOG_ERR) << "Opening stream" << std::endl;
              mOffset=0;
          } 

          void DemoSubNode::closeStream() {
              getLogStream(ocfa::misc::LOG_ERR) << "Closing stream" << std::endl;
          } 

          size_t DemoSubNode::streamRead(char *buf, size_t count){
              getLogStream(ocfa::misc::LOG_ERR) << "Request to read " << count << " bytes" <<std::endl;
              size_t dataleft=4096 - mOffset;
              size_t rval=count;
              if (dataleft < count) {
                  rval=dataleft; 
              }
              char c='2';
              if (mFirst)
                 c='1';
              memset((void *)buf, c, rval);
              mOffset+=rval;
              getLogStream(ocfa::misc::LOG_ERR) << "Succeded to read " << rval << " bytes" <<std::endl;
              return rval;            
          } 

          std::string DemoSubNode::getSoftLinkablePath(ocfa::misc::DigestPair **) { 
              getLogStream(ocfa::misc::LOG_ERR) << "No soft linkable path for node" << std::endl;
              return "";
          } 

          std::string DemoSubNode::getHardLinkablePath(std::string targetbasepath,ocfa::misc::DigestPair **) { 
              getLogStream(ocfa::misc::LOG_ERR) << "No hard linkable path for node" << std::endl;
              return "";
          } 

          bool DemoSubNode::hasSubEntities() { return false;} 

          void DemoSubNode::resetSubEntityIterator() {} 

          bool DemoSubNode::nextSubEntity() { return false;} 

          void DemoSubNode::getCurrentSubEntity(TreeGraphNode ** subent) {
             throw ocfa::misc::OcfaException("DemoSubNode has no current SubEntity",this);
          } 

          std::string DemoSubNode::getCurrentSubEntityRelation() {
             throw ocfa::misc::OcfaException("DemoSubNode has no current SubEntity",this);
          } 

          std::string DemoSubNode::getName() {
             getLogStream(ocfa::misc::LOG_ERR) << "Returning subnode name" << std::endl;
             if (mFirst) {
               return "FirstNode";
             }
             return "SecondNode";
          } 

          void DemoSubNode::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
            getLogStream(ocfa::misc::LOG_ERR) << "Transfering metamap to caller" << std::endl;
            *map=mMeta;
            mMeta=0;
          }           

          void DemoSubNode::unlinkOnDestruct(){}
  }
}
