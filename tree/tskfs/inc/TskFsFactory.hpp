#ifndef TSKFSFACTORY_HPP
#define TSKFSFACTORY_HPP
#include "treegraph.hpp"
#include <libcarvpath.h>
#ifndef CARVPATH_ERR_MAXERRNO
#error Unable to build with old version of libcarvpath.Please upgrade libcarvpath to a current version.
#endif
namespace ocfa {
  namespace tsk {
     class TskFsFactory: public ocfa::treegraph::TreeGraphFactory,public ocfa::OcfaObject {
        public:
          TskFsFactory(std::map<std::string,ocfa::misc::Scalar> *attributes);
          ~TskFsFactory();
          std::string getCharset();
          void createTopNode(std::string path, ocfa::treegraph::TreeGraphNode **node,std::string timesoucebase);
        private:
          carvpath_library *mCpLib;
          bool mVerboseMeta; // Config indicates we should produce verbose meta data.
          bool mDoCpSlack;   // Config indicates we should produce fileslack nodes if they can be represented as carvpaths.
          bool mDoRawSlack;  //config indicates we should produce fileslack nodes if they can only be represented as raw data.
     }; 
  }
}
#endif
