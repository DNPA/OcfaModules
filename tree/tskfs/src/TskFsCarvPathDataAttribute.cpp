#include "TskFsStreamDataAttribute.hpp"  
#include "TskFsCarvPathFileSlack.hpp"
#include "TskFsCarvPathDataAttribute.hpp"
#include <libcarvpath.h>
#include <errno.h>

typedef struct {
  carvpath_entity *parent;
  carvpath_entity *child;
} carvpath_childparent;

namespace ocfa {
  namespace tsk {
          //Helper struct for walking the data block and composing a carvpath.
          typedef struct {
             carvpath_entity *parent; //The parent carvpath of the filesystem partition.
             carvpath_entity *child; //The child carvpath being composed.
             size_t fsblocksize; //The block size of the filesystem.
          } carvpath_childparent;
                                                                 
          TskFsCarvPathDataAttribute::TskFsCarvPathDataAttribute(TSK_FS_INFO *fs,carvpath_entity *topcp,const TSK_FS_ATTR *attribute,bool doslack,off_t realsize,bool verbosemeta):TskFsBaseDataAttribute(fs,attribute,doslack,realsize,verbosemeta) {
	     updateTypeName("TskFsCarvPathDataAttribute");
             ocfa::misc::metautil::addMetaToMap(mMeta,"resident", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(0)));
             carvpath_childparent cp;  //Our helper struct.
             cp.parent=topcp;;         //set the patent carvpath.
             cp.child=0;               //clear the child carvpath.
             cp.fsblocksize=fs->block_size; //set the filesystem block size.
             try {
                tsk_fs_attr_walk(mAttribute,TSK_FS_FILE_WALK_FLAG_AONLY,streamdata_action,&cp); //Let sleuthkit lib walk the data blocks using the streamdata_action method.
             } catch (std::exception &e) {
                getLogStream(ocfa::misc::LOG_ERR) << "Exception while calling tsk_fs_attr_walk" << std::endl;
                throw(e);
             }
             if (cp.child) {
               mSlackSize=0;
               carvpath_entity *filedata=carvpath_derive(cp.child,0,mRealSize,CARVPATH_OOR_SPARSE); //Derive the carvpath of the non slack part of the file data blocks carvpath.
               if (filedata == 0) {
                   getLogStream(ocfa::misc::LOG_ERR) << "Problem deriving non-slack carvpath entity from child carvpath :" <<  carvpath_error_as_string(errno) << std::endl;
               } else {
                 char *cfiledata=carvpath_get_as_path(filedata);
                 if (cfiledata == 0) {
                   getLogStream(ocfa::misc::LOG_ERR) << "Problem fething string representation from carvpath entity :" <<  carvpath_error_as_string(errno) << std::endl;
                   throw ocfa::misc::OcfaException("carvpath library problem");
                 } 
                 mCarvPath = std::string(cfiledata) + ".crv"; // Convert the carvpath to a string.
                 carvpath_free(filedata,0); //Free the filedata carvpath.
               }
               if (mDoSlack && (mRealSize < carvpath_get_size(cp.child,0))) {
                  //If we should do slack, derive a carvpath for the file slack.
                  carvpath_entity *fileslack=carvpath_derive(cp.child,mRealSize,carvpath_get_size(topcp,0)-mRealSize,CARVPATH_OOR_SPARSE);
                  if (fileslack == 0) {
                    getLogStream(ocfa::misc::LOG_ERR) << "Problem deriving slack carvpath entity from child carvpath :" <<  carvpath_error_as_string(errno) << std::endl;
                  } else {
                    mSlackSize=carvpath_get_size(topcp,0)-mRealSize;
                    char *cfileslack=carvpath_get_as_path(fileslack);
                    if (cfileslack == 0) {
                       getLogStream(ocfa::misc::LOG_ERR) << "Problem fething string representation from slack carvpath entity :" <<  carvpath_error_as_string(errno) << std::endl;
                       throw ocfa::misc::OcfaException("carvpath library problem");
                    }
                    mSlackCarvPath=std::string(cfileslack) + ".crv"; //convert the carvpath to a string.
                    carvpath_free(fileslack,0); // free the fileslack carvpath.
                  }
               }
               carvpath_free(cp.child,0); //free the clild entity carvpath.
             } 
             if (mCarvPath == "")  {
                carvpath_entity *emptycp=carvpath_parse(topcp,(char *) "0+0",CARVPATH_OOR_SPARSE);
                if (emptycp == 0) {
                   getLogStream(ocfa::misc::LOG_ERR) << "Unable to parse 0+0 entity:" <<  carvpath_error_as_string(errno) << std::endl;
                   throw ocfa::misc::OcfaException("carvpath library problem");
                }
                char *cemptycp=carvpath_get_as_path(emptycp);
                if (cemptycp == 0) {
                    getLogStream(ocfa::misc::LOG_ERR) << "Problem fething string representation from empty carvpath entity :" <<  carvpath_error_as_string(errno) << std::endl;
                    throw ocfa::misc::OcfaException("carvpath library problem");
                }
                mCarvPath = std::string(cemptycp) + ".crv"; // Convert the carvpath to a string.
                carvpath_free(emptycp,0);
                ocfa::misc::metautil::addMetaToMap(mMeta,"validsize", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(0)));
                ocfa::misc::metautil::addMetaToMap(mMeta,"specialmarker", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("invalidsize")));
                mRealSize=0;
                mSlackSize=0;
             } 
          }

          TskFsCarvPathDataAttribute::~TskFsCarvPathDataAttribute(){
          }
          
          std::string TskFsCarvPathDataAttribute::getSoftLinkablePath(ocfa::misc::DigestPair **) {
             return mCarvPath;
          }
          void TskFsCarvPathDataAttribute::getCurrentSubEntity(TreeGraphNode ** subent){
               *subent = new TskFsCarvPathFileSlack(mSlackCarvPath,mSlackSize,mVerboseMeta);
               return;
          }
          bool TskFsCarvPathDataAttribute::hasSubEntities(){
               if ((mSlackCarvPath != "") && mDoSlack && (mSlackSize >0)) {
                 return true;
               }
               return false;
          }

         //Callback for the sleuthkit tsk_fs_attr_walk function.
         TSK_WALK_RET_ENUM TskFsCarvPathDataAttribute::streamdata_action(TSK_FS_FILE *file, TSK_OFF_T offset, TSK_DADDR_T daddr, char *buf , size_t size, TSK_FS_BLOCK_FLAG_ENUM flags, void *ptr)
         {
            carvpath_childparent *pc=(carvpath_childparent *) ptr; //Cast the state pinter to our parent child structure.
            if (pc->child) {
               //If we already have a child carvpath, lets append.
               if (flags & TSK_FS_BLOCK_FLAG_SPARSE ) { 
                  carvpath_append_sparse(pc->child,size,0);
               } else {
                 carvpath_append(pc->child,daddr*(pc->fsblocksize),size,0,CARVPATH_OOR_SPARSE);
               }
            } else {
              //If we don't already have a child carvpath, create one.
              if (flags & TSK_FS_BLOCK_FLAG_SPARSE) {
                 pc->child=carvpath_new_sparse_entity(size,pc->parent); 
              } else {
                 pc->child=carvpath_derive(pc->parent,daddr*(pc->fsblocksize),size,CARVPATH_OOR_SPARSE);
              }
            }
            return TSK_WALK_CONT;
         }

  }
}
