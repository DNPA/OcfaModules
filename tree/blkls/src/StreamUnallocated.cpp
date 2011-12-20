#include "StreamUnallocated.hpp"
#include"misc.hpp"
#include "OcfaObject.hpp"
#include <boost/lexical_cast.hpp>
#include<fstream>
#include<sstream>
namespace ocfa {
  namespace tsk {

    // if called with softlinkable = true, a carvpath is asumed which will be returned by getSoftLinkablePath. 
    // In this case you don't need to specify the offset. 
    // if called with softlinkable = false you will need to specify the offset and size which will be used by streamToOutput.
    StreamUnallocated::StreamUnallocated(std::string filename, off_t offset, off_t size, bool softlinkable): ocfa::OcfaObject("StreamUnallocated","blkls"), mName(filename), mOffset(offset), mSize(size), mSoftlinkable(softlinkable), mHasContent(true)
    {
      ocfaLog(ocfa::misc::LOG_DEBUG, "StreamUnallocated constructed");
      initMetaMap();
    }

    
   std::string StreamUnallocated::getName(){
      std::ostringstream name;
      name << "Unallocated_" << mOffset << "_" << mSize;
      return name.str();
   }

   void StreamUnallocated::initMetaMap(){
      mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();
      ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("unallocateddiskspace")));
      ocfa::misc::metautil::addMetaToMap(mMeta,"size", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(mSize)));
      if (!mSoftlinkable){
        ocfa::misc::metautil::addMetaToMap(mMeta,"offset", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(mOffset)));
      }
    }

    StreamUnallocated::~StreamUnallocated() {
       if (mMeta) {
            map < string, misc::MetaValue * >::const_iterator p;
            for (p = mMeta->begin(); p != mMeta->end(); ++p) {
               delete p->second;
            }
            delete mMeta;
            mMeta=0;
       }
    }


   // return carvpath

   std::string StreamUnallocated::getSoftLinkablePath(ocfa::misc::DigestPair **){ //RJM: return empty string if input was not a carvfs path, otherwise streamToOutput will never get called.
     if (mSoftlinkable){
       std::string realpath = ocfa::misc::fsutil::dereferencePath(mName + std::string(".crv"));
       ocfaLog(ocfa::misc::LOG_DEBUG, "SLP: " + realpath );
       return realpath;
     } else {
       return "";
     }


   }

    void StreamUnallocated::streamToOutput(ocfa::misc::AbstractWriteFacet &writefacet){
       if (!mSoftlinkable){
          std::ifstream file;
          file.open(mName.c_str(), ifstream::binary | ifstream::in);
          file.seekg(mOffset, ios_base::beg);
          char *buf = new char[mSize];
          file.read(buf, mSize);
          writefacet(buf, mSize);
          delete[] buf;
       }
    }



    void StreamUnallocated::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
      //RJM: check precondition *map==0
      ocfaLog(ocfa::misc::LOG_DEBUG, "Metamap returned" );
      if (*map == 0){
        *map=mMeta;
        mMeta=0;
      } else {
        throw ocfa::misc::OcfaException("StreamUnallocated::takeMetaMap called with **map not cleared.");
      }
     } 
  }
}
