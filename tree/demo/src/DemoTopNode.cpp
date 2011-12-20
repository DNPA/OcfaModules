#include "../inc/DemoTopNode.hpp"
#include "../inc/DemoSubNode.hpp"
namespace ocfa {
  namespace demo {
          DemoTopNode::DemoTopNode(std::string path):OcfaObject("DemoTopNode","demo"),mPath(path),mIterator(false),mDeleted(false){
             mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();
             (*mMeta)["chunks"]= new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(2));
          }

          DemoTopNode::~DemoTopNode(){}

	  bool DemoTopNode::hasContent() {
            return false; 
          }

          off_t DemoTopNode::getSize()  {
             throw ocfa::misc::OcfaException("Can get size on DemoTopNode",this);

          }
	  void DemoTopNode::openStream()  {
             throw ocfa::misc::OcfaException("Can not open stream on DemoTopNode",this);

          }
          void DemoTopNode::closeStream() {
             throw ocfa::misc::OcfaException("Can not close stream on DemoTopNode",this);
          }

          size_t DemoTopNode::streamRead(char *buf, size_t count) {
             throw ocfa::misc::OcfaException("Can not streamread on DemoTopNode",this);
          }

          std::string DemoTopNode::getSoftLinkablePath(ocfa::misc::DigestPair **) {
             throw ocfa::misc::OcfaException("Can not get softlinkable path on DemoTopNode",this);
          }

          std::string DemoTopNode::getHardLinkablePath(std::string targetbasepath,ocfa::misc::DigestPair **)  {
             throw ocfa::misc::OcfaException("Can not get hardlinkable path on DemoTopNode",this);
          }

          bool DemoTopNode::hasSubEntities() { return true;}

          void DemoTopNode::resetSubEntityIterator() {
                 getLogStream(ocfa::misc::LOG_DEBUG) << "Resetting SubEntityIterator" << std::endl;
                 mIterator=false;
          }

          bool DemoTopNode::nextSubEntity() {
              if (mIterator == false) {
                 getLogStream(ocfa::misc::LOG_ERR) << "Iterating to the second entity" << std::endl;
                 mIterator=true;
                 return true;
              }
              getLogStream(ocfa::misc::LOG_ERR) << "Nothing left to iterate." << std::endl;
              return false;
          }

          void DemoTopNode::getCurrentSubEntity(TreeGraphNode ** subent) {
            getLogStream(ocfa::misc::LOG_ERR) << "Creating new DemoSubNode. path=" << mPath << " , type=" << mIterator << std::endl;
            *subent = new DemoSubNode(mPath,mIterator);
          }

          std::string DemoTopNode::getCurrentSubEntityRelation() {
             getLogStream(ocfa::misc::LOG_ERR) << "Returning entity relation name" << std::endl;
             if (mIterator) {
               return "secondtype";
             } 
             return "firsttype"; 
          }

          std::string DemoTopNode::getName() {
             getLogStream(ocfa::misc::LOG_ERR) << "Returning top node name 'demotop'" << std::endl;
             return "demotop";
          }

          void DemoTopNode::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
             getLogStream(ocfa::misc::LOG_ERR) << "Transfering metamap to caller" << std::endl; 
             *map=mMeta;
             mMeta=0;
          }

          void DemoTopNode::unlinkOnDestruct(){
            getLogStream(ocfa::misc::LOG_ERR) << "Setting unlink on destruct" << std::endl;
          }           
  }
}
