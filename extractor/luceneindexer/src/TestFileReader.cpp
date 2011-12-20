#include <CLucene.h>
#include <CLucene/analysis/standard/StandardAnalyzer.h>
#include <CLucene/util/Reader.h>
#include <iostream>
#include "WashAnalyzer.hpp"
using namespace lucene::analysis;
using namespace lucene::analysis::standard;
using namespace lucene::util;
int main(int argc, char *argv[]){

  try {
    if (argc != 2){
      
      cout << "Usage: " << argv[0] << " <filename>" << endl;
      exit (1);
    }
    else {
      
      FileReader *reader =  _CLNEW FileReader(argv[1], "LATIN1");
      const TCHAR *bufferUCS = new TCHAR[101];
      while (reader->read(bufferUCS,(int32_t) 0,(int32_t)100) > 0 ){
	
	char buffer[1000];
	STRCPY_TtoA(buffer, bufferUCS, 100);
	cout << buffer;
	
      }

      
      delete reader;
    }
  } catch (CLuceneError &e){

    cerr << e.what() << endl;

  }
}
