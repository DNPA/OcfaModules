#include "BlklsTopNode.hpp"
#include "StreamUnallocated.hpp"
#include "misc.hpp"
#include<libcarvpath.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include<errno.h>

using namespace ocfa::misc;
namespace ocfa {
  namespace tsk {

    // path is the full path and filename to a raw image file, possibly a carvpath.
    // if path ends in ".crv" we consider it to be a carvpath and softlinkable paths may be derived from its sub-entities.
    // if path does not end with ".crv" we derive sub-entities which are read by streamToOutput.
    // in both cases we use the sleuthkit walk_* callback functions to generate carvpaths for each continuous chunk of unallocated blocks.
    // The file should contain one partition containing a valid filesystem.
    // If there is no valid filesystem present we assume the complete file as unallocated data.
    // 
    BlklsTopNode::BlklsTopNode(std::string path):OcfaObject("BlklsTopNode","blkls"), mMeta(0), mFsInfo(0), mImgInfo(0), mCpLib(0), mCarvpathTop(0), mFragIx(0), mLastBlockIx(0), mLastWalkState(TSK_WALK_STOP), mPseudoCarvPath(false) {
      ocfaLog(ocfa::misc::LOG_DEBUG, std::string("TopNode created with path: ") + path); //RJM: make this (and others!)  LOG_INFO ?
      mMeta= new std::map < std::string, ocfa::misc::MetaValue * >();

      //Make sure our path if a symbolic link gets dereferenced first.
      mRealPath=fsutil::dereferencePath(path);
      struct stat basestat;
      if (lstat(mRealPath.c_str(),&basestat)== -1){
         throw ocfa::misc::OcfaException("Problem calling stat on "+mRealPath,this);
      }
      mCpLib = carvpath_init(0,0);
      if (!mCpLib){
         throw ocfa::misc::OcfaException("LibCarvPath initialization failed.");
      }
      
      if ((mRealPath.size() > 4) && (mRealPath.find(".crv",mRealPath.size()-4) != std::string::npos )) {

	//If our single file image is contained in carvFs, set our mCarvpathTop with a usable carvpath top entity
	//that we later can derive other carvpaths from. 
        std::string cpbasename=mRealPath.substr(0, mRealPath.size() - 4);
	mCarvpathTop = carvpath_top_entity(basestat.st_size,(char *) cpbasename.c_str(), mCpLib);

	OcfaLogger::Instance()->syslog(LOG_DEBUG, "BlklsTopNode") << "CarvPathTop created with size " << basestat.st_size << " and path " << cpbasename << endl ;
      } else {
        // The file is not a carvpath, but we treat it as one and will return special subentities later on which will mask the derived carvpaths
        mPseudoCarvPath=true;
        mCarvpathTop = carvpath_top_entity(basestat.st_size,(char *) mRealPath.c_str(), mCpLib);
        OcfaLogger::Instance()->syslog(LOG_DEBUG, "BlklsTopNode") << "pseudo CarvPathTop created with size " << basestat.st_size << " and path " << mRealPath << endl ; 
      }
      //Unless otherwise determined, the input is assumed to be a single image raw (dd) file containing one filesystem.
      TSK_IMG_TYPE_ENUM imagetype=TSK_IMG_TYPE_RAW_SING;
      TSK_TCHAR *fnames[2];
      fnames[0] = new TSK_TCHAR[mRealPath.size()+1];
      strcpy(fnames[0], mRealPath.c_str());
      fnames[1] = 0;
#ifdef TSK_VERSION_NUM
      mImgInfo = tsk_img_open(1, (const TSK_TCHAR**) fnames, imagetype, 0);
#else
      mImgInfo = tsk_img_open(1, (const TSK_TCHAR**) fnames, imagetype);
#endif
      mFsInfo=tsk_fs_open_img(mImgInfo, 0, TSK_FS_TYPE_DETECT);
      if (mFsInfo == 0) {
	OcfaLogger::Instance()->syslog(LOG_WARNING,"BlklsTopNode") << std::string("Could not autodetect valid filesystem on: ") << std::string(fnames[0]);
        ocfa::misc::metautil::addMetaToMap(mMeta,"validformat", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(0)));
	mValidFormat = false;
      } else {
        ocfa::misc::metautil::addMetaToMap(mMeta,"validformat", new ocfa::misc::ScalarMetaValue(ocfa::misc::Scalar(1)));
        mValidFormat = true;
      }
      delete []fnames[0];
      // preemptive initialize the subentity iterator in case the treegraph lib doesn't
      resetSubEntityIterator(); 
    }


    BlklsTopNode::~BlklsTopNode() {
      ocfaLog(ocfa::misc::LOG_DEBUG, std::string("Destructor BlklsTopNode"));
      if (mFsInfo) {
         delete mFsInfo;
      }
      if (mImgInfo) {
	tsk_img_close(mImgInfo);
	mImgInfo=0;
      }
      int failure = 0;
      if (mCarvpathTop) {
	carvpath_free(mCarvpathTop, &failure);
	mCarvpathTop=0; 
      }
      if (mCpLib){
        carvpath_finish(mCpLib);
      } 
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
    bool BlklsTopNode::hasContent() { 
      // the topnode will return its content as one chunck of unallocated data, specified as a carvpath.
      return false;
    }

     
    off_t BlklsTopNode::getSize() {
      //RJM: given that hasContent returns false, this method should not get called, we could do with a litle less code here.
      ocfaLog(ocfa::misc::LOG_ERR, std::string("getSize called. Returning 0. FIXME"));
	      return 0;
      // fill the mCarvpathTop
      getSoftLinkablePath(NULL);
      int failure = 0;
      return carvpath_get_size(mCarvpathTop, &failure);  
    }
     
    void BlklsTopNode::openStream() {
    }

    void BlklsTopNode::closeStream() {
    }

    size_t BlklsTopNode::streamRead(char *buf, size_t count) {
      return 0;
    }


    // sleuthkit callback function, not a method of BlklsTopNode
    TSK_WALK_RET_ENUM streamdata_action_eager(const TSK_FS_BLOCK *a_block, void *ptr){
      carvpath_entity *cpe = (carvpath_entity *)ptr;
      int failure = 0;
      carvpath_append(cpe, (a_block->addr *  a_block->fs_info->block_size ), a_block->fs_info->block_size, &failure, CARVPATH_OOR_FAIL);
      if (failure != CARVPATH_ERR_OUT_OF_RANGE){
	return TSK_WALK_CONT;
      } else {
	return TSK_WALK_STOP;
      }
    }

    // sleuthkit callback function, not a method of BlklsTopNode
    TSK_WALK_RET_ENUM streamdata_action_append_cp_entity(const TSK_FS_BLOCK *a_block, void *ptr){
      walkstate *WalkState = (walkstate *)ptr;
      carvpath_entity *cpe = WalkState->currentity;
      int failure = 0;
      // first we try to predict whether adding the current block will create a new fragment, or whether it will be merged by libcarvpath to the existing last one
      if ( a_block->addr == WalkState->lastblockix ){
	// libcarvpath should merge
	carvpath_append(cpe, (a_block->addr *  a_block->fs_info->block_size ), a_block->fs_info->block_size, &failure, CARVPATH_OOR_FAIL);
	if (failure){
            OcfaLogger::Instance()->syslog(LOG_ERR, "streamdata_action_append_cp_entity: carvpath_append: ") << string(carvpath_error_as_string(errno)) <<  endl;
        }
        // if we return TSK_WALK_CONT, but the walk stops anyway, we'll infer that apparently we reached eof.
        WalkState->lastblockix = a_block->addr+1;
        WalkState->returnedval = TSK_WALK_CONT;
	return TSK_WALK_CONT;
      } else {
	// lib carvpath would create new fragment. Don't update the lastblockix 
        WalkState->returnedval = TSK_WALK_STOP;
	return TSK_WALK_STOP;
      }
       
    }


    // sleuthkit callback function, not a method of BlklsTopNode
    TSK_WALK_RET_ENUM streamdata_action_new_cp_entity(const TSK_FS_BLOCK *a_block, void *ptr){
      walkstate *WalkState = (walkstate *)ptr;
      carvpath_entity *cpe = WalkState->topentity;
      int failure = 0;
      WalkState->currentity = carvpath_derive(cpe, (a_block->addr *  a_block->fs_info->block_size ), a_block->fs_info->block_size, CARVPATH_OOR_FAIL);
      if (!WalkState->currentity){
         OcfaLogger::Instance()->syslog(LOG_ERR, "streamdata_action_new_cp_entity: carvpath_derive out of range") << endl; 
      }    
      // return STOP, caller should continue walk with streamdata_action_appendfragments
      WalkState->lastblockix = a_block->addr+1;
      return TSK_WALK_STOP;
    }
  
    // Calling softlinkablepath will generate a carvpath of all unallocated data. This might be a lengthy (and eager) operation.
    // It might be better to use the subentity interface, which will try to return data in a interleaved (and lazy) manner
    std::string BlklsTopNode::getSoftLinkablePath(ocfa::misc::DigestPair **) {
        //RJM: Wont get called given that hasContent() returns false. 
        ocfaLog(ocfa::misc::LOG_NOTICE, std::string("getSoftLinkablePath called. Expensive operation. Should FIXME"));
        // derive first level carvpath-entity to add chucks to.
        carvpath_entity *dummy = carvpath_derive(mCarvpathTop, 0, 0, CARVPATH_OOR_FAIL);
        tsk_fs_block_walk(mFsInfo,0, mFsInfo->last_block_act, TSK_FS_BLOCK_WALK_FLAG_UNALLOC, streamdata_action_eager, dummy);
        int failure;
        carvpath_entity *flattened = carvpath_flatten(dummy);
        return carvpath_get_as_path(flattened);

    }

    std::string  BlklsTopNode::getHardLinkablePath(std::string targetbasepath,ocfa::misc::DigestPair **) {
      //RJM: Wont get called given that hasContent() returns false.
      // hardlinkable path probably does not make sense, unless we copy out all unallocated blocks to a file and return the filename.
      ocfaLog(ocfa::misc::LOG_ERR, std::string("getHardLinkablePath called. FIXME"));
      return "";
    }

    bool BlklsTopNode::hasSubEntities() {
      if (!mValidFormat){
	 return false;
      }
      ocfaLog(ocfa::misc::LOG_NOTICE, std::string("hasSubenties called. Returning true but this is a FIXME."));
      return true; // FIXME: Howto determine whether there are unallocated blocks without first trying??       
    }
  
    void BlklsTopNode::resetSubEntityIterator() {
      if (!mValidFormat){
	 return ;
      }
      ocfaLog(ocfa::misc::LOG_DEBUG, std::string("resetSubEntityIterator called"));
      mCurrSubEntity = 0;  
      mFragIx = 0; 
      mLastBlockIx = 0;
      mLastWalkState = TSK_WALK_STOP; 
      // set to the first subEntity
      nextSubEntity(); 
    }

    bool BlklsTopNode::nextSubEntity() {
      int failure = 0;
      // to prevent memleaks, first delete the previous subentity, if any
      if (mCurrSubEntity){
         carvpath_free(mCurrSubEntity, &failure);
         mCurrSubEntity = 0;
      }
      if (!mValidFormat){
         return false;
      }
      ocfaLog(ocfa::misc::LOG_DEBUG, std::string("nextSubEntity called"));
      if (mLastWalkState == TSK_WALK_CONT){
        // if we returned TSK_WALK_CONT last time, but the tsk_fs_block_walk returned anyway, then apparently we reached EOF back then. 
        // for now, we assume that then there are no more blocks to get
        ocfaLog(ocfa::misc::LOG_DEBUG, std::string("End of subentities reached. Returning false."));
        return false;
      }
      walkstate WalkState;
      WalkState.lastblockix = mLastBlockIx;
      WalkState.topentity = mCarvpathTop;
      WalkState.currentity=0;
      // Init a new carvpath entity with block
      if ( tsk_fs_block_walk(mFsInfo, mLastBlockIx, mFsInfo->last_block_act, TSK_FS_BLOCK_WALK_FLAG_UNALLOC, streamdata_action_new_cp_entity, &WalkState) == 1){
  ocfaLog(ocfa::misc::LOG_DEBUG, std::string("tsk_fs_block_walk(new_cp_entity) returned 1. returning false in nextSubEntity."));
          return false;
      }
      // make the walk continue at the next block
      if (tsk_fs_block_walk(mFsInfo, WalkState.lastblockix, mFsInfo->last_block_act, TSK_FS_BLOCK_WALK_FLAG_UNALLOC, streamdata_action_append_cp_entity, &WalkState) == 1 ){
          ocfaLog(ocfa::misc::LOG_DEBUG, std::string("tsk_fs_block_walk(append_cp_entity) returned 1. returning false in nextSubEntity."));
          return false;
      }
      mLastWalkState = WalkState.returnedval;
      mLastBlockIx = WalkState.lastblockix;
      mCurrSubEntity = WalkState.currentity;
      return (mCurrSubEntity != 0);
    }    
 
    void BlklsTopNode::getCurrentSubEntity(TreeGraphNode ** subent) {
      ocfaLog(ocfa::misc::LOG_DEBUG, std::string("getCurrentSubEntity called"));
      if (mCurrSubEntity == 0){
         ocfaLog(ocfa::misc::LOG_INFO, std::string("mCurrSubEntity == 0"));
         *subent = 0;
         return;
      }
      int failure = 0;
      char *entitypath = carvpath_get_as_path(mCurrSubEntity);
      
      if (*subent) {
	throw ocfa::misc::OcfaException("BlklsTopNode::getCurrentSubEntity called with subent not cleared");
      }
      if (mPseudoCarvPath){
	ocfaLog(ocfa::misc::LOG_DEBUG, std::string("pseudo carvpath=") + std::string(entitypath));
        OcfaLogger::Instance()->syslog(LOG_DEBUG, "BlklsTopNode") << "Creating StreamUnallocated with filename: " << mRealPath << " Offset: " << carvpath_fragment_get_offset(mCurrSubEntity, 0, &failure) << " Size: " << carvpath_fragment_get_size(mCurrSubEntity, 0, &failure) << endl;  
        *subent = new StreamUnallocated(mRealPath, carvpath_fragment_get_offset(mCurrSubEntity, 0, &failure), carvpath_fragment_get_size(mCurrSubEntity, 0, &failure), false );
      } else {
        *subent = new StreamUnallocated(carvpath_get_as_path(carvpath_flatten(mCurrSubEntity)), carvpath_fragment_get_offset(mCurrSubEntity, 0, &failure), carvpath_get_size(mCurrSubEntity, &failure), true);
      }
      return;
    }

    std::string BlklsTopNode::getCurrentSubEntityRelation() {
      return "unallocated"; 
    }

    std::string BlklsTopNode::getName() {
      return "unallocated";
    }

    void BlklsTopNode::takeMetaMap(std::map < std::string, misc::MetaValue * >**map) {
      if (*map == 0){
        *map=mMeta;
        mMeta=0;
      } else {
        throw ocfa::misc::OcfaException("BlklsTopNode::takeMetaMap called with **map not cleared.");
      }
    }

  }
}
