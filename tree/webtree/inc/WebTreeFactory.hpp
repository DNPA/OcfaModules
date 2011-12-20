#ifndef WEBTREEFACTORY_HPP
#define WEBTREEFACTORY_HPP
#include "treegraph.hpp"
namespace ocfa {
  namespace webtree {
     class WebTreeFactory: public ocfa::treegraph::TreeGraphFactory,public ocfa::OcfaObject {
        public:
          WebTreeFactory(std::map<std::string,ocfa::misc::Scalar> *attributes);
          ~WebTreeFactory();
          std::string getCharset();
          void createTopNode(std::string path, ocfa::treegraph::TreeGraphNode **node,std::string timesourcebase);
        private:
     }; 
  }
}
#endif
