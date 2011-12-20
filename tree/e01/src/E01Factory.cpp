#include "../inc/E01Factory.hpp"
#include "../inc/E01Node.hpp"
#include "misc.hpp"
namespace ocfa {
  namespace e01 {
        E01Factory::E01Factory(std::map<std::string,ocfa::misc::Scalar> *attributes):ocfa::OcfaObject("E01Factory","demo"){
        }


        E01Factory::~E01Factory(){}


        std::string E01Factory::getCharset(){ return "UTF8";}


        void E01Factory::createTopNode(std::string path, ocfa::treegraph::TreeGraphNode **node,
                 std::string timesoucebase) {
            *node=new E01Node(path);
        }


  }
}
