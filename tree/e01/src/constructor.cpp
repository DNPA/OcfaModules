#include "../inc/E01Factory.hpp"
extern "C"
{
  ocfa::e01::E01Factory * constructor (std::map<std::string,ocfa::misc::Scalar> *attributes)
  {
    ocfa::e01::E01Factory *topnode=new ocfa::e01::E01Factory(attributes);
    return topnode;
  }
}

