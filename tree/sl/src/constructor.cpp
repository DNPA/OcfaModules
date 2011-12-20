#include "../inc/SoftLinkFactory.hpp"
extern "C"
{
  ocfa::sl::SoftLinkFactory * constructor (std::map<std::string,ocfa::misc::Scalar> *attributes)
  {
    ocfa::sl::SoftLinkFactory *topnode=new ocfa::sl::SoftLinkFactory(attributes);
    return topnode;
  }
}

