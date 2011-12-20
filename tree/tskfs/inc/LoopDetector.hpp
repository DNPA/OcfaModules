#ifndef LOOPDETECTOR_HPP
#define LOOPDETECTOR_HPP
#include "AbstractLoopDetector.hpp"
#include "LoopException.hpp"
namespace ocfa {
  namespace tsk {
    //This is the basic implementation of the loop detector. Its normal operation is to
    //return true if the inode number argument is equal to its own inode number, or return
    //the return value of its parent otherwise.
    //Next to its regular loopdetection, the LoopDetector will throw an exception in its constructor
    //if its parent returns true on its inode number.
    class LoopDetector: public AbstractLoopDetector {
       public:
         LoopDetector(AbstractLoopDetector &parent,int id) throw(LoopException): mParent(parent),mId(id) {
            if (mParent(mId) ) {
               throw LoopException();
            }
         }
         bool operator()(int id){
           return ((mId == id) || mParent(id));
         }         
       private:
         AbstractLoopDetector &mParent;
         int mId;          
    };
  }
}
#endif
