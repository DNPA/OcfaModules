#include "../inc/RawCpFactory.hpp"
#include "../inc/RawCpNode.hpp"
#include "misc.hpp"
namespace ocfa {
  namespace rawcp {
        RawCpFactory::RawCpFactory(std::map<std::string,ocfa::misc::Scalar> *attributes):ocfa::OcfaObject("RawCpFactory","demo"){
        }


        RawCpFactory::~RawCpFactory(){}


        std::string RawCpFactory::getCharset(){ return "UTF8";}


        void RawCpFactory::createTopNode(std::string path, ocfa::treegraph::TreeGraphNode **node,
                 std::string timesoucebase) {
            *node=new RawCpNode(path);
        }


  }
}
