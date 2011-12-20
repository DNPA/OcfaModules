#include "TskFsBaseDataAttribute.hpp"  

namespace ocfa {
  namespace tsk {
          TskFsBaseDataAttribute::TskFsBaseDataAttribute( TSK_FS_INFO *fs,const TSK_FS_ATTR *attribute,bool doslack,off_t realsize,bool verbosemeta):OcfaObject("TskFsBaseDataAttribute","tskfs"),mName("<UNNAMED>"),mVerboseMeta(verbosemeta),mFs(fs),mAttribute(attribute),mDoSlack(doslack),mRealSize(realsize) {
             //Create a new meta map.
             mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();
             //Set the nodetype to alternatestream, note that TskFsInode may replace this with its own if this is the primary stream data attribute.
             if (mRealSize == 0) {
                ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("emptyfile")));
             } else {
                ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("alternatestream")));
             }
             //Determine the attribute type.
             std::string ntfsattrtype="";
             switch (mAttribute->type) {
                case TSK_FS_ATTR_TYPE_DEFAULT:
                      ntfsattrtype="default";
                      break;
                case TSK_FS_ATTR_TYPE_NTFS_DATA:
                      ntfsattrtype="data";
                      break;
                case TSK_FS_ATTR_TYPE_NTFS_SI:
                      ntfsattrtype="si";
                      break;
                case TSK_FS_ATTR_TYPE_NTFS_ATTRLIST:
                      ntfsattrtype="attrlist";
                      break;
                case TSK_FS_ATTR_TYPE_NTFS_FNAME:
                      ntfsattrtype="fname";
                      break;
                case TSK_FS_ATTR_TYPE_NTFS_VVER:
		      ntfsattrtype="vver";
                      break;
                case TSK_FS_ATTR_TYPE_NTFS_SEC:
                      ntfsattrtype="sec";
                      break;
                case TSK_FS_ATTR_TYPE_NTFS_VNAME:
                      ntfsattrtype="vname";
                      break;
                case TSK_FS_ATTR_TYPE_NTFS_VINFO:
                      ntfsattrtype="vinfo";
                      break;
                case TSK_FS_ATTR_TYPE_NTFS_IDXROOT:
                      ntfsattrtype="idxroot";
                      break;
                case TSK_FS_ATTR_TYPE_NTFS_IDXALLOC:
                      ntfsattrtype="idxaloc";
                      break;
                case TSK_FS_ATTR_TYPE_NTFS_BITMAP:
                      ntfsattrtype="bitmap";
                      break;
                case TSK_FS_ATTR_TYPE_NTFS_REPARSE:
                      ntfsattrtype="reparse";
                      break;
                case TSK_FS_ATTR_TYPE_NTFS_EAINFO:
                      ntfsattrtype="eainfo";
                      break;
                case TSK_FS_ATTR_TYPE_NTFS_EA:
                      ntfsattrtype="ea";
                      break;
                case TSK_FS_ATTR_TYPE_NTFS_PROP:
                      ntfsattrtype="prop";
                      break;
                case TSK_FS_ATTR_TYPE_NTFS_LOG:
                      ntfsattrtype="log";
                      break;
                default:
                      ntfsattrtype="undefined";
             }
             ocfa::misc::metautil::addMetaToMap(mMeta,"ntfs-attribute-type", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(ntfsattrtype)));       
             if (mAttribute->name  && (mAttribute->name != "") && (std::string(mAttribute->name) != "N/A")) {
                mName= std::string("<attribute::") + ntfsattrtype + "::" + mAttribute->name + ">";
             } else {
                mName= std::string("<attribute::") + ntfsattrtype + "::unnamed>";
             }
             //Add meta data about encryption.
             if (mAttribute->flags & TSK_FS_ATTR_ENC) {
                 ocfa::misc::metautil::addMetaToMap(mMeta,"encrypted", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(1)));
                 ocfa::misc::metautil::addMetaToMap(mMeta,"specialmarker", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("encrypted-file")));
             } else {
                 ocfa::misc::metautil::addMetaToMap(mMeta,"encrypted", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(0)));
             }
             //Add meta data about compression.
             if (mVerboseMeta) {
               if (mAttribute->flags & TSK_FS_ATTR_COMP) {
                 ocfa::misc::metautil::addMetaToMap(mMeta,"compressed", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(1)));
               } else {
                 ocfa::misc::metautil::addMetaToMap(mMeta,"compressed", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(0)));
               }
             }
             //Add meta data about size.
             ocfa::misc::metautil::addMetaToMap(mMeta,"size", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(mRealSize)));
          }

          TskFsBaseDataAttribute::~TskFsBaseDataAttribute(){
             //Cleanup the meta map if it has not been fetched.
             if (mMeta) {
               map < string, misc::MetaValue * >::const_iterator p;
               for (p = mMeta->begin(); p != mMeta->end(); ++p) {
                 delete p->second;
               }
               delete mMeta;
               mMeta=0;
             }
          }
          std::string TskFsBaseDataAttribute::getName(){
              return mName;
          }
          void TskFsBaseDataAttribute::takeMetaMap(std::map < std::string, misc::MetaValue * >**map){
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
          off_t TskFsBaseDataAttribute::getSize(){
               return mRealSize;
          }
 
          bool TskFsBaseDataAttribute::hasContent() {
               if (mRealSize > 0) {
                   return true;
               }
               return false;
          }
  }
}
