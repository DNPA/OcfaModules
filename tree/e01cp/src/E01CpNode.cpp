#include "../inc/E01CpNode.hpp"
#include "misc.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <boost/lexical_cast.hpp>
#include <CarvPathRepositoryBuilder.hpp>
#include <time.h>
namespace ocfa {
  namespace e01cp {
          bool E01CpNode::fileExists(std::string path) {
            struct stat astat;
            if (lstat(path.c_str(),&astat)== -1) {
               return false; 
            }
            return true; 
          }

          void E01CpNode::fillFileNames(std::string path) {
             int namelen=path.size()+1;
             int index=0;
             mFilesCount=0;
             for (index=0;index<100+22*26*26;index++) {
               mFilenames[index] = (char *) malloc(namelen);
               strncpy(mFilenames[index],path.c_str(),path.size()-3);
               if (index < 99) {
                 if (index < 9) {
                    sprintf(mFilenames[index]+path.size()-3,"E0%d",index+1);
                 } else {
                    sprintf(mFilenames[index]+path.size()-3,"E%d",index+1);
                 }
               }  else {
                   char c1=((index - 99) / (26*26)) + 'E';
                   char c2=(((index - 99) %(26*26)) / 26) + 'A';
                   char c3=((index -99) % 26 ) + 'A';
                   sprintf(mFilenames[index]+path.size()-3,"%c%c%c",c1,c2,c3);
               }
               if (fileExists(mFilenames[index])) {
                 mFilesCount++;
               } else {
                 if (index == 0) {
                    throw ocfa::misc::OcfaException("No E01 file derivable from supplied path",this);
                 }
                 return;
               }
             }
     
          }
          E01CpNode::E01CpNode(std::string path):OcfaObject("E01CpNode","e01cp"),mReadOffset(0){
             getLogStream(ocfa::misc::LOG_NOTICE) << "Start processing e01 data." << std::endl;
             mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();
             fillFileNames(path);
             mEwfHandle= libewf_open(mFilenames,mFilesCount,LIBEWF_FLAG_READ);
             if (mEwfHandle == 0) {
                  throw ocfa::misc::OcfaException("Problem opening encase image.",this);
             }
             if (libewf_get_media_size(mEwfHandle,&mSize) == -1) {
                throw ocfa::misc::OcfaException("Problem getting media size from ewf image",this);
             }
             ocfa::misc::metautil::addMetaToMap(mMeta,"size", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long)mSize)));
             ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("disk-image")));
//             u_int8_t guid[16];
//             if( libewf_get_guid(mEwfHandle,guid,16) == -1) {
//                 throw ocfa::misc::OcfaException("Problem getting GUID from libewf",this);
//             }
//             char asciiGuid[40];
//             sprintf(asciiGuid,"%2.2x%2.2x%2.2x%2.2x-%2.2x%2.2x-%2.2x%2.2x-%2.2x%2.2x-%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
//                                 guid[ 0 ], guid[ 1 ], guid[ 2 ], guid[ 3 ], guid[ 4 ], guid[ 5 ], guid[ 6 ], guid[ 7 ],
//                                 guid[ 8 ], guid[ 9 ], guid[ 10 ], guid[ 11 ], guid[ 12 ], guid[ 13 ], guid[ 14 ], guid[ 15 ]
//                                );
//             (*mMeta)["guid"]= new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(asciiGuid));
             if (libewf_parse_header_values(mEwfHandle,LIBEWF_DATE_FORMAT_ISO8601) !=1) {
                 throw ocfa::misc::OcfaException("Problemm in libewf parsing header values.",this);
             }
             uint32_t headercount;
             if (libewf_get_amount_of_header_values(mEwfHandle,&headercount) == -1) {
                throw ocfa::misc::OcfaException("Problem getting header count from libewf.",this);
             }
             for (uint32_t headerno=0;headerno<headercount;headerno++) {
                 char key[64];
                 char val[128];
                 if(( libewf_get_header_value_identifier(mEwfHandle,headerno,key,64) == 1) &&
                    ( libewf_get_header_value(mEwfHandle,key,val,128) == 1) ){
                        if ((strlen(key) > 7) && (strcmp(key+strlen(key)-5,"_date")==0)) {
                          const ocfa::misc::DateTime *datetime=new ocfa::misc::DateTime(ocfa::misc::DateTime::translate(val),"INVALID");
                          ocfa::misc::metautil::addMetaToMap(mMeta,key, new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(&datetime)));
                        } else {
                          ocfa::misc::metautil::addMetaToMap(mMeta,key, new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(val)));
                        }
                 }
             }
             ocfa::misc::ArrayMetaValue *mv=new ocfa::misc::ArrayMetaValue();
             for (int fileindex=0;fileindex < mFilesCount;fileindex++ ) {
                ocfa::misc::Scalar filePath(mFilenames[fileindex]);
                mv->addMetaValue(filePath);
             }
             (*mMeta)["imgdatafiles"]=mv;
             std::string vardir=ocfa::misc::OcfaConfig::Instance()->getValue("varroot");
             try {
                CarvPathRepository *repository=CarvPathRepositoryBuilder::buildRepository(vardir + "/carvfs");
                char buffer[512];
                CarvPathImage *image=repository->createImage(mSize);
                getLogStream(ocfa::misc::LOG_NOTICE) << "Streaming content to archive" << std::endl;
                mStartTime=time(NULL);
                while(streamRead(buffer,512) == 512) {
                   image->writeblock((unsigned char *) buffer);
                }
                image->setDone();
                getLogStream(ocfa::misc::LOG_NOTICE) << "Streaming content to archive done." << std::endl;
                mCarvPath=image->getCarvPath();
                mDoneTime=time(NULL);
             } 
             catch (std::string &err) {
               throw ocfa::misc::OcfaException(err,this);
             }  
          }

          E01CpNode::~E01CpNode(){
             libewf_close(mEwfHandle);
             if (mStartTime && mDoneTime) {
                time_t spent=mDoneTime - mStartTime;
                time_t wasted=time(NULL) - mDoneTime;
                if (wasted > 1) {
                   getLogStream(ocfa::misc::LOG_NOTICE) << "Streaming took " << spent << " seconds, wasted an additional " << wasted << " seconds as a result of an incomplete API" << std::endl;
                }
             }
          }

	  bool E01CpNode::hasContent() {
            return true; 
          }

	  void E01CpNode::openStream()  {
             mReadOffset=0;
          }
          void E01CpNode::closeStream() {
          
          }

          size_t E01CpNode::streamRead(char *buf, size_t count) {
              size64_t left=mSize - mReadOffset;
              size64_t realcount=count;
              if (left < realcount) {
                  getLogStream(ocfa::misc::LOG_NOTICE) << realcount << " bytes requested while only " << left << " bytes left while at offset " << mReadOffset << "(size=" << mSize << "), returning left bytes." <<  std::endl;
                  realcount=left;
              }
              if (realcount == 0) { return 0;}
              size_t readcount=libewf_read_random(mEwfHandle,buf,realcount,mReadOffset);
              if (realcount != readcount) {
                 getLogStream(ocfa::misc::LOG_ERR) << "Read at offset " << mReadOffset << " returned only " << readcount << " bytes while " << realcount << "were expected. File size=" << mSize << " , left=" << left << std::endl; 
                 throw ocfa::misc::OcfaException(std::string("libewf_read_random returned less bytes ") + boost::lexical_cast< std::string > (readcount) + " than expected " + boost::lexical_cast< std::string > (realcount) ); 
              }
              mReadOffset += readcount;
              return readcount;
          }

          std::string E01CpNode::getSoftLinkablePath(ocfa::misc::DigestPair **) {
             return mCarvPath;
          }

          std::string E01CpNode::getHardLinkablePath(std::string targetbasepath,ocfa::misc::DigestPair **)  {
             return "";
          }

          bool E01CpNode::hasSubEntities() { return false;}

          void E01CpNode::resetSubEntityIterator() {
                 throw ocfa::misc::OcfaException("No iterator operations for E01CpNode",this);
          }

          bool E01CpNode::nextSubEntity() {
              throw ocfa::misc::OcfaException("No iterator operations for E01CpNode",this);
          }

          void E01CpNode::getCurrentSubEntity(TreeGraphNode ** subent) {
              throw ocfa::misc::OcfaException("No iterator operations for E01CpNode",this);
          }

          std::string E01CpNode::getCurrentSubEntityRelation() {
              throw ocfa::misc::OcfaException("No iterator operations for E01CpNode",this);
          }

          std::string E01CpNode::getName() {
              return "image";
          }

          void E01CpNode::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
             *map=mMeta;
             mMeta=0;
          }

          void E01CpNode::unlinkOnDestruct(){
          }           
  }
}
