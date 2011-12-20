#include "TskFsStreamDataAttribute.hpp"  
#include "TskFsFileSlack.hpp"

namespace ocfa {
  namespace tsk {
          TskFsStreamDataAttribute::TskFsStreamDataAttribute( TSK_FS_INFO *fs,const TSK_FS_ATTR *attribute,bool doslack,off_t realsize,bool verbosemeta):TskFsBaseDataAttribute(fs,attribute,doslack,realsize,verbosemeta) {
             updateTypeName("TskFsStreamDataAttribute");
             ocfa::misc::metautil::addMetaToMap(mMeta,"resident", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(0)));
          }
          TskFsStreamDataAttribute::~TskFsStreamDataAttribute(){
          }
          void TskFsStreamDataAttribute::streamToOutput(ocfa::misc::AbstractWriteFacet &writefacet){
              tsk_fs_attr_walk(mAttribute,TSK_FS_FILE_WALK_FLAG_NOID,streamdata_action,&writefacet); //Hand the write facet directly to the sleuthkit attribute walk function.
          }
          void TskFsStreamDataAttribute::getCurrentSubEntity(TreeGraphNode ** subent){
               *subent = new TskFsFileSlack(mFs,mAttribute,mRealSize);
               return;
          }
          bool TskFsStreamDataAttribute::hasSubEntities(){
               off_t blocksize=mFs->block_size;
               if (mDoSlack && (mRealSize % blocksize)) { 
                 return true;
               }
               return false;
          }

         TSK_WALK_RET_ENUM TskFsStreamDataAttribute::streamdata_action(TSK_FS_FILE *file, TSK_OFF_T offset, TSK_DADDR_T daddr, char *buf , size_t size, TSK_FS_BLOCK_FLAG_ENUM flags, void *ptr)
         {
            ocfa::misc::AbstractWriteFacet &write_to_store_entity = *(( ocfa::misc::AbstractWriteFacet *) ptr); //Cast the generic state pointer to the original write facet.
            write_to_store_entity(buf,size); //Forward the data to the write facet.
            return TSK_WALK_CONT;
         }

  }
}
