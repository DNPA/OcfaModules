#ifndef E01FACTORY_HPP
#define E01FACTORY_HPP
#include "treegraph.hpp"
namespace ocfa {
  namespace e01 {
     class E01Factory: public ocfa::treegraph::TreeGraphFactory,public ocfa::OcfaObject {
        public:
          E01Factory(std::map<std::string,ocfa::misc::Scalar> *attributes);
          ~E01Factory();
          std::string getCharset();
          void createTopNode(std::string path, ocfa::treegraph::TreeGraphNode **node,std::string timesoucebase);
     }; 
  }
}
#endif
