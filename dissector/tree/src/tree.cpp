
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

#include <tree.hpp>
#include <stdexcept>
#include <vector>
#include <regex.h>
#include <treegraph/TreeGraphModuleLoader.hpp>
#include <misc/Exception.hpp>

using namespace
  ocfa::facade;
using namespace
  ocfa::evidence;
using namespace
  ocfa::misc;

treeDissector::treeDissector(std::string treemodule):EvidenceDeriveAccessor(treemodule,"default") {
  mAttributes["workdir"]=Scalar(getWorkDir());
  ocfa::treegraph::TreeGraphModuleLoader::selectAndInit(treemodule,&mAttributes); 
}

void  treeDissector::processEvidenceMessage(const ocfa::message::Message &inMessage){

  ocfaLog(LOG_DEBUG, "entering treeDissector::processEvidenceMessage");
  if (inMessage.getSender() == 0){
    throw OcfaException("received evidence message without a sender ?? ", this);
  }
  // JBS might not be necessary to copy all this stuff, I just do it to prevent untraceable
  // memory bugs.
  if (mpRouter != 0){
    delete mpRouter;
  }
  mpRouter = new ModuleInstance(inMessage.getSender());
  EvidenceFileAccessor::processEvidenceMessage(inMessage);
}


void treeDissector::processEvidence ()
{
  ocfa::store::EvidenceStoreEntity * evidence = fetchEvidenceStoreObject ();
  if (evidence) {
    std::string datapath=evidence->getAsFilePath();
    delete evidence;
    std::string rootnodename=getConfEntry("rootnodename");
    if (rootnodename == "") {
        rootnodename = "toptreenode";
    }
    Evidence *output=derive(datapath,Scalar(rootnodename,"LATIN1"),"content");
    submitEvidence(output);
    delete(output);
  } else {
    throw OcfaException ("Evidence without evidence store", this);
  }
  return; 
}


int main(int argc , char **argv)
{
  treeDissector *tree = 0;
  if (argc < 2) {
     cerr << "Usage : \n\t" << argv[0] << " <treemodulename>\n\n";
  }
  try
  {
    tree = new treeDissector(argv[1]);
  } catch (OcfaException &ex)
  {
    OcfaLogger::Instance()->syslog(LOG_ERR, "tree.default") << "TreeDissector loader error: " << ex.what() << "for tree module '" << argv[1] << "'." << endl; 
    cerr << "Ocfa Exeption cougth in module constructor: " << ex.
      what () << "\n";
    return 2;
  }
  try
  {
    tree->run ();
  }
  catch (OcfaException &ex)
  {
    tree->getLogStream (LOG_CRIT) << "Ocfa Exeption cougth: " << ex.what () << "\n";
  } 
  catch (std::runtime_error &ex) 
  {
     tree->getLogStream (LOG_CRIT) << "std::runtime_error cought: " << ex.what() << "\n";
  }
  catch (std::exception &ex) 
  {
     OcfaException fakeex(ex.what());
     fakeex.logWhat();
     tree->getLogStream (LOG_CRIT) << "std::exception cought: " << ex.what() << "\n";
  }
  catch (std::string &s) 
  {
    tree->getLogStream (LOG_CRIT) << "Tree module RUN :String as exception cought: " << s << std::endl;
  }
  catch (...)
  {
    tree->getLogStream (LOG_CRIT) << "Tree module RUN : Non Ocfa Exeption cougth\n";
  }
  //tree->PrintObjCount ();
  delete tree;
  return 1;
}
