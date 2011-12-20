#ifndef INCLUDED_FIVESFACES_H
#define INCLUDED_FIVESFACES_H

#include<facade/EvidenceFileAccessor.hpp>
#include<opencv/cv.h>
#include<opencv/highgui.h>
using namespace ocfa::facade;
namespace ocfa {
namespace module {
  class Fivesfaces : public EvidenceFileAccessor {
    private:
       // you might want to put state variables, handles to libstructures etc here ... 
       CvMemStorage* storage ;
       CvHaarClassifierCascade* cascade ;
       void Fives_DetectFaces( IplImage* image );
    public:
	Fivesfaces();
	virtual ~Fivesfaces();
	virtual void processEvidence();
  }; 
}

}

#endif
