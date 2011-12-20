#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "TskFsDirectory.hpp"
#include "TskFsDuplicateDirectory.hpp"
namespace ocfa {
  namespace tsk {

//#error need to fix this file
     TskFsDirectory::TskFsDirectory(TSK_FS_INFO *fs,carvpath_entity *cp,TSK_INUM_T inum,std::string name,TSK_FS_NAME *nameinfo,AbstractLoopDetector &parentdetector,std::string timesource,bool doslack,bool verbosemeta):ocfa::OcfaObject("TskFsDirectory","tsk"),mVerboseMeta(verbosemeta),mFileSystem(fs),mCarvPathTop(cp),mName(name),mEntryIndex(0),mDirectory(0),mAsFile(0),mLoopDetector(parentdetector,inum),mTimeSource(timesource),mDoSlack(doslack),mAltStreamMode(false),mMeta(0) {
         mDirectory=tsk_fs_dir_open_meta(fs,inum);
         if (mDirectory) {
             mAsFile = new TskFsInode(mFileSystem,mCarvPathTop,inum,name,nameinfo,mTimeSource,mDoSlack,mVerboseMeta);
         } else {
            getLogStream(ocfa::misc::LOG_WARNING) << "Unable to open directory '" << name << "' (inode=" << inum << ") for processing."  << std::endl;
            mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();
            ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("dir")));
            ocfa::misc::metautil::addMetaToMap(mMeta,"isdeleted", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(1)));
            ocfa::misc::metautil::addMetaToMap(mMeta,"valid", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(0)));
            ocfa::misc::metautil::addMetaToMap(mMeta,"specialmarker", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("invalid-directory-inode")));
         }
         //If we have no directory entries other than "." and "..", go directly to altstream mode.
         if ((mDirectory) && (mDirectory->names_used > 2)) {
            mAltStreamMode=false;
         } else {
            mAltStreamMode=true;;
         }
     }

     TskFsDirectory::~TskFsDirectory(){
         if (mAsFile) {
            delete mAsFile;
         }
         if (mDirectory) {
            tsk_fs_dir_close(mDirectory);
            mDirectory=0;
         }
         if (mMeta) {
            map < string, misc::MetaValue * >::const_iterator p;
            for (p = mMeta->begin(); p != mMeta->end(); ++p) {
               delete p->second;
            }
            delete mMeta;
            mMeta=0;
         }
         return;
     }

     bool TskFsDirectory::hasSubEntities() {
         //If the directory has more than two (. and ..) entries, we have sub entities.
         if ((mDirectory) && (mDirectory->names_used > 2)) {
           return true;
         }
         //If the directory has no sub entities, but the mAsFile does, we also have sub entities. 
         if (mAsFile) {
            return mAsFile->hasSubEntities();
         }
         return false;
     }
     void TskFsDirectory::resetSubEntityIterator() {
         getLogStream(ocfa::misc::LOG_DEBUG) << "Resetting sub entity iterator, this should normaly not be needed." << std::endl;
         mEntryIndex=0; 
         //If we have no directory entries other than "." and "..", go directly to altstream mode.
         if ((mDirectory) && (mDirectory->names_used > 2)) {
            mAltStreamMode=false;
         } else {
            mAltStreamMode=true;;
         }
     }
     bool TskFsDirectory::nextSubEntity() {
        if (mAsFile == 0) {
             getLogStream(ocfa::misc::LOG_ERR) << "ERROR: nextSubEntity returning false for invalid node" << std::endl;  
             return false;
        }
        if (mAltStreamMode == false) { //Regular directory entries first.
         if ((mDirectory) && (mDirectory->names_used > (mEntryIndex+1))) { //If any left move to the next directory entry.
            mEntryIndex++;
            return true;
         } else { //If no directory entries left, see if we have alt stream content.
            if (mAsFile->hasSubEntities() == false) {  //Return false if we don't.
                return false;
            }
            mAltStreamMode=true; //Set the mode to alternate streams if we do and return true.
            return true;
         }
       } else { //If in alt stream mode forward this call to our mAsFile.
          return mAsFile->nextSubEntity();
       }
     }
     void TskFsDirectory::getCurrentSubEntity(TreeGraphNode ** subent) {
        if (*subent) {
            throw ocfa::misc::OcfaException("ERROR: TskFsFileSystem::getCurrentSubEntity called with subent not cleared");
        }
        if (mAltStreamMode == false) { //If we are in regular directory entry mode, work to return the current directory entry.
           //Assert some prerequisites.
           if (mDirectory == 0) { 
              throw ocfa::misc::OcfaException("ERROR: mDirectory == 0",this);
           }
           if (mDirectory->names == 0) {
              throw ocfa::misc::OcfaException("ERROR: mDirectory->names == 0",this);
           }
           TSK_FS_NAME *curname=&(mDirectory->names[mEntryIndex]); //Fetch the prospective current sub entity from the sleuthkit names array.
           //Skip any non valid prospective directory entry to the next one if its a '.' or '..' entry.
           while ( (curname) && ((curname->name == 0) || (curname->name[0] == 0) || (std::string("..") == curname->name ) || (std::string(".") == curname->name)))  {
              if (mEntryIndex <= mDirectory->names_used) {
                  mEntryIndex++;
                  curname=&(mDirectory->names[mEntryIndex]);
              } else {
                  //If we have stepped passed the size of the name array, move on to altstream mode andtry to get the first sub entity there. 
                  getLogStream(ocfa::misc::LOG_ERR) << "ERROR: getCurrentSubEntity stepped outside of its range, attempting to recover." << std::endl;
                  curname=0;
                  mAltStreamMode=true;
                  if (mAsFile == 0) {
                     throw ocfa::misc::OcfaException("Unable to recover,  mAsFile is null for directory",this);
                  }
                  mAsFile->getCurrentSubEntity(subent);
                  if (*subent) {
                     throw ocfa::misc::OcfaException("Unable to recover,  mAsFile->getCurrentSubEntity returned NULL as sub entity.",this);
                  }
                  return; //Return the result of the recovery.
              }
           }
           if (curname) { //Check an other prerequisite.
             //Get the name(s) of the sub entity.
             std::string utf8name="<nameless>";
             if (curname->name) {
                   utf8name=curname->name;
             }
             std::string utf8shrtname="";
             if (curname->shrt_name) {
                utf8shrtname= curname->shrt_name;
             }
             //Get the string representation of the sleuthkit type of the named entry.
             std::string nametype(tsk_fs_name_type_str[curname->type]);
             //Get our subent inode number.
             TSK_INUM_T inode=curname->meta_addr;
             //Determine if the sub entity is allocated or not.
             bool allocated=false;
             if (curname->flags & TSK_FS_NAME_FLAG_ALLOC) {
                allocated=true;
             } 
             switch (curname->type) {
               case TSK_FS_NAME_TYPE_DIR:
                       //If the subent is an other directory check for loops in the parent chain to avoid unlimited looping in corrupted filesystem data (this includes overwritten deleted stuff).
                       if (mLoopDetector(inode)) {
                         getLogStream(ocfa::misc::LOG_ERR) << "Detected loop, creating dummy pseudo node" << std::endl;
                         *subent = new TskFsDuplicateDirectory(inode,utf8name,mVerboseMeta); //Creating a dummy subent is better here than throwing an exception.
                       } else {
                         *subent = new TskFsDirectory(mFileSystem,mCarvPathTop,inode,utf8name,curname,mLoopDetector,mTimeSource,mDoSlack,mVerboseMeta); //a regular dir.
                       }                
                       break;
               case TSK_FS_NAME_TYPE_UNDEF:
               case TSK_FS_NAME_TYPE_FIFO:
               case TSK_FS_NAME_TYPE_CHR:
               case TSK_FS_NAME_TYPE_BLK:
               case TSK_FS_NAME_TYPE_REG:
               case TSK_FS_NAME_TYPE_LNK:
               case TSK_FS_NAME_TYPE_SOCK:
               case TSK_FS_NAME_TYPE_SHAD:
               case TSK_FS_NAME_TYPE_WHT:
               case TSK_FS_NAME_TYPE_VIRT:
               default:
                       //Everyting other than a directory requires just a basic inode object.
                       *subent = new TskFsInode(mFileSystem,mCarvPathTop,inode,utf8name,curname,mTimeSource,mDoSlack,mVerboseMeta);
                       break;
             }
           } else {
              getLogStream(ocfa::misc::LOG_ERR) << "ERR: curname == 0, this should not happen!" << std::endl;
              throw ocfa::misc::OcfaException("Unable to recover,  curname ended up as NULL..",this);
           }
        } else {
           mAsFile->getCurrentSubEntity(subent);
        } 
     }

     std::string TskFsDirectory::getCurrentSubEntityRelation() {
       if (mAltStreamMode == false) { //Check if we are processing a regular directory entry.
        if ((mDirectory) && (mDirectory->names_used > 0)) { //Check some prerequisites.
           TSK_FS_NAME *curname=&(mDirectory->names[mEntryIndex]);
           if (curname) {
              if (curname-> flags & TSK_FS_NAME_FLAG_UNALLOC) {
                 return "removedentry";
              }
              switch (curname->type) { //Determine the entity relation from the type of the sub entity.
                 case TSK_FS_NAME_TYPE_FIFO:
                 case TSK_FS_NAME_TYPE_CHR:
                 case TSK_FS_NAME_TYPE_BLK:
                 case TSK_FS_NAME_TYPE_LNK:
                 case TSK_FS_NAME_TYPE_SOCK:
                 case TSK_FS_NAME_TYPE_SHAD:
                 case TSK_FS_NAME_TYPE_WHT:
                     return "specialdirentry"; 
                 case TSK_FS_NAME_TYPE_REG:
                     return "filedirentry";
                 case TSK_FS_NAME_TYPE_DIR:
                 case TSK_FS_NAME_TYPE_VIRT:
                     return "dirdirentry";
                 case TSK_FS_NAME_TYPE_UNDEF:
                     return "removedentry"; //FIXME, we need to do better here to make sure, libtsk seems ambiguous on this point, or we are doing something wrong.
                 default:
                     return "undefined";      
              }
           } else {
              getLogStream(ocfa::misc::LOG_ERR) << "ERROR: curname is NULL, this should not happen" << std::endl;
           }
        } else {
           getLogStream(ocfa::misc::LOG_ERR) << "ERROR: No entry at current index, this should not happen" << std::endl;
        }
        //If the prerequisites were not met for some reason, returning undef seems somewhat less destructive than throwing an exception.
        return "undefined";
       } else {
         //If we are in altstream mode, ask mAsFile what the relation is. 
         return mAsFile->getCurrentSubEntityRelation();
       }
     }

     std::string TskFsDirectory::getName(){
         return mName;
     }

     void TskFsDirectory::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
         if (*map != 0) {
             throw ocfa::misc::OcfaException("takeMetaMap called with non NULL map pointer.",this);
         }
         if (mAsFile) {
           mAsFile->takeMetaMap(map);
         } else {
            if (mMeta == 0) {
               throw ocfa::misc::OcfaException("takeMetaMap invoked for the second time",this);
            }
            *map=mMeta;
            mMeta=0;
         }
     }
     void TskFsDirectory::streamToOutput(ocfa::misc::AbstractWriteFacet &writefacet) {
          if (mAsFile) {
             //Under normal circumstances forward streamToOutput to the mAsFile.
             mAsFile->streamToOutput(writefacet);
          } else {
             //Doing nothing but logging seems somewhat less destructive than throwing an exception here.
             getLogStream(ocfa::misc::LOG_ERR) << "ERROR: No mAsFile defined" << std::endl;
          }
     }
  }
}
