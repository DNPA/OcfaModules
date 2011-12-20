#ifndef DEMOFACTORY_HPP
#define DEMOFACTORY_HPP
#include "treegraph.hpp"
namespace ocfa {
  namespace demo {
     class DemoFactory: public ocfa::treegraph::TreeGraphFactory,public ocfa::OcfaObject {
        public:
          DemoFactory(std::map<std::string,ocfa::misc::Scalar> *attributes);
          ~DemoFactory();
          std::string getCharset();
          void createTopNode(std::string path, ocfa::treegraph::TreeGraphNode **node,std::string timesoucebase);
     }; 
  }
}
#endif
