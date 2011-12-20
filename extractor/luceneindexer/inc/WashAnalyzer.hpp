#include <CLucene.h>
#include "CLucene/util/Reader.h"
#include "CLucene/analysis/AnalysisHeader.h"
#include "CLucene/analysis/Analyzers.h"
#include "CLucene/analysis/standard/StandardFilter.h"
#include "CLucene/analysis/standard/StandardTokenizer.h"
#include "OcfaLengthFilter.hpp"

class WashAnalyzer : public lucene::analysis::standard::StandardAnalyzer {
public:
  
  WashAnalyzer();
  ~WashAnalyzer();
  lucene::analysis::TokenStream *tokenStream(const TCHAR *fieldname, 
			   lucene::util::Reader *reader);
};

