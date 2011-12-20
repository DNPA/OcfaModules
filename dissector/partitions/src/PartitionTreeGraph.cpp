#include "../inc/PartitionTreeGraph.hpp"

   PartitionTreeGraph::PartitionTreeGraph(std::string baseDir):mBaseDir(baseDir),mSubEntCount(0){
      //FIXME: determine the entity count.
   }
   std::string PartitionTreeGraph::getCharSet(){
      return "UTF8";
   }
   void PartitionTreeGraph::getConnectedNode(string path, FsConnectedNode ** node){
      if (*node != 0) {
              throw ocfa::misc::OcfaException("getCurrentSubEntity called with non NULL target.",this);
      }
      *node=new Partition(mBaseDir,path);
      return;
   }
   std::string PartitionTreeGraph::getName(){
      return "mmls-output";
   }
   bool PartitionTreeGraph::hasContent(){ return false;}
   bool PartitionTreeGraph::isReadOnly(){ return false;}
   bool PartitionTreeGraph::isReadable(){ return true;}
   void PartitionTreeGraph::openStream() { throw ocfa::misc::OcfaException("No stream for directory node",this);}
   void PartitionTreeGraph::closeStream() { throw ocfa::misc::OcfaException("No stream for directory node",this);}
   size_t PartitionTreeGraph::streamRead(char *, size_t) { throw ocfa::misc::OcfaException("No stream for directory node",this);}
   void PartitionTreeGraph::takeMetaMap(std::map < std::string, misc::MetaValue * >**map){
      std::map < std::string, misc::MetaValue * > *rvalMap= new std::map < std::string, misc::MetaValue * >();
      //FIXME: fill map with usefull metadata.
   }
   bool PartitionTreeGraph::hasSubEntities() {
      return (mSubEntCount > 0);
   }
   void PartitionTreeGraph::resetSubEntityIterator() {
     return mSubEntNo=1;
   }
   bool PartitionTreeGraph::nextSubEntity() {
     if (mSubEntNo < mSubEntCount) {
        mSubEntNo++;
        return true;
     } else {
        return false;
     }
   }
   void PartitionTreeGraph::getCurrentSubEntity(TreeGraphNode ** subent){
      if (*subent != 0) {
        throw ocfa::misc::OcfaException("getCurrentSubEntity called with non NULL target.",this);
      }
      *subent=new Partition(mBaseDir,mSubEntNo);
      return;
   }
   std::string PartitionTreeGraph::getCurrentSubEntityRelation(){
      return "partition";
   }
   bool PartitionTreeGraph::isRecursivelyUnlinkable(){
      return true;
   }
   void PartitionTreeGraph::unlinkOnDestruct(){};
   std::string PartitionTreeGraph::getSoftLinkablePath(){
      throw ocfa::misc::OcfaException("No soft linkable path for directory.",this);}
   }
   std::string PartitionTreeGraph::getHardLinkablePath(){
      throw ocfa::misc::OcfaException("No hard linkable path for directory.",this);}
   }
   misc::FragmentList *PartitionTreeGraph::getStoreDataMask(){
       return 0;
   }
   PartitionTreeGraph::~PartitionTreeGraph() {
     ;
   }
};
#endif
