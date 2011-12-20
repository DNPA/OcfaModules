#include <WashAnalyzer.hpp>
using namespace lucene::util;
using namespace lucene::analysis;
using namespace lucene::analysis::standard;
WashAnalyzer::WashAnalyzer(): StandardAnalyzer(){

}

TokenStream *WashAnalyzer::tokenStream(const TCHAR *fieldName, Reader *reader){

  return _CLNEW OcfaLengthFilter(StandardAnalyzer::tokenStream(fieldName, reader), true);
}

WashAnalyzer::~WashAnalyzer(){
}
