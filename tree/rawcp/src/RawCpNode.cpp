#include "../inc/RawCpNode.hpp"
#include "misc.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <boost/lexical_cast.hpp>
#include <CarvPathRepositoryBuilder.hpp>
#include <fcntl.h>
#include <time.h>

namespace ocfa {
  namespace rawcp {
          RawCpNode::RawCpNode(std::string path):OcfaObject("RawCpNode","rawcp"),mReadOffset(0),mStartTime(0),mDoneTime(0){
             getLogStream(ocfa::misc::LOG_DEBUG) << "Start processing raw data." << std::endl;
             mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();
             mRawHandle= open(path.c_str(),O_LARGEFILE | O_RDONLY);
             if (mRawHandle == -1) {
                  throw ocfa::misc::OcfaException("Problem opening raw image.",this);
             }
             mSize=lseek(mRawHandle,0,SEEK_END);
             lseek(mRawHandle,0,SEEK_SET);
             if (mSize == -1) {
                throw ocfa::misc::OcfaException("Problem getting media size from raw image",this);
             }
             ocfa::misc::metautil::addMetaToMap(mMeta,"size", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long) mSize)));
             ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("disk-image")));
             std::string vardir=ocfa::misc::OcfaConfig::Instance()->getValue("varroot");
             try {
                CarvPathRepository *repository=CarvPathRepositoryBuilder::buildRepository(vardir + "/carvfs");
                char buffer[512];
                CarvPathImage *image=repository->createImage(mSize);
                getLogStream(ocfa::misc::LOG_DEBUG) << "Streaming content to archive" << std::endl;
                mStartTime=time(NULL);
                while(streamRead(buffer,512) == 512) {
                   image->writeblock((unsigned char *) buffer);
                }
                image->setDone();
                getLogStream(ocfa::misc::LOG_DEBUG) << "Streaming content to archive done." << std::endl;
                mCarvPath=image->getCarvPath();
                mDoneTime=time(NULL);
             } 
             catch (std::string &err) {
               throw ocfa::misc::OcfaException(err,this);
             }
          }

          RawCpNode::~RawCpNode(){
             close(mRawHandle);
             if (mStartTime && mDoneTime) {
                time_t spent=mDoneTime - mStartTime;
                time_t wasted=time(NULL) - mDoneTime;
                if (wasted > 1) {
                   getLogStream(ocfa::misc::LOG_NOTICE) << "Streaming took " << spent << " seconds, wasted an additional " << wasted << " seconds as a result of an incomplete API" << std::endl;
                }
             }
          }

	  bool RawCpNode::hasContent() {
            return true; 
          }

	  void RawCpNode::openStream()  {
             mReadOffset=0;
             lseek(mRawHandle,0,SEEK_SET);
          }
          void RawCpNode::closeStream() {
          
          }

          size_t RawCpNode::streamRead(char *buf, size_t count) {
              return read(mRawHandle,buf,count);
          }

          std::string RawCpNode::getSoftLinkablePath(ocfa::misc::DigestPair **) {
             return mCarvPath;
          }

          std::string RawCpNode::getHardLinkablePath(std::string targetbasepath,ocfa::misc::DigestPair **)  {
             return "";
          }

          bool RawCpNode::hasSubEntities() { return false;}

          void RawCpNode::resetSubEntityIterator() {
                 throw ocfa::misc::OcfaException("No iterator operations for RawCpNode",this);
          }

          bool RawCpNode::nextSubEntity() {
              throw ocfa::misc::OcfaException("No iterator operations for RawCpNode",this);
          }

          void RawCpNode::getCurrentSubEntity(TreeGraphNode ** subent) {
              throw ocfa::misc::OcfaException("No iterator operations for RawCpNode",this);
          }

          std::string RawCpNode::getCurrentSubEntityRelation() {
              throw ocfa::misc::OcfaException("No iterator operations for RawCpNode",this);
          }

          std::string RawCpNode::getName() {
              return "image";
          }

          void RawCpNode::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
             *map=mMeta;
             mMeta=0;
          }

          void RawCpNode::unlinkOnDestruct(){
          }           
  }
}
