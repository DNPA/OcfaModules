#ifndef PROC_URL_FILTER_HPP
#define PROC_URL_FILTER_HPP
#include <CLucene.h>
#include "CLucene/analysis/AnalysisHeader.h"
#include "CLucene/analysis/Analyzers.h"
//#include "CLucene/StandardTokenizerConstants.h"
#include "CLucene/util/StringBuffer.h"
#include <set>
#include <iostream>
#include <libpq-fe.h>

CL_NS_USE(analysis)


/**
 * Filter that stores all urls and email in a table in the database.
 * TODO: Only email is stored right now.
 *
 */
class ProcessUrlEmailFilter : public TokenFilter {
public:
  ProcessUrlEmailFilter(TokenStream *in, bool deleteTokenStream, PGConn *connection);
  ~ProcessUrlEmailFilter();
  bool next(Token *token);
protected:
  bool isUrl(Token *token);
  void storeInTable(Token *token, string &inTableName); 
private:
  PGConn *pgconn;
};







