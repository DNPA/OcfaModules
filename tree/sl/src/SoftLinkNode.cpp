#include "../inc/SoftLinkNode.hpp"
#include "misc.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <boost/lexical_cast.hpp>
#include <CarvPathRepositoryBuilder.hpp>
#include <fcntl.h>

namespace ocfa {
  namespace sl {
          SoftLinkNode::SoftLinkNode(std::string path):OcfaObject("SoftLinkNode","sl"){
             mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();
	     int rawHandle= open(path.c_str(),O_LARGEFILE | O_RDONLY);
             if (rawHandle == -1) {
                  throw ocfa::misc::OcfaException("Problem opening encase image.",this);
             }
             mSize=lseek(rawHandle,0,SEEK_END);
	     close(rawHandle);
             (*mMeta)["size"]= new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long)mSize));
             (*mMeta)["nodetype"]= new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("disk-image"));
             mCarvPath=path;
          }

          SoftLinkNode::~SoftLinkNode(){
          }

	  bool SoftLinkNode::hasContent() {
	    return (mSize > 0);
          }

	  void SoftLinkNode::openStream()  {
          }
          void SoftLinkNode::closeStream() {
          }

          size_t SoftLinkNode::streamRead(char *buf, size_t count) {
          }

          std::string SoftLinkNode::getSoftLinkablePath(ocfa::misc::DigestPair **) {
             return mCarvPath;
          }

          std::string SoftLinkNode::getHardLinkablePath(std::string targetbasepath,ocfa::misc::DigestPair **)  {
             return "";
          }

          bool SoftLinkNode::hasSubEntities() { return false;}

          void SoftLinkNode::resetSubEntityIterator() {
                 throw ocfa::misc::OcfaException("No iterator operations for RawCpNode",this);
          }

          bool SoftLinkNode::nextSubEntity() {
              throw ocfa::misc::OcfaException("No iterator operations for RawCpNode",this);
          }

          void SoftLinkNode::getCurrentSubEntity(TreeGraphNode ** subent) {
              throw ocfa::misc::OcfaException("No iterator operations for RawCpNode",this);
          }

          std::string SoftLinkNode::getCurrentSubEntityRelation() {
              throw ocfa::misc::OcfaException("No iterator operations for RawCpNode",this);
          }

          std::string SoftLinkNode::getName() {
              return "image";
          }

          void SoftLinkNode::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
             *map=mMeta;
             mMeta=0;
          }

          void SoftLinkNode::unlinkOnDestruct(){
          }           
  }
}
