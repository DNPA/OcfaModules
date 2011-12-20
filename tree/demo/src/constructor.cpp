#include "../inc/DemoFactory.hpp"
extern "C"
{
  ocfa::demo::DemoFactory * constructor (std::map<std::string,ocfa::misc::Scalar> *attributes)
  {
    ocfa::demo::DemoFactory *topnode=new ocfa::demo::DemoFactory(attributes);
    return topnode;
  }
}

