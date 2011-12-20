#ifndef _CARVPATH_PARTITION_HPP
#define _CARVPATH_PARTITION_HPP
#include <treegraph.h>
class CarvPathPartition: public ocfa::treegraph::TreeGraphNode {
  public:
   CarvPathPartition(std::string metaFile);
   std::string getName();
   bool hasContent();
   bool isReadOnly();
   bool isReadable();
   void openStream();
   void closeStream();
   size_t streamRead(char *buf, size_t count);
   void takeMetaMap(std::map < std::string, misc::MetaValue * >**map);
   bool hasSubEntities();
   void resetSubEntityIterator();
   bool nextSubEntity();
   void getCurrentSubEntity(TreeGraphNode ** subent);
   std::string getCurrentSubEntityRelation();
   bool isRecursivelyUnlinkable();
   void unlinkOnDestruct();
   std::string getSoftLinkablePath();
   std::string getHardLinkablePath();
   misc::FragmentList *getStoreDataMask();
   ~CarvPathPartition();
  private:
   std::string mPartitionName;
   std::string mCarvPath;
   std::map < std::string, misc::MetaValue * > *mMetaMap;
};
#endif
