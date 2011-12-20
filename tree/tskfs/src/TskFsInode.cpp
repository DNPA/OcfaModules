#include "TskFsInode.hpp"
#include <limits>
#include "TskFsCarvPathDataAttribute.hpp"
#include "TskFsStreamDataAttribute.hpp"
#include "TskFsResidentDataAttribute.hpp"
#include "tsk3/libtsk.h"
namespace ocfa {
  namespace tsk {
          //Helper method for constructor for processing nameinfo derivable meta data.
          void TskFsInode::processNameInfoMeta(TSK_FS_NAME *nameinfo) {
                if (nameinfo->flags & TSK_FS_NAME_FLAG_UNALLOC) { // process the flags to see if this node is an allocated or a deleted file. 
                  ocfa::misc::metautil::addMetaToMap(mMeta,"isdeleted", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(1)));
                  mDeleted=true;
                } else {
                  ocfa::misc::metautil::addMetaToMap(mMeta,"isdeleted", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(0)));
                } 
                if (nameinfo->shrt_name) { //If the inode has a short name, add it as meta data.
                  ocfa::misc::metautil::addMetaToMap(mMeta,"shortname", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(nameinfo->shrt_name)));
                }
                //process the type from the nameinfo and set the nodetype metadata.
                std::string nodetype;
                nodetype="file";
                switch (nameinfo->type) {
                  case TSK_FS_NAME_TYPE_FIFO:  nodetype="fifo"; break;
                  case TSK_FS_NAME_TYPE_CHR:   nodetype="characterdevice"; break;
                  case TSK_FS_NAME_TYPE_DIR:   nodetype="dir"; break;
                  case TSK_FS_NAME_TYPE_BLK:   nodetype="blockdevice"; break;
                  case TSK_FS_NAME_TYPE_LNK:   nodetype="softlink"; break;
                  case TSK_FS_NAME_TYPE_SOCK:  nodetype="socket"; break;
                  case TSK_FS_NAME_TYPE_WHT:   nodetype="whiteout"; break;
                  case TSK_FS_NAME_TYPE_VIRT:  nodetype="libtsk3_virtual"; break;
                  case TSK_FS_NAME_TYPE_SHAD:  nodetype="shad";break;
                  case TSK_FS_NAME_TYPE_UNDEF: nodetype="file";break;
                  case TSK_FS_NAME_TYPE_REG:   break;
                  default: nodetype="file";
                }
                ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(nodetype)));
          }
          //constructor helper for processing sleuthkit meta structure.
          void TskFsInode::processFileInfoMeta(TSK_FS_META *meta,std::string timesource) {
                   const ocfa::misc::DateTime *dt=0;
                   if (mVerboseMeta) { //uid, gid, dbtime, tsk-type, mode, nlink  and seq are considered verbose meta.
                     ocfa::misc::metautil::addMetaToMap(mMeta,"tsk-type", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(tsk_fs_meta_type_str[meta->type])));
                     ocfa::misc::metautil::addMetaToMap(mMeta,"mode", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long)(meta->mode))));
                     ocfa::misc::metautil::addMetaToMap(mMeta,"nlink", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long)(meta->nlink))));
                     ocfa::misc::metautil::addMetaToMap(mMeta,"uid", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long)(meta->uid))));
                     ocfa::misc::metautil::addMetaToMap(mMeta,"gid", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long int)(meta->gid))));
                     ocfa::misc::metautil::addMetaToMap(mMeta,"seq", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long int)(meta->seq))));
                   } 
                   if (meta->time2.ext2.dtime > 0) { 
                     dt=new ocfa::misc::DateTime(meta->time2.ext2.dtime,timesource);
                     if (mDeleted){
                       ocfa::misc::metautil::addMetaToMap(mMeta,"deletetime", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((const ocfa::misc::DateTime**)&dt)));
                     } else {
                       ocfa::misc::metautil::addMetaToMap(mMeta,"backuptime", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((const ocfa::misc::DateTime**)&dt)));
                     }
                   }
                   if (std::string(tsk_fs_meta_type_str[meta->type]) != "d") {
                      ocfa::misc::metautil::addMetaToMap(mMeta,"size", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long int)(meta->size))));
                      if (meta->size == 0) {
                         ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("emptyfile")));
                      } 
                   }
                   if (meta->mtime > 0) {
                     dt=new ocfa::misc::DateTime(meta->mtime,timesource);
                     ocfa::misc::metautil::addMetaToMap(mMeta,"mtime", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((const ocfa::misc::DateTime**)&dt)));
                   }
                   if (meta->ctime > 0) {
                     dt=new ocfa::misc::DateTime(meta->ctime,timesource);
                     ocfa::misc::metautil::addMetaToMap(mMeta,"ctime", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((const ocfa::misc::DateTime**)&dt)));
                   }
                   if (meta->crtime > 0) {
                     dt=new ocfa::misc::DateTime(meta->crtime,timesource);
                     ocfa::misc::metautil::addMetaToMap(mMeta,"crtime", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((const ocfa::misc::DateTime**)&dt)));
                   }
                   if (mTskFile->meta->link) {
                       ocfa::misc::metautil::addMetaToMap(mMeta,"symlink", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(meta->link)));
                   }

          }
 
          //Helper function for processing NTFS SI attribute.
          void TskFsInode::processNtfsStandardInfo(const TSK_FS_ATTR *attribute){
                //Define a standard_info_struct to binary cast our attribute data to.
                struct standard_info_struct {
                   long long int creation_time;
                   long long int alter_time;
                   long long int mft_alter_time;
                   long long int access_time;
                   long long int standard_flags;
                   long long int max_versions;
                   long long int version;
                   long long int class_id;
                   long long int owner_id;
                   long long int security_id;
                   long long int quota_charged;
                   long long int usn;
                } *standard_info;
                if (attribute->rd.buf_size >= sizeof(struct standard_info_struct)) {
                     standard_info = (standard_info_struct *) attribute->rd.buf;
                     ocfa::misc::metautil::addMetaToMap(mMeta,"si_version", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long int) standard_info->version)));
                     ocfa::misc::metautil::addMetaToMap(mMeta,"si_max_version", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long int) standard_info->max_versions)));
                     ocfa::misc::metautil::addMetaToMap(mMeta,"si_security_id", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long int) standard_info->security_id)));
                     ocfa::misc::metautil::addMetaToMap(mMeta,"si_class_id", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long int) standard_info->class_id)));
                     ocfa::misc::metautil::addMetaToMap(mMeta,"si_quota_charged", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long int) standard_info->quota_charged)));
                     ocfa::misc::metautil::addMetaToMap(mMeta,"si_usn", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long int) standard_info->usn)));
                     if (standard_info->standard_flags & 0x0001) {
                         ocfa::misc::metautil::addMetaToMap(mMeta,"si_readonly", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(1)));
                     } else {
                         ocfa::misc::metautil::addMetaToMap(mMeta,"si_readonly", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(0)));
                     }
                     if (standard_info->standard_flags & 0x0002) {
                         ocfa::misc::metautil::addMetaToMap(mMeta,"si_hidden", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(1)));
                     } else {
                         ocfa::misc::metautil::addMetaToMap(mMeta,"si_hidden", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(0)));
                     }
                     if (standard_info->standard_flags & 0x0004) {
                         ocfa::misc::metautil::addMetaToMap(mMeta,"si_system", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(1)));
                     } else {
                         ocfa::misc::metautil::addMetaToMap(mMeta,"si_system", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(0)));
                     }
                     if (standard_info->standard_flags & 0x0020) {
                          ocfa::misc::metautil::addMetaToMap(mMeta,"si_archive", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(1)));
                     } else {
                          ocfa::misc::metautil::addMetaToMap(mMeta,"si_archive", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(0)));
                     }
                     if (standard_info->standard_flags & 0x0100) {
                         ocfa::misc::metautil::addMetaToMap(mMeta,"si_temporary", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(1)));
                     } else {
                         ocfa::misc::metautil::addMetaToMap(mMeta,"si_temporary", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(0)));
                     }
                     if (standard_info->standard_flags & 0x0200) {
                         ocfa::misc::metautil::addMetaToMap(mMeta,"si_sparse", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(1)));
                     } else {
                         ocfa::misc::metautil::addMetaToMap(mMeta,"si_sparse", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(0)));
                     }
                     if (standard_info->standard_flags & 0x0400) {
                         ocfa::misc::metautil::addMetaToMap(mMeta,"si_reparsepoint", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(1)));
                     } else {
                         ocfa::misc::metautil::addMetaToMap(mMeta,"si_reparsepoint", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(0)));
                     }
                     if (standard_info->standard_flags & 0x0800) {
                         ocfa::misc::metautil::addMetaToMap(mMeta,"si_compressed", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(1)));
                     } else {
                         ocfa::misc::metautil::addMetaToMap(mMeta,"si_compressed", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(0)));
                     }
                     if (standard_info->standard_flags & 0x4000) {
                         ocfa::misc::metautil::addMetaToMap(mMeta,"si_encrypted", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(1)));
                     } else {
                         ocfa::misc::metautil::addMetaToMap(mMeta,"si_encrypted", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(0)));
                     }
                } else {
                    getLogStream(ocfa::misc::LOG_ERR) << "ERROR: bad sized NTFS SI attribute." << std::endl;
                }
          }

          //Helper function for constructor for processing data attribute.
          attributetype TskFsInode::processAttribute(const TSK_FS_ATTR *attribute,std::string name) {
              if (attribute->size == 0) {
                 return ATTR_NODATA;
              }
//LibTsk interface for distinguishing between primary and alternate streams changed with the 3.2.0 version. 
#if (TSK_VERSION_NUM < 0x03020000)
              if (((attribute->name) && ((std::string(attribute->name) == "$Data")) && (attribute->type == TSK_FS_ATTR_TYPE_NTFS_DATA))||(attribute->type == TSK_FS_ATTR_TYPE_DEFAULT)) { //Process normal data attribute.
#else
              if (((attribute->name == 0) && (attribute->type == TSK_FS_ATTR_TYPE_NTFS_DATA)) || (attribute->type == TSK_FS_ATTR_TYPE_DEFAULT)) {
#endif
                   if (name =="EFS0.TMP") { //Special marker for EFS0.TMP file.
                      ocfa::misc::metautil::addMetaToMap(mMeta,"specialmarker", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("efs-crypto-temp")));
                   }
                   return ATTR_PRIMARY;
              }
              if (attribute->name && (std::string(attribute->name) == "$I30")) {
                  return ATTR_NODATA;
              }
              if (attribute->type == TSK_FS_ATTR_TYPE_NTFS_BITMAP) {
                 return ATTR_NODATA;
              }
              if (attribute->flags & TSK_FS_ATTR_NONRES) { //All non resident attributes are processed as data attributes.
                 return ATTR_DATA;
              } else {
                 switch (attribute->type) {
                      case TSK_FS_ATTR_TYPE_NTFS_SI:
			    if (mVerboseMeta) { //Only process SI information in verbose meta mode. 
                               processNtfsStandardInfo(attribute);
                            }
                            //fall trough OK.
                      case TSK_FS_ATTR_TYPE_NTFS_FNAME:
                           //fall trough OK.
                      case TSK_FS_ATTR_TYPE_NTFS_VVER:
                            return ATTR_NODATA;
                      default:
                            return ATTR_DATA;
                 }
              }
          }

          TskFsInode::TskFsInode( TSK_FS_INFO *fs,carvpath_entity *cp,TSK_INUM_T inum,std::string name,TSK_FS_NAME *nameinfo,std::string timesource,bool doslack,bool verbosemeta):OcfaObject("TskFsInode","tskfs"),mVerboseMeta(verbosemeta),mMeta(0),mFs(fs),mCp(cp),mName(name),mDoSlack(doslack),mMainStream(0),mMainStreamIndex(std::numeric_limits<std::size_t>::max()),mSlackProcessed(true),mValidDataStreamCount(0),mFirstValidStreamIndex(0),mLastValidStreamIndex(0),mTskFile(0),mStreamIndex(0),mDeleted(false) {
              mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();
              if (mVerboseMeta) {
                //The inode is considered verbose meta data.
                ocfa::misc::metautil::addMetaToMap(mMeta,"inode_no", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long) inum)));
              }
              std::string nodetype="undefined";
              std::string nodename="<topdir>";
              if (nameinfo) { //Normaly we have a nameinfo.
                long long int filesize=0;
                processNameInfoMeta(nameinfo);
                nodename=nameinfo->name;
                mTskFile=tsk_fs_file_open_meta(fs,0,nameinfo->meta_addr); //Open the inode object from a meta name.
              } else { //The top dir has no nameinfo.
                 std::string nodetype="dir";
                 ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(nodetype)));
                 mTskFile=tsk_fs_file_open_meta(fs,0,inum); //Open the inode object from an inode number.
              }
              if (mTskFile && mTskFile->meta) { //If possible process file structure meta. 
                   processFileInfoMeta(mTskFile->meta,timesource);
              }
              size_t attrcount=tsk_fs_file_attr_getsize(mTskFile);  //Get the number of attributes.
              mValidDataStreamCount=0;
              size_t index;
              for (index=0;index<attrcount;index++) { //process each attribute of the inode.
                 getLogStream(ocfa::misc::LOG_DEBUG) << "Index=" <<  index << std::endl;
                 bool isdataattr=false;
                 bool isprimarydataattr=false;
                 const TSK_FS_ATTR *attribute=tsk_fs_file_attr_get_idx(mTskFile,index); //fetch the attribute.
                 attributetype currattrtype=ATTR_NODATA; //Set the default attribute type as a non data one.
                 if (attribute) {
                    currattrtype=processAttribute(attribute,nodename); // Process the attribute in a basic way and determine its type.
                 }
                 if ((currattrtype == ATTR_PRIMARY) && (mMainStream == 0)) { //Process the attribute marked as primary, normaly this is the base file content attribute.
                     if (attribute->flags & TSK_FS_ATTR_RES) { //Resident attributes require a TskFsResidentDataAttribute object.
                          mMainStream = new TskFsResidentDataAttribute(mFs,attribute,mDoSlack,mTskFile->meta->size,mVerboseMeta);
                     } else {
                          if ((attribute->flags & TSK_FS_ATTR_COMP) || (mCp == 0)) { //Compressed files and non carvpath images require a TskFsStreamDataAttribute object.
                              mMainStream = new TskFsStreamDataAttribute(mFs,attribute,mDoSlack,mTskFile->meta->size,mVerboseMeta);
                          } else { //If nothing disalows it, carvpath data attributes are best.
                              mMainStream = new TskFsCarvPathDataAttribute(mFs,mCp,attribute,mDoSlack,mTskFile->meta->size,mVerboseMeta);
                          }
                     }
                     if (mDoSlack && mMainStream && mMainStream->hasSubEntities()) {
                          mSlackProcessed=false;  //If we need to process slack, and our main stream has slack, set a flag so we will know to process it later on.
                     } else {
                          mSlackProcessed=true;
                     }
                     mMainStreamIndex=index; //Set the index for our mainstream attribute.
                     std::map < std::string, ocfa::misc::MetaValue * > *mainstreammetamap=0;
                     //From the mainstream, copy all meta data other than "nodetype".
                     mMainStream->takeMetaMap(&mainstreammetamap); 
                     if (mainstreammetamap) {
                          map < string, misc::MetaValue * >::const_iterator p;
                          for (p = mainstreammetamap->begin(); p != mainstreammetamap->end(); ++p) {
                              if (p->first == "nodetype") {
                                delete p->second;
                              } else { 
                                ocfa::misc::metautil::addMetaToMap(mMeta,p->first,p->second );
                                (*mMeta)[p->first] = p->second;
                              }          
                          }
                          delete mainstreammetamap;
                     }
                 } else if ((currattrtype == ATTR_PRIMARY) || (currattrtype == ATTR_DATA)) { //Process non primary data attribute.
                   if (mValidDataStreamCount==0) { //If we have no valid altstreams yet, set the mFirstValidStreamIndex to this one.
                       mFirstValidStreamIndex=index;
                       mStreamIndex=mFirstValidStreamIndex;
                   }
                   mValidDataStreamCount++;      // Update the number of valid altstreams.
                   mLastValidStreamIndex=index;  // Update the last valid altstream index.
                 }
              }
          }
          TskFsInode::~TskFsInode() {
            if (mMainStream) {
               delete mMainStream;
               mMainStream=0;
            } 
            if (mTskFile) {
              tsk_fs_file_close(mTskFile);
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
          std::string TskFsInode::getName(){
             return mName;
          }
          void TskFsInode::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
             if (*map == 0) {
                  if (mMeta == 0) {
                     throw ocfa::misc::OcfaException("takeMetaMap incoked for the second time",this);
                  }
                  *map=mMeta;
                  mMeta=0;
             } else {
                 throw ocfa::misc::OcfaException("takeMetaMap invoked with nonn NULLed map handle.",this);
             }
          }
          bool TskFsInode::hasContent() {
             if (mMainStream) {
                return true;
             }
             return false;
          }
          off_t TskFsInode::getSize() {
             if  (mMainStream) {
                return mMainStream->getSize();
             }
             return 0;
          }
          void TskFsInode::streamToOutput(ocfa::misc::AbstractWriteFacet &writefacet){
             if  (mMainStream) {
                mMainStream->streamToOutput(writefacet);
             } else {
                getLogStream(ocfa::misc::LOG_ERR) << "ERROR: Inode with no valid mMainStream, this should not happen." << std::endl;
             }
          }
          std::string TskFsInode::getSoftLinkablePath(ocfa::misc::DigestPair **dp) {
             if (mMainStream) {
               return mMainStream->getSoftLinkablePath(dp);
             } else {
               getLogStream(ocfa::misc::LOG_ERR) << "ERROR: Inode with no valid mMainStream, this should not happen." << std::endl;
             }
             return "";
          }
          bool TskFsInode::hasSubEntities(){
             return ((mValidDataStreamCount > 0)||(mMainStream && mMainStream->hasSubEntities()));
          }
          void TskFsInode::resetSubEntityIterator() {
             mStreamIndex=mFirstValidStreamIndex;
             if (mMainStream) {
                mMainStream->resetSubEntityIterator();
             }
	     mSlackProcessed=true;
             if (mMainStream && mDoSlack && mMainStream->hasSubEntities()) {
                mSlackProcessed=false;
             }
          }

          bool TskFsInode::nextSubEntity() {
            if (mSlackProcessed==false) { //If we are in slack processing mode, move out of this mode to altstream processing mode.
               mSlackProcessed=true;
               if (mValidDataStreamCount > 0) { //If there are altstreams, return true
                  return true;
               }
               return false; //It there arn't, there is no next sub entity so we return false;
            } 
            //We were in altstream processing mode already.
            if (mStreamIndex >= mLastValidStreamIndex) { //Dont move past the end.
               return false;
            }
            size_t whilecount=0;
            while ((mStreamIndex < mLastValidStreamIndex)&&(whilecount < 10000)) { //FIXME, need t take this whilecount out eventually, need to test first. // CODE REVIEW OV : Fix this
               whilecount++;
               mStreamIndex++;
               if (mStreamIndex != mMainStreamIndex) {
                 const TSK_FS_ATTR *attribute=tsk_fs_file_attr_get_idx(mTskFile,mStreamIndex); //Fetch the next attribute.
                 if (attribute){
                   if (attribute->flags & TSK_FS_ATTR_NONRES) { //Non resident attributes are always OK 
                        return true; 
                   }
                   if (!((attribute->type == TSK_FS_ATTR_TYPE_NTFS_SI) || (attribute->type == TSK_FS_ATTR_TYPE_NTFS_FNAME) || (attribute->type == TSK_FS_ATTR_TYPE_NTFS_BITMAP)||(attribute->type == TSK_FS_ATTR_TYPE_NTFS_VVER))) { //Unless the attribute is a parsed or excluded meta attribute, return as altstream.
                        return true;
                   }
                 }
               }     
            }
            if (whilecount >= 10000) {
               getLogStream(ocfa::misc::LOG_ERR) << "ERROR: unexpected long loop in while loop, this should not happen." << std::endl;
            }
            return false;
          }

          void TskFsInode::getCurrentSubEntity(TreeGraphNode ** subent) {
             if (mSlackProcessed == false) { //If we are in slack mode forward this call to mMainStream.
                 if (mMainStream) {
                   mMainStream->getCurrentSubEntity(subent);
                 } else {
                    throw ocfa::misc::OcfaException("getCurrentSubEntity called on node without valid mMainStream, unable to recover.",this);
                 }
             } else { //If we are in altstream mode, create an new attribute object of the appropriate type.
               const TSK_FS_ATTR *attribute=tsk_fs_file_attr_get_idx(mTskFile,mStreamIndex);
               if (attribute){
                 if (attribute->flags & TSK_FS_ATTR_RES) {
                     *subent = new TskFsResidentDataAttribute(mFs,attribute,mDoSlack,attribute->size,mVerboseMeta);
                 } else {
                    if ((attribute->flags & TSK_FS_ATTR_COMP) || (mCp == 0)) {
                         *subent = new TskFsStreamDataAttribute(mFs,attribute,mDoSlack,attribute->size,mVerboseMeta);
                    } else {
                         *subent = new TskFsCarvPathDataAttribute(mFs,mCp,attribute,mDoSlack,attribute->size,mVerboseMeta);
                    }
                 }
               }
             }
          }

          
          std::string TskFsInode::getCurrentSubEntityRelation() {
             if (mSlackProcessed == false) {
                 return "unallocated";
             }
             return "alternatestream";
          }

  }
}


