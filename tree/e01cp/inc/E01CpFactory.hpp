#ifndef E01CPFACTORY_HPP
#define E01CPFACTORY_HPP
#include "treegraph.hpp"
namespace ocfa {
  namespace e01cp {
     class E01CpFactory: public ocfa::treegraph::TreeGraphFactory,public ocfa::OcfaObject {
        public:
          E01CpFactory(std::map<std::string,ocfa::misc::Scalar> *attributes);
          ~E01CpFactory();
          std::string getCharset();
          void createTopNode(std::string path, ocfa::treegraph::TreeGraphNode **node,std::string timesoucebase);
     }; 
  }
}
#endif
