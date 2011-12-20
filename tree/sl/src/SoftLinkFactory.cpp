#include "../inc/SoftLinkFactory.hpp"
#include "../inc/SoftLinkNode.hpp"
#include "misc.hpp"
namespace ocfa {
  namespace sl {
        SoftLinkFactory::SoftLinkFactory(std::map<std::string,ocfa::misc::Scalar> *attributes):ocfa::OcfaObject("SoftLinkFactory","sl"){
        }


        SoftLinkFactory::~SoftLinkFactory(){}


        std::string SoftLinkFactory::getCharset(){ return "UTF8";}


        void SoftLinkFactory::createTopNode(std::string path, ocfa::treegraph::TreeGraphNode **node,
                 std::string timesoucebase) {
            *node=new SoftLinkNode(path);
        }


  }
}
