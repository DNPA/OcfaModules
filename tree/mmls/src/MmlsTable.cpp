#include "MmlsTable.hpp"
#include "CarvPathPartition.hpp"
#include "StreamPartition.hpp"
#include <boost/lexical_cast.hpp>

namespace ocfa {
  namespace tsk {
     MmlsTable::MmlsTable(carvpath_entity *cptopent,TSK_VS_INFO *vshandle,int table_num,std::string description,off_t start,off_t length,TSK_PNUM_T pnum):
                                                                                                                                 OcfaObject("MmlsTable","mmls"),
                                                                                                                                 mVsHandle(vshandle),
                                                                                                                                 mTableNum(table_num),
                                                                                                                                 mActivePartitionIndex(0),
                                                                                                                                 mFirstPartitionIndex(1),
                                                                                                                                 mImageHandle(vshandle->img_info),
                                                                                                                                 mCarvpathTop(cptopent) 
     {
         //Make sure that names are unique by prefixing the partition number as used internaly by libstk3 to the description text.
         mName =  boost::lexical_cast<std::string>(pnum) + "-" + description;
         mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();
         //The first partition is always the table itself, so to find the first real child partition we set the index to 0
         // and invoke nextSubEntity().
         mActivePartitionIndex=0;
         nextSubEntity();
         //Than we safe the first child partition index by setting mFirstPartitionIndex.
         mFirstPartitionIndex=mActivePartitionIndex;
         //Add some usefull meta data.
         if (mCarvpathTop) {
            ocfa::misc::metautil::addMetaToMap(mMeta,"datamode", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("carvpath")));
         } else {
            ocfa::misc::metautil::addMetaToMap(mMeta,"datamode", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("stream")));
         }
         ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("pseudodir")));
         ocfa::misc::metautil::addMetaToMap(mMeta,"metaoffset", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(start)));
         ocfa::misc::metautil::addMetaToMap(mMeta,"metasize", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(length)));
         ocfa::misc::metautil::addMetaToMap(mMeta,"tablenum", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(table_num)));
     };
     MmlsTable::~MmlsTable() {
         if (mMeta) {
             map < string, misc::MetaValue * >::const_iterator p;
            for (p = mMeta->begin(); p != mMeta->end(); ++p) {
               delete p->second;
            }
            delete mMeta;
            mMeta=0;
         }        
     };

     bool MmlsTable::hasSubEntities() {
       return (mFirstPartitionIndex < mVsHandle->part_count); 
     };

     void MmlsTable::resetSubEntityIterator() {
         mActivePartitionIndex=mFirstPartitionIndex;
     };

     bool MmlsTable::nextSubEntity() {
        //Save the original mActivePartitionIndex, we may need it if there is no next one.
        TSK_PNUM_T oldIndex=mActivePartitionIndex; 
        //Search the next partition within this table.
        while (mActivePartitionIndex < (mVsHandle->part_count -1)) {
            mActivePartitionIndex++;
            const TSK_VS_PART_INFO *partition=tsk_vs_part_get(mVsHandle,mActivePartitionIndex);
            int8_t tableno=partition->table_num; 
            if (tableno > 0 ) {
              //Partition tables are bound to their own table level what leaves them unconnected
              //to the level one below. We thus decrement the table number of extended tables
              //by one to fix the tree.
              std::string description= partition->desc;
              size_t i = description.find("(#");
              if ( (i != std::string::npos ) &&
                  ((i+2) < description.length())
              )
              {
                  int table_num = atoi(description.substr(i + 2, description.length() - 2).c_str());
                  tableno = table_num -1;
              }
            }           
            if (tableno == mTableNum) {
              return true;
            }
        }
        //We didn't find the next one, lets restore the old value.
        mActivePartitionIndex=oldIndex;
        return false;
     };

     void MmlsTable::getCurrentSubEntity(TreeGraphNode ** subent) {
         if (*subent) {
            throw ocfa::misc::OcfaException("MmlsTopNode::getCurrentSubEntity called with subent not cleared");
         }
         const TSK_VS_PART_INFO *partition=tsk_vs_part_get(mVsHandle,mActivePartitionIndex);
         off_t start  = partition->start * mVsHandle->block_size;
         off_t length = partition->len * mVsHandle->block_size;
         std::string description= partition->desc ;
         bool allocated=(partition->flags & TSK_VS_PART_FLAG_ALLOC);
         bool meta=(partition->flags & TSK_VS_PART_FLAG_META);
         //Avoid copying out extended partitions not marked as meta data partitions by setting the meta flag. 
         //FIXME (THIS IS A NASTY HACK!)
         size_t i = description.find("Extended");
         if (i != std::string::npos ) {
            meta=true;
         } 
         i = description.find("(#");
         if ( (i != std::string::npos ) && 
             ((i+2) < description.length())
            ) 
         { 
            //A child node can be an other extended table.
            int table_num = atoi(description.substr(i + 2, description.length() - 2).c_str());
            *subent=new MmlsTable(mCarvpathTop,mVsHandle,table_num,description,start,length,mActivePartitionIndex);
         } else {
            //Or a leave node.
            if (mCarvpathTop) {
                //Represented either by a carvpath based node interface.
                *subent=new CarvPathPartition(mCarvpathTop,start,length,description,allocated,meta,mActivePartitionIndex);
            } else {
                //Or a stream based node interface.
                *subent=new StreamPartition(mImageHandle,start,length,description,allocated,meta,mActivePartitionIndex);
            }
         }
         return;
     };
     void MmlsTable::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
         *map=mMeta;
         mMeta=0;
     }
     
  }
}
