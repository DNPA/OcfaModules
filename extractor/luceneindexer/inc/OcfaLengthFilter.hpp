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
/**
 * A token filter that disallows tokens smaller than 2 and not on the whitelist.
 * Further it will return false if more than maxAmountOfTerms are retrieved.
 */
class OcfaLengthFilter: public TokenFilter {
public:
  OcfaLengthFilter(TokenStream *in, bool deleteTokenStream, int inMaxAmountOfTerms = -1);

  ~OcfaLengthFilter();
  bool next(Token *token);
private:
  std::set<TCHAR *, TCharCmp> whiteList;
  int maxAmountOfTerms;
  int amountOfTerms;
};

#endif
