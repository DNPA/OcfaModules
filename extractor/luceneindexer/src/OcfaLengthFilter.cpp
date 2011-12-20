

#include <OcfaLengthFilter.hpp>
#include <iostream>




OcfaLengthFilter::OcfaLengthFilter(TokenStream *in, bool deleteTokenStream, int inMaxAmountOfTerms):
  TokenFilter(in, deleteTokenStream), amountOfTerms(0) {
#ifdef OCFA_USE_SA_WHITELIST
  whiteList.insert(_T("aa"));
  whiteList.insert(_T("ab"));
  whiteList.insert(_T("ae"));
  whiteList.insert(_T("ag"));
  whiteList.insert(_T("ak"));
  whiteList.insert(_T("as"));

  whiteList.insert(_T("ba"));
  whiteList.insert(_T("bs"));
  whiteList.insert(_T("bv"));

  whiteList.insert(_T("ca"));
  whiteList.insert(_T("cb"));
  whiteList.insert(_T("cl"));
  whiteList.insert(_T("co"));
  whiteList.insert(_T("cs"));
  whiteList.insert(_T("cv"));

  whiteList.insert(_T("dc"))
;
  whiteList.insert(_T("ec"));

  whiteList.insert(_T("mc"));
  whiteList.insert(_T("mp"));

  whiteList.insert(_T("nb"));
  whiteList.insert(_T("nv"));

  whiteList.insert(_T("ra"));
  whiteList.insert(_T("rc"));

  whiteList.insert(_T("sa"));
  whiteList.insert(_T("sg"));
  whiteList.insert(_T("sl"));
  whiteList.insert(_T("d"));
  whiteList.insert(_T("ua"));
#endif
  maxAmountOfTerms = inMaxAmountOfTerms;
}

OcfaLengthFilter::~OcfaLengthFilter(){
 
}

/**
 * checks length of words and whether the maxAmount of terms was given.
 *
 */
bool OcfaLengthFilter::next(Token *token){

  if ((maxAmountOfTerms > 0) && (amountOfTerms >= maxAmountOfTerms)){
    return false;
  }
 else{
   while(input->next(token)){

     char buffer[1000];
     STRCPY_TtoA(buffer, token->termText(), 1000);
     //cout << "checking token: " << buffer <<  " length: " << token->termTextLength() << endl;
     
     
     if ((token->termTextLength() > 2)
	 || (whiteList.find((TCHAR *)token->termText()) != whiteList.end())){
       
       amountOfTerms++;
       return true;
     }
   }
   return false;
 }
}
