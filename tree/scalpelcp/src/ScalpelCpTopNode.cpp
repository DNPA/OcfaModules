#include "ScalpelCpTopNode.hpp"
#include "CarvPathSection.hpp"
#include <misc/OcfaConfig.hpp>
#include "../../../binaries.hpp"
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>

namespace ocfa {
  namespace scalpel {
          ScalpelCpTopNode::ScalpelCpTopNode(std::string path,carvpath_library *cplib,std::string workdir):OcfaObject("ScalpelCpTopNode","scalpelcp"),mCarvpathTop(0),mWorkDir(workdir),mActiveLine(0){
             std::string realpath=ocfa::misc::fsutil::dereferencePath(path);
             if ((realpath.size() > 4) && (realpath.find(".crv",realpath.size()-4) != std::string::npos )) { 
                std::string etcdir = ocfa::misc::OcfaConfig::Instance()->getValue("ocfaetc");
                std::string cmd= std::string(BINARY_SCALPEL) + " -c " + etcdir +"/scalpel.conf -p -o " + workdir + " " + realpath + " 2>/dev/null >/dev/null";
                int status = system(cmd.c_str());
                if(WEXITSTATUS(status) == 0) {  
                                        
                } else {

                }
                std::string reportpath=std::string(workdir) + "/audit.txt"; 
                ifstream auditlogstream(reportpath.c_str());
                if (auditlogstream.fail()) {
                   getLogStream(ocfa::misc::LOG_ERR) << "Opening audit file at : '" << reportpath << "' FAILED" << std::endl;
                }
                size_t linecount=0;  
                while (!auditlogstream.fail() && !auditlogstream.eof()){ 
                  std::string line;
                  getline(auditlogstream, line);
                  if (line.c_str()[0] == '0') { 
                     linecount++;
                     mCarvLines.push_back(line);
                  }                  
                }
                auditlogstream.close();
                unlink(reportpath.c_str());  
                std::string cpbasename=realpath.substr(0, realpath.size() - 4);
                struct stat basestat;
                if (lstat(realpath.c_str(),&basestat)== -1) {
                   throw ocfa::misc::OcfaException("Problem calling stat on "+realpath,this);
                }
                mCarvpathTop = carvpath_top_entity(basestat.st_size,(char *) cpbasename.c_str(),cplib);
             } else {
                getLogStream(ocfa::misc::LOG_WARNING) << "The scalpelcp module only works on CarvFS input. The input supplied (" << path <<")."  << std::endl << "("<< realpath << ") is a raw data file. USE PHOTOREC INSTEAD !!" << std::endl;
             }             
          }

          ScalpelCpTopNode::~ScalpelCpTopNode(){
             if (mCarvpathTop) {
               carvpath_free(mCarvpathTop, 0);
               mCarvpathTop=0;
             }
          }
          bool ScalpelCpTopNode::hasSubEntities(){
            return (mCarvLines.size() > 0);
          }
          void ScalpelCpTopNode::resetSubEntityIterator() {
            mActiveLine=0;
          }
          bool ScalpelCpTopNode::nextSubEntity() {
             if ((mActiveLine+1) < mCarvLines.size()) {
                mActiveLine++;
                return true;
             }
             return false;
          }
          void ScalpelCpTopNode::getCurrentSubEntity(TreeGraphNode ** subent) {
             *subent = new CarvPathSection(mCarvpathTop,mCarvLines[mActiveLine]);
             return;
          }
          void ScalpelCpTopNode::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
             *map= new std::map < std::string, ocfa::misc::MetaValue * >();
             ocfa::misc::metautil::addMetaToMap(*map,"nodetype", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("pseudodir")));
             return;
          }
  }
}

