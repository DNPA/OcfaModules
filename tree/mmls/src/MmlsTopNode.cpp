#include "MmlsTopNode.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include "CarvPathPartition.hpp"
#include "StreamPartition.hpp"
#include "MmlsTable.hpp"
namespace ocfa {
  namespace tsk {

     //Private helper method for determining if file exists.
     bool MmlsTopNode::fileExists(std::string path) {
            struct stat astat;
            if (lstat(path.c_str(),&astat)== -1) {
               return false;
            }
            return true;
     }

     //Private initialization method for filling a character string array with all files that together
     //combine to make an encase image.
     //path is the full path and filename to an encase E01 file
     void MmlsTopNode::fillFileNames(std::string path) {
             int namelen=path.size()+1;
             int index=0;
             mFilesCount=0;
             for (index=0;index<100+22*26*26;index++) {
               mFilenames[index] = (char *) malloc(namelen);
               strncpy(mFilenames[index],path.c_str(),path.size()-3);
               if (index < 99) {
                 //The first set of filenames ends with '.E01' upto 'E99' using only a numeric count.
                 if (index < 9) {
                    sprintf(mFilenames[index]+path.size()-3,"E0%d",index+1);
                 } else {
                    sprintf(mFilenames[index]+path.size()-3,"E%d",index+1);
                 }
               }  else {
                 //The remaining set of filenames ends with '.EAA' upto '.ZZZ' using the alphabeth.
                   char c1=((index - 99) / (26*26)) + 'E';
                   char c2=(((index - 99) %(26*26)) / 26) + 'A';
                   char c3=((index -99) % 26 ) + 'A';
                   sprintf(mFilenames[index]+path.size()-3,"%c%c%c",c1,c2,c3);
               }
               if (fileExists(mFilenames[index])) {
                 mFilesCount++;
               } else {
                 if (index == 0) {
                    throw ocfa::misc::OcfaException(std::string("No E01 file derivable from supplied path:") + mFilenames[index],this);
                 }
                 return;
               }
             }

     }

     // path is the full path and filename to a raw image file or encase E01 file 
     MmlsTopNode::MmlsTopNode(std::string path,carvpath_library *cplib):OcfaObject("MmlsTopNode","mmls"),mMeta(0),mImageHandle(0),mVsHandle(0),mReadOffset(0),mActivePartitionIndex(0),mFilesCount(0),mCarvpathTop(0) {
          mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();
          //Make sure our path if a symbolic link gets derefernced first.
          std::string realpath=ocfa::misc::fsutil::dereferencePath(path);
          //Unless otherwise determined, the input is assumed to be a single image raw (dd) file.
          TSK_IMG_TYPE_ENUM imagetype=TSK_IMG_TYPE_RAW_SING;          
          if ((realpath.size() > 4) && (realpath.find(".E01",realpath.size()-4) != std::string::npos )) {
              //If the file name indicates an EWF file, check if libtsk is compiled with EWF support.
              if ((tsk_img_type_supported() & TSK_IMG_TYPE_EWF_EWF) == 0) {
                 throw ocfa::misc::OcfaException("The sleuthkit library was build without libewf support",this);
              }
              //Find all other parts of the EWF image and fill our mFilenames member with it.
              fillFileNames(realpath);
              //Remember to tell libtsk we are dealing with an EWF image here.
              imagetype=TSK_IMG_TYPE_EWF_EWF; 
          } else {
              //If our image is not an EWF image, assume it is a single file image and set that name in mFilenames.
              mFilenames[0] = (char *) malloc(realpath.size()+1);
              strcpy(mFilenames[0],realpath.c_str());
              mFilesCount=1;
              if ((realpath.size() > 4) && (realpath.find(".crv",realpath.size()-4) != std::string::npos )) {
                 //If our single file image is contained in carvFs, set our mCarvpathTop with a usable carvpath top entity
                 //that we later can derive other carvpaths from. 
                 std::string cpbasename=realpath.substr(0, realpath.size() - 4);
                 struct stat basestat;
                 if (lstat(realpath.c_str(),&basestat)== -1) {
                   throw ocfa::misc::OcfaException("Problem calling stat on "+realpath,this);
                 }
                 mCarvpathTop = carvpath_top_entity(basestat.st_size,(char *) cpbasename.c_str(),cplib);
                 if (mCarvpathTop == 0) {
                     getLogStream(ocfa::misc::LOG_ERR) << "Mmls has a problem creating top entity for '" << cpbasename << "'" << std::endl;
                 }
              } 
              if (mCarvpathTop == 0) {
                getLogStream(ocfa::misc::LOG_WARNING) << "Mmls running in RAW (copy-out) mode for '" << realpath << "'. This will take up much storage!" << std::endl;
              }
          }      
          //Now lets talk to libtsk and open our image file.
#ifdef TSK_VERSION_NUM
          mImageHandle=tsk_img_open(mFilesCount,(const TSK_TCHAR**) mFilenames,imagetype,0);
#else
          mImageHandle=tsk_img_open(mFilesCount,(const TSK_TCHAR**) mFilenames,imagetype);
#endif
          if (mImageHandle == 0) {
             throw ocfa::misc::OcfaException(std::string("Problem opening image with the sleuthkit library: ")+mFilenames[0],this);
          }
          //Try to open the partition table of the image.
          mVsHandle=tsk_vs_open(mImageHandle,0,TSK_VS_TYPE_DETECT);
          if (mVsHandle) {
             //If this succeeds, our top node is a pseudo directory and the top of our tree.
             if (mVsHandle->part_count > 0) {
                ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("pseudodir")));
             } else {
                ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("disk-image/empty-ptable")));
             }
             ocfa::misc::metautil::addMetaToMap(mMeta,"partitiontabletype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(tsk_vs_type_toname(mVsHandle->vstype))));
             ocfa::misc::metautil::addMetaToMap(mMeta,"tableoffset", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long) mVsHandle->offset)));
             ocfa::misc::metautil::addMetaToMap(mMeta,"blocksize", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long) mVsHandle->block_size)));
             if (mVsHandle->endian == TSK_BIG_ENDIAN) {
                 ocfa::misc::metautil::addMetaToMap(mMeta,"endian", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("big")));
             } else {
                ocfa::misc::metautil::addMetaToMap(mMeta,"endian", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("litle")));
             }
             if (mVsHandle->part_count > 0) {
                 ocfa::misc::metautil::addMetaToMap(mMeta,"partitiontable", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("used")));
             } else {
                 ocfa::misc::metautil::addMetaToMap(mMeta,"size", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long) mImageHandle->size)));
                 ocfa::misc::metautil::addMetaToMap(mMeta,"partitiontable", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("empty")));
                 (*mMeta)["size"]= new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long) mImageHandle->size));
             }
          } else {
             //If this fails, our top node is a single 'unallocated' node with data in it.
             ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("disk-image/no-ptable")));
             ocfa::misc::metautil::addMetaToMap(mMeta,"size", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long) mImageHandle->size)));
             ocfa::misc::metautil::addMetaToMap(mMeta,"partitiontable", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("non")));
          }    
     }
     MmlsTopNode::~MmlsTopNode() {
         for (u_int16_t index=0;index<mFilesCount;index++) {
            free(mFilenames[index]);
         } 
         if (mVsHandle) {
            tsk_vs_close(mVsHandle);
            mVsHandle=0;
         }
         if (mImageHandle) {
            tsk_img_close(mImageHandle);
            mImageHandle=0;
         }
         if (mCarvpathTop) {
            carvpath_free(mCarvpathTop,0);
            mCarvpathTop=0; 
         }
         if (mMeta) {
            map < string, misc::MetaValue * >::const_iterator p;
            for (p = mMeta->begin(); p != mMeta->end(); ++p) {
               delete p->second;
            }
            delete mMeta;
            mMeta=0;
         }
     }


     //Private helper method for detemining if the top node has DATA content
     bool MmlsTopNode::hasContent() { 
        if ((mVsHandle) && (mVsHandle->part_count > 0)) { 
              return false;
        } 
        return true;
     }

     //Private helper method for determining if tho paths reside on the same device.
     bool MmlsTopNode::isOnSameDevice(std::string p1,std::string p2) {
        struct stat astat;
        struct stat bstat;
        if (lstat(p1.c_str(),&astat)== -1) {
               return false;
        }
        if (lstat(p2.c_str(),&bstat)== -1) {
               return false;
        }
        if (astat.st_dev == bstat.st_dev) { 
           return true;
        }
        return false;
     }

     
     off_t MmlsTopNode::getSize() {
         return mImageHandle->size;        
     }
     
     void MmlsTopNode::openStream() {
        mReadOffset=0;
     }

     void MmlsTopNode::closeStream() {
        mReadOffset=0;
     }

     size_t MmlsTopNode::streamRead(char *buf, size_t count) {
        size_t rdbytes= tsk_img_read(mImageHandle, mReadOffset,buf,count);
        mReadOffset += rdbytes;
        return rdbytes;
     }

     std::string MmlsTopNode::getSoftLinkablePath(ocfa::misc::DigestPair **) {
        std::string path1=mFilenames[0];
        if ((mFilesCount == 1) && (path1.find(".crv",path1.size()-4) != std::string::npos )) {
           //If the input was a carvpath, this path can be returned as soft linkable path.
           return path1;
        } 
        return "";
     }

     std::string  MmlsTopNode::getHardLinkablePath(std::string targetbasepath,ocfa::misc::DigestPair **) {
        std::string path1=mFilenames[0];
        if ((mFilesCount == 1) && (path1.find(".crv",path1.size()-4) == std::string::npos )) {
          if (isOnSameDevice(targetbasepath,path1)) {
             //If the path isn't a carvpath or a part of an EWF image, and the input resides on the repository its filesystem,
             //then this path can be returned as hard linkable path.
             return path1;
          }
        }
        return "";
     }

     bool MmlsTopNode::hasSubEntities() {
         if ((mVsHandle) && (mVsHandle->part_count > 0)) {
            return true;
         }
         return false;
     }
  
     void MmlsTopNode::resetSubEntityIterator() {
         mActivePartitionIndex=0;
     }

     bool MmlsTopNode::nextSubEntity() {
         //The top node only returns slices that are not part of any table.
         //These can be unallocated slices or the primary partition table.
         while (mActivePartitionIndex < (mVsHandle->part_count -1)) {
            mActivePartitionIndex++;
            const TSK_VS_PART_INFO *partition=tsk_vs_part_get(mVsHandle,mActivePartitionIndex);
            if (partition->table_num == -1) {
               return true;
            }
         }
         return false;
     }    
 
     void MmlsTopNode::getCurrentSubEntity(TreeGraphNode ** subent) {
         if (*subent) {
            throw ocfa::misc::OcfaException("MmlsTopNode::getCurrentSubEntity called with subent not cleared");
         }
         std::string path1=mFilenames[0];
         const TSK_VS_PART_INFO *partition=tsk_vs_part_get(mVsHandle,mActivePartitionIndex); 
         off_t start  = partition->start * mVsHandle->block_size;
         off_t length = partition->len * mVsHandle->block_size;
         std::string description= partition->desc ; 
         while ((description.length()>1) && ((description.c_str()[0] == ' ') || (description.c_str()[0] == '\t'))) {
            description.erase(0,1);
         }
         while((description.length()>1) && ((description.c_str()[description.length()-1] == ' ') || (description.c_str()[description.length()-1] == '\t'))) {
            description.erase(description.length()-1,1);
         }
         bool allocated=(partition->flags & TSK_VS_PART_FLAG_ALLOC);
         bool meta=(partition->flags & TSK_VS_PART_FLAG_META);         

         size_t i = description.find("(#");
         if ( (i != std::string::npos) && 
             ((i+2) < description.length())
            ) {
            //Children can be a table (the primary in this case)
            int table_num = atoi(description.substr(i + 2, description.length() - 2).c_str());
            *subent=new MmlsTable(mCarvpathTop,mVsHandle,table_num,description,start,length,mActivePartitionIndex);
         } else {      
            //Or children can be leaf nodes.
            if ((mFilesCount == 1) && (path1.find(".crv",path1.size()-4) != std::string::npos)) {
                //Either representable by carv paths.
                *subent=new CarvPathPartition(mCarvpathTop,start,length,description,allocated,meta,mActivePartitionIndex);   
            } else {
                //Or only representable by a stream.
                *subent=new StreamPartition(mImageHandle,start,length,description,allocated,meta,mActivePartitionIndex);
            }
         }
         return;
     }

     std::string MmlsTopNode::getCurrentSubEntityRelation() {
        return "partitionentry"; 
     }

     std::string MmlsTopNode::getName() {
        return "diskimage";
     }

     void MmlsTopNode::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
         *map=mMeta;
         mMeta=0;
     }

     void unlinkOnDestruct() {
         ;
     }
  }
}
