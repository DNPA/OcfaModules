#ifndef SCALPELFACTORY_HPP
#define SCALPELFACTORY_HPP
#include "treegraph.hpp"
#include <libcarvpath.h>
#ifndef CARVPATH_ERR_MAXERRNO
#error Unable to build with old version of libcarvpath.Please upgrade libcarvpath to a current version.
#endif
namespace ocfa {
  namespace scalpel {
     class ScalpelCpFactory: public ocfa::treegraph::TreeGraphFactory,public ocfa::OcfaObject {
        public:
          ScalpelCpFactory(std::map<std::string,ocfa::misc::Scalar> *attributes);
          ~ScalpelCpFactory();
          std::string getCharset();
          void createTopNode(std::string path, ocfa::treegraph::TreeGraphNode **node,std::string timesourcebase);
        private:
          carvpath_library *mCpLib;
          std::string mWorkDir;
     }; 
  }
}
#endif
