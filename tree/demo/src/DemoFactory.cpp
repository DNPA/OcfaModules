#include "../inc/DemoFactory.hpp"
#include "../inc/DemoTopNode.hpp"
#include "misc.hpp"
namespace ocfa {
  namespace demo {
        DemoFactory::DemoFactory(std::map<std::string,ocfa::misc::Scalar> *attributes):ocfa::OcfaObject("DemoFactory","demo"){
           for (std::map<std::string,ocfa::misc::Scalar>::iterator attribute=attributes->begin();attribute != attributes->end();attribute++) {
               std::string name=attribute->first;
               std::string val=attribute->second.asUTF8();
               getLogStream(ocfa::misc::LOG_ERR) << "Attribute: " << name << "=" << val << std::endl;                     
           }
        }


        DemoFactory::~DemoFactory(){}


        std::string DemoFactory::getCharset(){ return "LATIN1";}


        void DemoFactory::createTopNode(std::string path, ocfa::treegraph::TreeGraphNode **node,
                 std::string timesoucebase) {
            getLogStream(ocfa::misc::LOG_ERR) << "Creating new DemoTopNode from path=" << path << std::endl; 
            *node=new DemoTopNode(path);
        }


  }
}
