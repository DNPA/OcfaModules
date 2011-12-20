#ifndef SLFACTORY_HPP
#define SLFACTORY_HPP
#include "treegraph.hpp"
namespace ocfa {
  namespace sl {
     class SoftLinkFactory: public ocfa::treegraph::TreeGraphFactory,public ocfa::OcfaObject {
        public:
          SoftLinkFactory(std::map<std::string,ocfa::misc::Scalar> *attributes);
          ~SoftLinkFactory();
          std::string getCharset();
          void createTopNode(std::string path, ocfa::treegraph::TreeGraphNode **node,std::string timesoucebase);
     }; 
  }
}
#endif
