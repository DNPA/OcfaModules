#include <tcpflow.hpp>
#include<tcpflow.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdexcept>
#include <vector>
#include <errno.h>
#include<string.h>
#include "TcpFlowTreeWalker.hpp"
using namespace ocfa;
using namespace evidence;
using namespace ocfa::misc;
using namespace ocfa::module;
tcpflowDissector::tcpflowDissector()
  : ocfa::facade::EvidenceDeriveAccessor("tcpflow","default") {

  EvidenceTreeWalker *walker = new TcpFlowTreeWalker(this, 0, getWorkDir(), true, "AUTO");
  setTreeWalker(&walker);
} 

void tcpflowDissector::processEvidence()
{
  char cline[1024];
 
  string tmpwd = getWorkDir() + string("/output");
  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject();
  if (evidence == 0)
    throw OcfaException("Evidence without evidence store", this);
  
  entrypoint(evidence->getAsFilePath().c_str(), getWorkDir().c_str());
  delete evidence;
  Evidence *derived = derive("/output", Scalar("content"));
  submitEvidence(derived);
  delete derived;
  return;
  
}

int main(int , char *[])
{
  //Ocfa::setDebug();
  tcpflowDissector *tcpflow=0;
  try {
    tcpflow = new tcpflowDissector();
  } catch(OcfaException ex) {
    ex.logWhat();
    return 2;
  }
  try {
    tcpflow->run();
  } catch(OcfaException ex) {
    tcpflow->getLogStream(LOG_CRIT) << "Ocfa Exeption cougth: " << ex.what() << "\n";
  } catch(...) {
    tcpflow->getLogStream(LOG_CRIT) << "Non Ocfa Exeption cougth\n";
  }
  tcpflow->PrintObjCount();
  delete tcpflow;
  return 1;
}
