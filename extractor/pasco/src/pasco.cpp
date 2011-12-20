#include<string>
#include"store/EvidenceStoreEntity.hpp"
#include"misc/OcfaLogger.hpp"
#include "pasco.hpp"
#include "libpasco.hpp"

using namespace ocfa::module;
using namespace ocfa::misc;

Pasco::Pasco(): EvidenceFileAccessor("pasco","core")
{
   // put thirdparty-library initializations here ...
}

Pasco::~Pasco()
{
   // clean up here ...
}

void Pasco::processEvidence()
{
  ocfa::store::EvidenceStoreEntity *evidence = fetchEvidenceStoreObject();
  std::string filename = evidence->getAsFilePath();
  getLogStream(LOG_DEBUG) << "Processing " << filename << endl;

  std::vector< URLRec * > urls;
  std::string ieversion;
  pascomain(filename, true, ieversion, urls);

  ArrayMetaValue *headers = new ArrayMetaValue(7);
  Scalar headerurl("url");
  Scalar headertype("urltype");
  Scalar headerascmodtime("ascmodtime");
  Scalar headerascaccesstime("ascaccesstime");
  Scalar headerfilename("filename");
  Scalar headerdirname("dirname");
  Scalar headerhttpheaders("httpheaders");
  
  headers->addMetaValue(headerurl);
  headers->addMetaValue(headertype);
  headers->addMetaValue(headerascmodtime);
  headers->addMetaValue(headerascaccesstime);
  headers->addMetaValue(headerfilename);
  headers->addMetaValue(headerdirname);
  headers->addMetaValue(headerhttpheaders);

  TableMetaValue urltable(&headers); 

  for (std::vector<URLRec *>::iterator itr = urls.begin(); itr != urls.end(); itr++)
  {
     ArrayMetaValue *row = new ArrayMetaValue(7);
     
     Scalar valurl((*itr)->Url);
     Scalar valtype((*itr)->Type);
     Scalar valascmodtime((*itr)->Ascmodtime);
     Scalar valascaccesstime((*itr)->Ascaccesstime);
     Scalar valfilename((*itr)->Filename);
     Scalar valdirname((*itr)->Dirname);
     Scalar valhttpheaders((*itr)->Httpheaders);
     row->addMetaValue(valurl);
     row->addMetaValue(valtype);  
     row->addMetaValue(valascmodtime);
     row->addMetaValue(valascaccesstime);
     row->addMetaValue(valfilename);
     row->addMetaValue(valdirname);
     row->addMetaValue(valhttpheaders);

     urltable.addRow(&row);

     // the thing *itr is pointing to is new-ed in pascomain.
     delete *itr;

  }

  setMeta("urllist", urltable);
  delete evidence;  
}

int main(int argc, char *argv[])
{
  Pasco *pPasco;
  try {
    pPasco = new Pasco();
    pPasco->run();
  } catch(...){
    return 1;
  }
  delete pPasco;
  return 0;
}
