#include "../inc/BlklsFactory.hpp"
extern "C"
{
  ocfa::tsk::BlklsFactory * constructor (std::map<std::string,ocfa::misc::Scalar> *attributes)
  {
    ocfa::tsk::BlklsFactory *factory=new ocfa::tsk::BlklsFactory(attributes);
    return factory;
  }
}

