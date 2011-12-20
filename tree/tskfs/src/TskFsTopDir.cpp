#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include "misc.hpp"
#include "TskFsTopDir.hpp"
#include "TskFsDirectory.hpp"
#include "TskFsMultipleFilesystem.hpp"
//#include "TskFsSystemDir.hpp"
//#include "TskFsOrphanesDir.hpp"
//#include "TskFsUnalocDir.hpp"
namespace ocfa {
  namespace tsk {

     TskFsTopDir::TskFsTopDir(std::string path,std::string timesource,bool dorawslack,bool docpslack,bool verbosemeta,carvpath_library *cplib):OcfaObject("TskFsTopDir","tskfs"),mTopDir(0),mVerboseMeta(verbosemeta),mMeta(0),mImageHandle(0),mCarvpathTop(0),mFileSystem(0) {
          //Make sure our path if a symbolic link gets derefernced first.
          std::string realpath=ocfa::misc::fsutil::dereferencePath(path);
          //Unless otherwise determined, the input is assumed to be a single image raw (dd) file.
          TSK_IMG_TYPE_ENUM imagetype=TSK_IMG_TYPE_RAW_SING;
          bool doslack=dorawslack;          
          if ((realpath.size() > 4) && (realpath.find(".crv",realpath.size()-4) != std::string::npos )) {
             //If our single file image is contained in carvFs, set our mCarvpathTop with a usable carvpath top entity
             //that we later can derive other carvpaths from. 
             std::string cpbasename=realpath.substr(0, realpath.size() - 4);
             struct stat basestat;
             if (lstat(realpath.c_str(),&basestat)== -1) {
               throw ocfa::misc::OcfaException("Problem calling stat on "+realpath,this);
             }
             mCarvpathTop = carvpath_top_entity(basestat.st_size,const_cast<char *>(cpbasename.c_str()),cplib);
             if (mCarvpathTop == 0) {
                  getLogStream(ocfa::misc::LOG_ERR) << "Problem creating carvpath top entity from '" << cpbasename << "' :" <<  carvpath_error_as_string(errno) << std::endl;          
             } else {
                doslack=docpslack;
             }
          } else {
               getLogStream(ocfa::misc::LOG_NOTICE) << "Tskfs running in RAW (copy-out) mode for '" << realpath << "'. This will take up much storage!" << std::endl; 
          }
          //Now lets talk to libtsk and open our image file.
          const TSK_TCHAR *filename=(const TSK_TCHAR *) malloc(path.size()+1);
          strcpy(const_cast<char *>(filename),path.c_str());
#ifdef TSK_VERSION_NUM
          mImageHandle=tsk_img_open(1,&filename,imagetype,0);
#else
          mImageHandle=tsk_img_open(1,&filename,imagetype);
#endif
          free((void *) filename); 
          if (mImageHandle == 0) {
             tsk_error_print(stderr);
             throw ocfa::misc::OcfaException(std::string("Problem opening image with the sleuthkit library: ")+path,this);
          }
          mFileSystem = tsk_fs_open_img(mImageHandle,0,TSK_FS_TYPE_DETECT);
          if ((mFileSystem) && (mFileSystem->ftype != TSK_FS_TYPE_UNSUPP)) {
             if (mFileSystem->ftype & TSK_FS_TYPE_FFS_DETECT ) {
                getLogStream(ocfa::misc::LOG_WARNING) << "Processing UFS filesystem, With the 3.1.3 version of the sleuthkit lib this is known to be a bit unstable." << std::endl;
             }
             mTopDir = new TskFsDirectory(mFileSystem,mCarvpathTop,mFileSystem->root_inum,"<root>",0,mAnchor,timesource,doslack,mVerboseMeta);
          } else {
                int fscount=0;
                int fs;
                for (fs=0;fs<6;fs++){
                   TSK_FS_TYPE_ENUM fstype=TSK_FS_TYPE_NTFS_DETECT;
                   switch (fs) {
                     case 1: fstype=TSK_FS_TYPE_FAT_DETECT;break;
                     case 2: fstype=TSK_FS_TYPE_FFS_DETECT;break;
                     case 3: fstype=TSK_FS_TYPE_EXT_DETECT;break;
                     case 4: fstype=TSK_FS_TYPE_ISO9660_DETECT;break;
                     case 5: fstype=TSK_FS_TYPE_HFS_DETECT;break;
                   }
                   TSK_FS_INFO *testfilesystem=tsk_fs_open_img(mImageHandle,0,fstype);
                   if ((testfilesystem) && (testfilesystem->ftype != TSK_FS_TYPE_UNSUPP)) {
                      fscount++;
                      tsk_fs_close(testfilesystem);
                   }  
                }
                if (fscount < 2) {
                  getLogStream(ocfa::misc::LOG_ERR) << "Unsupported filesystem on partition image:" << path << std::endl;
                  mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();      
                  ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("undefined")));
                  ocfa::misc::metautil::addMetaToMap(mMeta,"module-result", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("failure")));
                  ocfa::misc::metautil::addMetaToMap(mMeta,"specialmarker", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("unsupported-filesystem")));
                } else {
                    mTopDir = new TskFsMultipleFilesystem(mImageHandle,mCarvpathTop,fscount,mAnchor,timesource,doslack,mVerboseMeta);
                }
          }
     }

     TskFsTopDir::~TskFsTopDir() {
         if (mTopDir) {
             delete mTopDir;
             mTopDir=0;
         }
         if (mFileSystem) {
            tsk_fs_close(mFileSystem);
            mFileSystem=0;
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


     bool TskFsTopDir::hasContent() {
        return false;
     }

     off_t TskFsTopDir::getSize() {
         return 0;        
     }
 
     void TskFsTopDir::streamToOutput(ocfa::misc::AbstractWriteFacet &writefacet) {
          if (mTopDir) {
             mTopDir->streamToOutput(writefacet);
          } else {
             getLogStream(ocfa::misc::LOG_WARNING) << "No mTopDir defined, filesystem probably not supported by libtsk." << std::endl;
          }
     }
 
     bool TskFsTopDir::hasSubEntities() {
         if (mTopDir) {
            return mTopDir->hasSubEntities();
         } else {
             getLogStream(ocfa::misc::LOG_WARNING) << "No mTopDir defined, filesystem probably not supported by libtsk." << std::endl;
         }
         return false;
     }
  
     void TskFsTopDir::resetSubEntityIterator() {
         if (mTopDir) {
            mTopDir->resetSubEntityIterator();
         } else {
             getLogStream(ocfa::misc::LOG_WARNING) << "No mTopDir defined, filesystem probably not supported by libtsk." << std::endl;
         }
     }

     bool TskFsTopDir::nextSubEntity() {
         if (mTopDir) {
             return mTopDir->nextSubEntity();
         } else {
             getLogStream(ocfa::misc::LOG_WARNING) << "No mTopDir defined, filesystem probably not supported by libtsk." << std::endl;
         }
         return false;
     }    
 
     void TskFsTopDir::getCurrentSubEntity(TreeGraphNode ** subent) {
         if (*subent) {
            throw ocfa::misc::OcfaException("TskFsFileSystem::getCurrentSubEntity called with subent not cleared");
         }
         if (mTopDir) {
             return mTopDir->getCurrentSubEntity(subent);
         } else {
             getLogStream(ocfa::misc::LOG_WARNING) << "No mTopDir defined, filesystem probably not supported by libtsk." << std::endl;
         }
         return;
     }

     std::string TskFsTopDir::getCurrentSubEntityRelation() {
        if (mTopDir) {
            return mTopDir->getCurrentSubEntityRelation();
        } else {
             getLogStream(ocfa::misc::LOG_WARNING) << "No mTopDir defined, filesystem probably not supported by libtsk." << std::endl;
        }
        return "undefined"; 
     }

     std::string TskFsTopDir::getName() {
        if ((mFileSystem) && (mFileSystem->ftype != TSK_FS_TYPE_UNSUPP)) {
           return "<root>";
        }
        return "<no-or-unsupported-filesystem>";
     }

     
     void TskFsTopDir::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
         if (*map != 0) {
            throw ocfa::misc::OcfaException("takeMetaMap invoked with non NULL map pointer.",this);
         }
         if (mMeta) {
            *map=mMeta;
            mMeta=0;
         } else {
            if (mTopDir) {
               mTopDir->takeMetaMap(map);
            }
         } 
     }

  }
}
