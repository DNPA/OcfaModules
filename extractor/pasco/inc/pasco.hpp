#ifndef INCLUDED_PASCO_H
#define INCLUDED_PASCO_H

#include<facade/EvidenceFileAccessor.hpp>

using namespace ocfa::facade;
namespace ocfa {
namespace module {
  class Pasco : public EvidenceFileAccessor {
    private:
       // you might want to put state variables, handles to libstructures etc here ... 
    public:
	Pasco();
	virtual ~Pasco();
	virtual void processEvidence();
  }; 
}

}

#endif
