#include "../inc/E01Node.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <boost/lexical_cast.hpp>
namespace ocfa {
  namespace e01 {
          bool E01Node::fileExists(std::string path) {
            struct stat astat;
            if (lstat(path.c_str(),&astat)== -1) {
               return false; 
            }
            return true; 
          }

          void E01Node::fillFileNames(std::string path) {
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
          E01Node::E01Node(std::string path):OcfaObject("E01Node","e01"){
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
             ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype",new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("disk-image")));
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
              
          }

          E01Node::~E01Node(){
             libewf_close(mEwfHandle);  
          }

	  bool E01Node::hasContent() {
            return true; 
          }

	  void E01Node::openStream()  {
             mReadOffset=0;
          }
          void E01Node::closeStream() {
          
          }

          size_t E01Node::streamRead(char *buf, size_t count) {
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
                 throw ocfa::misc::OcfaException("libewf_read_random returned less bytes than expected."); 
              }
              mReadOffset += readcount;
              return readcount;
          }

          std::string E01Node::getSoftLinkablePath(ocfa::misc::DigestPair **) {
             return "";
          }

          std::string E01Node::getHardLinkablePath(std::string targetbasepath,ocfa::misc::DigestPair **)  {
             return "";
          }

          bool E01Node::hasSubEntities() { return false;}

          void E01Node::resetSubEntityIterator() {
                 throw ocfa::misc::OcfaException("No iterator operations for E01Node",this);
          }

          bool E01Node::nextSubEntity() {
              throw ocfa::misc::OcfaException("No iterator operations for E01Node",this);
          }

          void E01Node::getCurrentSubEntity(TreeGraphNode ** subent) {
              throw ocfa::misc::OcfaException("No iterator operations for E01Node",this);
          }

          std::string E01Node::getCurrentSubEntityRelation() {
              throw ocfa::misc::OcfaException("No iterator operations for E01Node",this);
          }

          std::string E01Node::getName() {
              return "image";
          }

          void E01Node::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
             *map=mMeta;
             mMeta=0;
          }

          void E01Node::unlinkOnDestruct(){
          }           
  }
}
