#include <CLucene.h>
#include <CLucene/analysis/standard/StandardAnalyzer.h>
#include <CLucene/util/Reader.h>
#include <iostream>
#include "WashAnalyzer.hpp"
using namespace lucene::analysis;
using namespace lucene::analysis::standard;
using namespace lucene::util;
int main(int argc, char *argv[]){

  if (argc != 2){

    cout << "Usage: " << argv[0] << " <filename>" << endl;
    exit (1);
  }
  else {

    try {
      FileReader *reader =  _CLNEW FileReader(argv[1],"UTF-8");
      
      Analyzer *analyzer = new StandardAnalyzer();
      TokenStream *tokenStream = analyzer->tokenStream(_T("iets"), reader);
      Token token;
      while (tokenStream->next(&token)){
	
	char buffer[1000];
	STRCPY_TtoA(buffer, token.termText(), 1000);
	cout << "\t token: " << buffer;
	STRCPY_TtoA(buffer, token.type(), 1000);
	
	cout << " type: " << buffer << endl;
      }
      tokenStream->close();
      delete tokenStream;
      //reader->close();
      delete reader;
      delete analyzer;
    } catch (CLuceneError &e){

      cerr << e.what() << endl;
    }
  }
}
