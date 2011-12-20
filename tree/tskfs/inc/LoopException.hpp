#ifndef LOOP_EXCEPTION
#define LOOP_EXCEPTION
#include <stdexcept>
namespace ocfa {
  namespace tsk {
     //The exeption thrown by LoopDetector if it is constructed while its parents processing of the inode number detects a loop.
     class LoopException: public std::runtime_error {
        public: 
           LoopException():std::runtime_error("LoopException"){}
     };
  }
}
#endif
