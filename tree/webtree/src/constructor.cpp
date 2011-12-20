#include "../inc/WebTreeFactory.hpp"
extern "C"
{
  ocfa::webtree::WebTreeFactory * constructor (std::map<std::string,ocfa::misc::Scalar> *attributes)
  {
    ocfa::webtree::WebTreeFactory *factory=new ocfa::webtree::WebTreeFactory(attributes);
    return factory;
  }
}

