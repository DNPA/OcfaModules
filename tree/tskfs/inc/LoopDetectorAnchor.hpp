#ifndef LOOPDETECTORANCHOR_HPP
#define LOOPDETECTORANCHOR_HPP
#include "AbstractLoopDetector.hpp"
namespace ocfa {
  namespace tsk {
    //This implementation of the AbstractLoopDetector is used as an anchor for loop detection.
    //This anchor always returns false on invocation.
    class LoopDetectorAnchor: public AbstractLoopDetector {
       public:
         LoopDetectorAnchor(){}
         bool operator()(int id){ return false;}
    };
  }
}
#endif
