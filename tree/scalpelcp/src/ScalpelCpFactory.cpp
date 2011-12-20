#include "../inc/ScalpelCpFactory.hpp"
#include "../inc/ScalpelCpTopNode.hpp"
#include "misc.hpp"
#include "errno.h"
namespace ocfa {
  namespace scalpel {
        ScalpelCpFactory::ScalpelCpFactory(std::map<std::string,ocfa::misc::Scalar> *attributes):ocfa::OcfaObject("ScalpelCpFactory","scalpel"),mCpLib(0){
           mCpLib=carvpath_init(1,0);
           mWorkDir=(*attributes)["workdir"].asUTF8();           
           if (mCpLib == 0) {
              getLogStream(ocfa::misc::LOG_ERR) << "Problem initializing libcarvpath: " << carvpath_error_as_string(errno) << std::endl;                
           }
        }


        ScalpelCpFactory::~ScalpelCpFactory(){
           if (mCpLib) {
              carvpath_finish(mCpLib);  
           }         
        }


        std::string ScalpelCpFactory::getCharset(){ return "UTF8";}


        void ScalpelCpFactory::createTopNode(std::string path, ocfa::treegraph::TreeGraphNode **node,
                 std::string timesourcebase) {
            *node=new ScalpelCpTopNode(path,mCpLib,mWorkDir);
        }
  }
}
