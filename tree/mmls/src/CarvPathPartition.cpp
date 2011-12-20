#include "CarvPathPartition.hpp"
#include <misc.hpp>
extern "C" {
#include <libcarvpath.h>
}
#include <boost/lexical_cast.hpp>
namespace ocfa {
  namespace tsk {
     CarvPathPartition::CarvPathPartition(carvpath_entity *cptopent, off_t offset, off_t len, std::string description, bool allocated, bool meta,TSK_PNUM_T pnum):OcfaObject("CarvPathPartition","mmls"),mSize(len) {
         mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();
         ocfa::misc::metautil::addMetaToMap(mMeta,"offset", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar((long long) offset)));
         ocfa::misc::metautil::addMetaToMap(mMeta,"partitioninfo", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(description)));
         ocfa::misc::metautil::addMetaToMap(mMeta,"size", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(len)));
         ocfa::misc::metautil::addMetaToMap(mMeta,"datamode", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("carvpath")));
         if (allocated) {
           ocfa::misc::metautil::addMetaToMap(mMeta,"allocated", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(1)));
           ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("diskpartition")));
         } else {
           ocfa::misc::metautil::addMetaToMap(mMeta,"allocated", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(0)));
           if (meta) {
              ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("vtmeta")));
           } else {
               ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("unallocateddiskspace")));
           }
         }
         mName = boost::lexical_cast<std::string>(pnum) + "-" + description;
         carvpath_entity *childcp=carvpath_derive(cptopent,offset,len,CARVPATH_OOR_TRUNCATE);
         std::string carvpath=std::string(carvpath_get_as_path(childcp)) + ".crv";
         carvpath_free(childcp,0);
         if (!(meta)) {
            carvpath_entity *childcp=carvpath_derive(cptopent,offset,len,CARVPATH_OOR_TRUNCATE);
            mPartitionCarvPath = std::string(carvpath_get_as_path(childcp)) + ".crv";
            carvpath_free(childcp,0);                                    
         }
         mStartTime=time(NULL);
     }
     CarvPathPartition::~CarvPathPartition() {
         if (mStartTime){
           time_t wasted=time(NULL)-mStartTime;
           if (wasted > 1) {
             getLogStream(ocfa::misc::LOG_NOTICE) << "Wasted "<< wasted << " seconds calculating a digest that will never be usefull" << std::endl;
           }
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
     void CarvPathPartition::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
         *map=mMeta;
         mMeta=0;
     }
  }
}
