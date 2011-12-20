#ifndef ABSTRACTLOOPDETECTOR_HPP
#define ABSTRACTLOOPDETECTOR_HPP
namespace ocfa {
  namespace tsk {
     //This is the interface for loop detection function objects. 
     //Each directory has a loopdetector member that cann be called with an inode number.
     //The function object returns true if the given inode number belogs to itself or one of its parents.
     class AbstractLoopDetector {
       public:
          virtual bool operator()(int id)=0;
          virtual ~AbstractLoopDetector(){}
     };     
  }
}
#endif
