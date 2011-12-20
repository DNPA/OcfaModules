#ifndef LENGTH_FILTER_HPP
#define LENGTH_FILTER_HPP
#include <CLucene.h>
#include "CLucene/analysis/AnalysisHeader.h"
#include "CLucene/analysis/Analyzers.h"
//#include "CLucene/StandardTokenizerConstants.h"
#include "CLucene/util/StringBuffer.h"
#include <set>
#include <iostream>
class TCharCmp {
public:
  inline bool operator()(const TCHAR *in1, const TCHAR *in2){
       
    return (_tcscmp(in1, in2) < 0);
  }
};

CL_NS_USE(analysis)
class LengthFilter: public TokenFilter {
public:
  LengthFilter(TokenStream *in, bool deleteTokenStream);
  ~LengthFilter();
  bool next(Token *token);
private:
  std::set<TCHAR *, TCharCmp> whiteList;
};

#endif
