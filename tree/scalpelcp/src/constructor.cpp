#include "../inc/ScalpelCpFactory.hpp"
extern "C"
{
  ocfa::scalpel::ScalpelCpFactory * constructor (std::map<std::string,ocfa::misc::Scalar> *attributes)
  {
    ocfa::scalpel::ScalpelCpFactory *factory=new ocfa::scalpel::ScalpelCpFactory(attributes);
    return factory;
  }
}

