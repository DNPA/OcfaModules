#include "../inc/MmlsFactory.hpp"
extern "C"
{
  ocfa::tsk::MmlsFactory * constructor (std::map<std::string,ocfa::misc::Scalar> *attributes)
  {
    ocfa::tsk::MmlsFactory *factory=new ocfa::tsk::MmlsFactory(attributes);
    return factory;
  }
}

