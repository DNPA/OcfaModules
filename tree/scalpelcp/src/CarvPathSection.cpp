#include "ScalpelCpTopNode.hpp"
#include "CarvPathSection.hpp"
#include "../../../binaries.hpp"
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <errno.h>

namespace ocfa {
  namespace scalpel {
      CarvPathSection::CarvPathSection(carvpath_entity *cptop,std::string scalpelline):OcfaObject("CarvPathSection","scalpelcp"),mMeta(0),mName("OCFA:UNDEFINED"),mSize(0),mCarvPath("OCFA-INVALID-CP"){
         mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();
         boost::char_separator<char> sep("\t ","",boost::drop_empty_tokens);
         boost::tokenizer<boost::char_separator<char> > tokens(scalpelline, sep);
         off_t cpoffset=0;
         bool chopped=false;
         boost::tokenizer<boost::char_separator<char> >::iterator tok_iter = tokens.begin();                                          
         if (tok_iter != tokens.end()) {
            mName=*tok_iter;
            ++tok_iter;
         }        
         if (tok_iter != tokens.end()) {
            cpoffset = boost::lexical_cast<off_t>(*tok_iter);
            ++tok_iter;
         }
         if (tok_iter != tokens.end()) {
            if (std::string(*tok_iter) == "YES") {
               chopped=true;
            }            
            ++tok_iter;
         }
         if (tok_iter != tokens.end()) {
            mSize=boost::lexical_cast<off_t>(*tok_iter);
            ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("file")));
         } else {
            getLogStream(ocfa::misc::LOG_WARNING) << "Insufficient tokens in line, invalid line!!" << std::endl;
            ocfa::misc::metautil::addMetaToMap(mMeta,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("invalid")));
            ocfa::misc::metautil::addMetaToMap(mMeta,"scalpelline", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(scalpelline)));
         }
         ocfa::misc::metautil::addMetaToMap(mMeta,"size", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(mSize)));
         if (chopped) {
             ocfa::misc::metautil::addMetaToMap(mMeta,"chopped", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(1)));
         } else {
             ocfa::misc::metautil::addMetaToMap(mMeta,"chopped", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(0)));
         }
         carvpath_entity *derived=carvpath_derive(cptop,cpoffset,mSize,CARVPATH_OOR_SPARSE);
         if (derived) {
           mCarvPath=std::string(carvpath_get_as_path(derived)) + ".crv";
           carvpath_free(derived,0); 
         } else {
           getLogStream(ocfa::misc::LOG_WARNING) << "Problem with carvpath_derive for offset=" << cpoffset << ", size=" << mSize << " error=" << carvpath_error_as_string(errno) << std::endl;
         } 
      };
      CarvPathSection::~CarvPathSection() { 
           if (mMeta) {
               map < string, misc::MetaValue * >::const_iterator p;
               for (p = mMeta->begin(); p != mMeta->end(); ++p) {
                 delete p->second;
               }
               delete mMeta;
               mMeta=0;
           }         
      };
      void CarvPathSection::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {    
          *map=mMeta;
          mMeta=0;         
      };       
  }
}
