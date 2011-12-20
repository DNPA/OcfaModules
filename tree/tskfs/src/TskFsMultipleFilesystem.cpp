#include "TskFsMultipleFilesystem.hpp"
#include "TskFsDirectory.hpp"
#include "misc.hpp"
namespace ocfa {
  namespace tsk {
TskFsMultipleFilesystem::TskFsMultipleFilesystem(TSK_IMG_INFO *img,carvpath_entity *cptop,int fscount,AbstractLoopDetector &parentdetector,std::string timesource,bool doslack,bool verbosemeta):ocfa::OcfaObject("TskFsMultipleFilesystem","tsk"),mImageHandle(img),mCarvpathTop(cptop),mFsCount(fscount),mActiveFsNum(0),mAnchor(parentdetector),mTimeSource(timesource),mDoSlack(doslack),mVerboseMeta(verbosemeta),mMeta(0),mFileSystem(0) {
  mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();
  ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("pseudodir")));
  getLogStream(ocfa::misc::LOG_WARNING) << "Processing partition with multiple filesystems." << std::endl;
}

TskFsMultipleFilesystem::~TskFsMultipleFilesystem() {
  getLogStream(ocfa::misc::LOG_WARNING) << "Done processing partition with multiple filesystems." << std::endl;
  if (mFileSystem) {
     tsk_fs_close(mFileSystem);
     mFileSystem=0;
  }
}

bool TskFsMultipleFilesystem::nextSubEntity(){
   if ((mFsCount-1) > mActiveFsNum) { 
     mActiveFsNum++;
     return true;
   } else {
     return false;
   }
}

void TskFsMultipleFilesystem::getCurrentSubEntity(TreeGraphNode ** subent) {
  if (*subent) {
            throw ocfa::misc::OcfaException("TskFsMultipleFilesystem::getCurrentSubEntity called with subent not cleared");
  }
  if (mFileSystem !=0) {
     tsk_fs_close(mFileSystem);
     mFileSystem=0;
  }

  int fscount=0;
  int fs;
  for (fs=0;fs<6;fs++){
    TSK_FS_TYPE_ENUM fstype=TSK_FS_TYPE_NTFS_DETECT;
    std::string fsname="<ntfs>";
    switch (fs) {
       case 1: fstype=TSK_FS_TYPE_FAT_DETECT;fsname="<fat>";break;
       case 2: fstype=TSK_FS_TYPE_FFS_DETECT;fsname="<ufs>";break;
       case 3: fstype=TSK_FS_TYPE_EXT_DETECT;fsname="<ext>";break;
       case 4: fstype=TSK_FS_TYPE_ISO9660_DETECT;fsname="<iso9660>";break;
       case 5: fstype=TSK_FS_TYPE_HFS_DETECT;fsname="<hfs>";break;
    }
    TSK_FS_INFO *testfilesystem=0;
    testfilesystem=tsk_fs_open_img(mImageHandle,0,fstype);
    if ((testfilesystem) && (testfilesystem->ftype != TSK_FS_TYPE_UNSUPP)) {
           if (fscount == mActiveFsNum) {
                mFileSystem=testfilesystem;
                if (fs == 2) {
                    getLogStream(ocfa::misc::LOG_WARNING) << "Processing UFS filesystem. With the 3.1.3 version of the sleuthkit lib this is known to be a bit unstable." << std::endl;
                } 
                *subent=new TskFsDirectory(mFileSystem,mCarvpathTop,mFileSystem->root_inum,fsname,0,mAnchor,mTimeSource,mDoSlack,mVerboseMeta);
                return;
           }
           fscount++;
           tsk_fs_close(testfilesystem);
    }
  }
  return; 
}

void TskFsMultipleFilesystem::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
             if (*map == 0) {
                  if (mMeta == 0) {
                      throw ocfa::misc::OcfaException("takeMetaMap invoked for the second time",this);
                  } 
                  *map=mMeta;
                  mMeta=0;
             } else {
                 throw ocfa::misc::OcfaException("takeMetaMap invoked with nonn NULLed map handle.",this);
             }
}

}}
