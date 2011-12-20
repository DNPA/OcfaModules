#include "../inc/TskFsFactory.hpp"
#include "../inc/TskFsTopDir.hpp"
#include "misc.hpp"
#include <errno.h>
namespace ocfa {
  namespace tsk {
        TskFsFactory::TskFsFactory(std::map<std::string,ocfa::misc::Scalar> *attributes):ocfa::OcfaObject("MmlsFactory","tsk"),mDoCpSlack(true),mDoRawSlack(true),mVerboseMeta(true){
           //Fetch some config info from the ocfa config file.
           mCpLib=carvpath_init(1,0);
           if (mCpLib == 0) {
               getLogStream(ocfa::misc::LOG_ERR) << "Problem initializing libcarvpath: " << carvpath_error_as_string(errno) << std::endl;
           }
           std::string slackhandling= misc::OcfaConfig::Instance()->getValue("handleslack");
           if ((slackhandling == "no")|| (slackhandling == "false")) {
               mDoCpSlack = false;
               mDoRawSlack = false;
           } else if ((slackhandling == "yes")||(slackhandling == "true")) {
               mDoCpSlack = true;
               mDoRawSlack = true;
           } else if (slackhandling == "cponly") {
               mDoCpSlack = true;
               mDoRawSlack = false;
           } else if (slackhandling == "rawonly") {
               mDoCpSlack = false;
               mDoRawSlack = true;
           }
           if ((misc::OcfaConfig::Instance()->getValue("verbosemeta") == "no")||(misc::OcfaConfig::Instance()->getValue("verbosemeta") == "false")) {
              mVerboseMeta=false;
           }
        }


        TskFsFactory::~TskFsFactory(){
          if (mCpLib) {
             carvpath_finish(mCpLib);
          }
        }


        std::string TskFsFactory::getCharset(){ return "UTF8";}


        void TskFsFactory::createTopNode(std::string path, ocfa::treegraph::TreeGraphNode **node,
                 std::string timesoucebase) {
            *node=new TskFsTopDir(path,timesoucebase,mDoRawSlack,mDoCpSlack,mVerboseMeta,mCpLib);
        }
  }
}
