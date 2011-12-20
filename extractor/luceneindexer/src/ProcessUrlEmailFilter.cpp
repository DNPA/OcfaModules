#include "ProcessUrlEmailFilter.hpp"

#include 
ProcessUrlEmailFilter::ProcessUrlEmailFilter(TokenStream *in, bool deleteTokenStream, PGConn *connection):
  TokenFilter(in, deleteTokenStream){

  pgconn = connection;
}



ProcessUrlEmailFilter::next(Token *token){

  if (input->next(token)){

    if (token->type() == _T("<EMAIL>")){


      storeInTable(token, "emailaddresses");
    }
    else if (token->type() == _T("<URL>")){

      storeInTable(token, "urls");
    }
    return true;
  }
  else {

    return false;
  }
}

bool ProcessUrlEmailFilter::isUrl(Token *token){

  
  
}

