#ifndef RAWCPFACTORY_HPP
#define RAWCPFACTORY_HPP
#include "treegraph.hpp"
namespace ocfa {
  namespace rawcp {
     class RawCpFactory: public ocfa::treegraph::TreeGraphFactory,public ocfa::OcfaObject {
        public:
          RawCpFactory(std::map<std::string,ocfa::misc::Scalar> *attributes);
          ~RawCpFactory();
          std::string getCharset();
          void createTopNode(std::string path, ocfa::treegraph::TreeGraphNode **node,std::string timesoucebase);
     }; 
  }
}
#endif
