#include "TcpFlowTreeWalker.hpp"

using namespace ocfa::misc;
using namespace ocfa::evidence;
using namespace ocfa::module;
using namespace ocfa::fs;
using namespace std;

TcpFlowTreeWalker::TcpFlowTreeWalker(OcfaModule *inModule, ModuleInstance *inRouter,
				     string inInitPath, bool inShouldDelete, string charset)
  : EvidenceTreeWalker(inModule, inRouter, inInitPath, inShouldDelete, charset){  
}


Evidence *TcpFlowTreeWalker::createEvidenceFromFsEntity(Evidence *inParent, ocfa::treegraph::TreeGraphNode *inFsEntity,
							const string inRelation, Scalar *inEvidenceName, 
							vector<CoParent> *coParents){
							
  Evidence *newEvidence = 
    EvidenceTreeWalker::createEvidenceFromFsEntity(inParent, inFsEntity, inRelation,  
						   inEvidenceName, coParents);
  
  if (inFsEntity->hasContent()){

    string name = inFsEntity->getName();
    getLogStream(LOG_DEBUG) << "name is " << name << endl;

    string source = name.substr(name.find("_") + 1, name.find("-"));
    getLogStream(LOG_DEBUG) << "source is " << source << endl;
    string dest = name.substr(name.find("-") + 1);
    getLogStream(LOG_DEBUG) << "dest is " << dest << endl;
    //getLogStream(LOG_DEBUG) << " last index is " <<  source.find_last_of(".") << endl;
    string sourceip = source.substr(0, source.find_last_of("."));
    getLogStream(LOG_DEBUG) <<  " sourceip is " << sourceip << endl;
    string sourceport = source.substr(source.find_last_of(".") + 1);
    getLogStream(LOG_DEBUG) << " sourceport is " << sourceport << endl;
    string destip = dest.substr(0, dest.find_last_of("."));
    getLogStream(LOG_DEBUG) << "destip is " << destip << endl;
    string destport = dest.substr(dest.find_last_of(".") + 1);
    getLogStream(LOG_DEBUG) << " destport is " << destport << endl;
    ActiveJob *currentJob = newEvidence->getActiveJob();
    if (currentJob == 0){

      throw OcfaException("no currentJob on new evidence for "+ inFsEntity->getName(), this);
    }
    addMetaString(currentJob, "sourceip", sourceip);
    addMetaString(currentJob, "sourceport", sourceport);
    addMetaString(currentJob, "destip", destip);
    addMetaString(currentJob, "destport", destport);
  }
  return newEvidence;
}



void TcpFlowTreeWalker::addMetaString(ActiveJob *inActiveJob, string inMetaName,
				      string inMetaValue){

  Scalar theScalar(inMetaValue);
  ScalarMetaValue theMetaValue(theScalar);
  inActiveJob->setMeta(inMetaName, &theMetaValue);
}
