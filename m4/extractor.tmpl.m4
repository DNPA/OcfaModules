define(`EXTRACTORHPP', 
`#ifndef INCLUDED_'$2`_H
#define INCLUDED_'$2`_H

#include<facade/EvidenceFileAccessor.hpp>

using namespace ocfa::facade;
namespace ocfa {
namespace module {
  class $3 : public EvidenceFileAccessor {
    private:
       // you might want to put state variables, handles to libstructures etc here ... 
    public:
	$3();
	virtual ~$3();
	virtual void processEvidence();
  }; 
}

}

#endif
')dnl
dnl
define(`EXTRACTORCPP',
`#include<string>
#include"store/EvidenceStoreEntity.hpp"
#include"misc/OcfaLogger.hpp"
#include "$1.hpp"

using namespace ocfa::module;
using namespace ocfa::misc;

$3::$3(): EvidenceFileAccessor("$1","core")
{
   // put thirdparty-library initializations here ...
}

$3::~$3()
{
   // clean up here ...
}

void $3::processEvidence()
{
   ocfa::store::EvidenceStoreEntity *evidence = fetchEvidenceStoreObject();
   std::string filename = evidence->getAsFilePath();
   getLogStream(LOG_DEBUG) << "Processing " << filename << endl;

   // do whatever you want to do with the evidence

   // Store metadata about the evidence, e.g.
   // setMeta("MetadataName", MetadataValue);

}

int main(int argc, char *argv[])
{
  $3 *p$3;
  try {
    p$3 = new $3();
    p$3->run();
  } catch(...){
    return 1;
  }
  delete p$3;
  return 0;
}'
)dnl
