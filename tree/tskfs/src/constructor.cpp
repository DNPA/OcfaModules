#include "../inc/TskFsFactory.hpp"
extern "C"
{
  ocfa::tsk::TskFsFactory * constructor (std::map<std::string,ocfa::misc::Scalar> *attributes)
  {
    ocfa::tsk::TskFsFactory *factory=new ocfa::tsk::TskFsFactory(attributes);
    return factory;
  }
}

