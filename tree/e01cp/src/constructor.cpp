#include "../inc/E01CpFactory.hpp"
extern "C"
{
  ocfa::e01cp::E01CpFactory * constructor (std::map<std::string,ocfa::misc::Scalar> *attributes)
  {
    ocfa::e01cp::E01CpFactory *topnode=new ocfa::e01cp::E01CpFactory(attributes);
    return topnode;
  }
}

