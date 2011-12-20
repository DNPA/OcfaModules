#include "../inc/RawCpFactory.hpp"
extern "C"
{
  ocfa::rawcp::RawCpFactory * constructor (std::map<std::string,ocfa::misc::Scalar> *attributes)
  {
    ocfa::rawcp::RawCpFactory *topnode=new ocfa::rawcp::RawCpFactory(attributes);
    return topnode;
  }
}

