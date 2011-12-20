#include "../inc/E01CpFactory.hpp"
#include "../inc/E01CpNode.hpp"
#include "misc.hpp"
namespace ocfa {
  namespace e01cp {
        E01CpFactory::E01CpFactory(std::map<std::string,ocfa::misc::Scalar> *attributes):ocfa::OcfaObject("E01CpFactory","demo"){
        }


        E01CpFactory::~E01CpFactory(){}


        std::string E01CpFactory::getCharset(){ return "UTF8";}


        void E01CpFactory::createTopNode(std::string path, ocfa::treegraph::TreeGraphNode **node,
                 std::string timesoucebase) {
            *node=new E01CpNode(path);
        }


  }
}
