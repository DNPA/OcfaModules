#ifndef BLKLSFACTORY_HPP
#define BLKLSFACTORY_HPP
#include "treegraph.hpp"
namespace ocfa {
  namespace tsk {
     class BlklsFactory: public ocfa::treegraph::TreeGraphFactory,public ocfa::OcfaObject {
        public:
          BlklsFactory(std::map<std::string,ocfa::misc::Scalar> *attributes);
          ~BlklsFactory();
          std::string getCharset();
          void createTopNode(std::string path, ocfa::treegraph::TreeGraphNode **node,std::string timesourcebase);
     }; 
  }
}
#endif
