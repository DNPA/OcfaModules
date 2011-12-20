#ifndef _PARTITION_TREE_GRAPH_HPP
#define _PARTITION_TREE_GRAPH_HPP
#include <treegraph.h>
class PartitionTreeGraph: public ocfa::treegraph::TreeGraphRootAndConnector {
  public:
   PartitionTreeGraph(std::string baseDir);
   std::string getCharSet();
   void getConnectedNode(string path, FsConnectedNode ** node);
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
   ~PartitionTreeGraph();
  private:
   std::string mBaseDir;
   size_t mSubEntCount;
};
#endif
