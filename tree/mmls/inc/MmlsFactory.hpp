#ifndef MMLSFACTORY_HPP
#define MMLSFACTORY_HPP
#include "treegraph.hpp"
#include <libcarvpath.h>
#ifndef CARVPATH_ERR_MAXERRNO
#error Unable to build with old version of libcarvpath.Please upgrade libcarvpath to a current version.
#endif
namespace ocfa {
  namespace tsk {
     class MmlsFactory: public ocfa::treegraph::TreeGraphFactory,public ocfa::OcfaObject {
        public:
          MmlsFactory(std::map<std::string,ocfa::misc::Scalar> *attributes);
          ~MmlsFactory();
          std::string getCharset();
          void createTopNode(std::string path, ocfa::treegraph::TreeGraphNode **node,std::string timesourcebase);
        private:
          carvpath_library *mCpLib;
     }; 
  }
}
#endif
