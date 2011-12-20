#include "../inc/WebTreeFactory.hpp"
#include "../inc/WebTreeNode.hpp"
#include "misc.hpp"
#include "errno.h"
namespace ocfa {
  namespace webtree {
        WebTreeFactory::WebTreeFactory(std::map<std::string,ocfa::misc::Scalar> *attributes):ocfa::OcfaObject("WebTreeFactory","webtree"){
        }
        WebTreeFactory::~WebTreeFactory(){
        }
        std::string WebTreeFactory::getCharset(){ return "UTF8";}
        void WebTreeFactory::createTopNode(std::string url, ocfa::treegraph::TreeGraphNode **node,
                 std::string timesourcebase) {
            *node=new WebTreeNode(url,timesourcebase,0);
        }
  }
}
