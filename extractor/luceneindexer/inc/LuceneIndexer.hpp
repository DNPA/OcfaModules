
//  The Open Computer Forensics Architecture moduleset.
//  Copyright (C) 2003..2006 KLPD  <ocfa@dnpa.nl>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef INCLUDED_LUCENCEINDEXER_H
#define INCLUDED_LUCENCEINDEXER_H
#include <sys/stat.h>
#include <facade/EvidenceFileAccessor.hpp>
#include <CLucene.h>
CL_NS_USE(index)

CL_NS_USE(store)

CL_NS_USE(analysis)
class LuceneIndexer : public  ocfa::facade::EvidenceFileAccessor {

public:
  LuceneIndexer();
  virtual ~LuceneIndexer();
  void processEvidence();
  /**
   * small addition to processEvidenceMessage. sets the current MetaDataid.
   */
  void processEvidenceMessage(const ocfa::message::Message &message);
protected:
  bool evidenceHasUtf16();
private:
  ssize_t mMaxSize;
  IndexWriter *mIndexWriter;
  lucene::analysis::Analyzer *mAnalyzer;
  string mCurrentMetaDataId;
  struct stat mStat;
};
#endif
