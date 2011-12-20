#include "../inc/MmlsFactory.hpp"
#include "../inc/MmlsTopNode.hpp"
#include "misc.hpp"
#include "errno.h"
namespace ocfa {
  namespace tsk {
        MmlsFactory::MmlsFactory(std::map<std::string,ocfa::misc::Scalar> *attributes):ocfa::OcfaObject("MmlsFactory","tsk"),mCpLib(0){
           mCpLib=carvpath_init(1,0);
           if (mCpLib == 0) {
              getLogStream(ocfa::misc::LOG_ERR) << "Problem initializing libcarvpath: " << carvpath_error_as_string(errno) << std::endl;                
           }
        }


        MmlsFactory::~MmlsFactory(){
           if (mCpLib) {
              carvpath_finish(mCpLib);  
           }         
        }


        std::string MmlsFactory::getCharset(){ return "UTF8";}


        void MmlsFactory::createTopNode(std::string path, ocfa::treegraph::TreeGraphNode **node,
                 std::string timesourcebase) {
            *node=new MmlsTopNode(path,mCpLib);
        }
  }
}
