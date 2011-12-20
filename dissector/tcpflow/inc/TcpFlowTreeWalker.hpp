#include "module/EvidenceTreeWalker.hpp"
#include "fs.hpp"

class TcpFlowTreeWalker : public ocfa::module::EvidenceTreeWalker {

public:

  TcpFlowTreeWalker(ocfa::module::OcfaModule *inModule, ocfa::misc::ModuleInstance *inRouter,
			    std::string inInitPath, bool inShouldDelete, std::string charset="AUTO");


  /**
   *
   */
  virtual ocfa::evidence::Evidence *createEvidenceFromFsEntity(ocfa::evidence::Evidence *inParent,
							       ocfa::treegraph::TreeGraphNode *inFsEntity, 
							       const std::string inRelation, 
							       ocfa::misc::Scalar *inEvidenceName = 0,
							       std::vector<ocfa::evidence::CoParent> *coParents = 0);
protected:
  virtual void addMetaString(ocfa::evidence::ActiveJob *inActiveJob, std::string metaName, 
			     std::string inMetaValue);
};
