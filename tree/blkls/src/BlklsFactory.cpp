#include "../inc/BlklsFactory.hpp"
#include "../inc/BlklsTopNode.hpp"
#include "OcfaObject.hpp"
#include "misc.hpp"
namespace ocfa {
  namespace tsk {
        BlklsFactory::BlklsFactory(std::map<std::string,ocfa::misc::Scalar> *attributes):ocfa::OcfaObject("BlklsFactory","tsk"){
        }


        BlklsFactory::~BlklsFactory(){}


        std::string BlklsFactory::getCharset(){ return "UTF8";}


        void BlklsFactory::createTopNode(std::string path, ocfa::treegraph::TreeGraphNode **node,
                 std::string timesourcebase) {
            ocfaLog(ocfa::misc::LOG_DEBUG, "Creating BlklsTopNode");
            *node=new BlklsTopNode(path);
        }
  }
}
