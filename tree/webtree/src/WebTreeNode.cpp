#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPMessage.h>
#include <Poco/Path.h>
#include <Poco/URI.h>
#include <Poco/Exception.h>
#include "../inc/WebTreeNode.hpp"
#include <boost/lexical_cast.hpp>
namespace ocfa {
  namespace webtree {

     WebTreeNode::WebTreeNode(std::string path,std::string timesource, WebTreeNode *parent):OcfaObject("WebTreeNode","mmls"),mURI(path),mTimeSource(timesource),mCurrentChildIndex(0), mMeta(0) {
          mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();
          try {
              mURI.normalize();
              std::string path(mURI.getPathAndQuery());
              if (path.empty()) { path="/";}
              Poco::Net::HTTPClientSession session(mURI.getHost(),mURI.getPort());
              Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_HEAD,path,Poco::Net::HTTPRequest::HTTP_1_1);
              session.sendRequest(request);
              Poco::Net::HTTPResponse response;
              session.receiveResponse(response);
              if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
                  (*mMeta)["nodetype"]= new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("webnode")); 
                  std::string contenttype="";
                  Poco::Net::NameValueCollection::ConstIterator  hiter;
                  for (hiter=response.begin() ; hiter != response.end() ; hiter++) {
                     std::string key=(*hiter).first;
                     std::string val=(*hiter).second;
                     (*mMeta)[key] = new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(val));
                     if (key == "Size") {
                       mSize=boost::lexical_cast<size_t>(val);
                     }
                     if (key == "Content-Type") {
                        contenttype=val;
                     }
                  } 
                  if (contenttype == "x-ocfa/directory-xml") {
                      Poco::Net::HTTPRequest getrequest(Poco::Net::HTTPRequest::HTTP_GET,path,Poco::Net::HTTPRequest::HTTP_1_1);
                      session.sendRequest(getrequest);
                      Poco::Net::HTTPResponse getresponse;
                      std::istream &bodystream = session.receiveResponse(getresponse);
                      if (getresponse.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
                          //FIXME, extract child evidences from the content.
                      }
                  }

              } else {
                 (*mMeta)["nodetype"]= new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("brokenwebnode"));
                 (*mMeta)["problem"]= new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(response.getReasonForStatus(response.getStatus())));
              }
          } catch (Poco::Exception &ex) {
              (*mMeta)["nodetype"]= new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar("brokenwebnode"));
              (*mMeta)["problem"]= new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(ex.displayText()));
          }          
     }

     WebTreeNode::~WebTreeNode() {
         if (mMeta) {
            map < string, misc::MetaValue * >::const_iterator p;
            for (p = mMeta->begin(); p != mMeta->end(); ++p) {
               delete p->second;
            }
            delete mMeta;
            mMeta=0;
         }
     }

     //Private helper method for detemining if the top node has DATA content
     bool WebTreeNode::hasContent() { 
        if (mSize > 0) {
          return true;
        }
        return false;
     }

     off_t WebTreeNode::getSize() {
         return mSize;        
     }
     
     std::string WebTreeNode::getSoftLinkablePath(ocfa::misc::DigestPair **) {
        return "";
     }

     std::string  WebTreeNode::getHardLinkablePath(std::string targetbasepath,ocfa::misc::DigestPair **) {
        return "";
     }

     bool WebTreeNode::hasSubEntities() {
         if (mChildren.size() > 0) {
            return true;
         }
         return false;
     }
  
     void WebTreeNode::resetSubEntityIterator() {
         mCurrentChildIndex=0;         
     }

     bool WebTreeNode::nextSubEntity() {
         if ((mChildren.size()-1) > mCurrentChildIndex) {
             mCurrentChildIndex++;
             return true;
         }
         return false;
     }    
 
     void WebTreeNode::getCurrentSubEntity(TreeGraphNode ** subent) {
        *subent=new WebTreeNode(mChildren[mCurrentChildIndex],mTimeSource,this);
     }

     std::string WebTreeNode::getCurrentSubEntityRelation() {
        return "webnode"; 
     }

     std::string WebTreeNode::getName() {
        return "webnode";
     }

     void WebTreeNode::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
         *map=mMeta;
         mMeta=0;
     }
     bool WebTreeNode::hasParentUrl(std::string url) {
         return false;
     }
  }
}
