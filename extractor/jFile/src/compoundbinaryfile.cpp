
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

#include<exception>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<iconv.h>
#include<errno.h>
#include<strstream>

#include"../inc/compoundbinaryfile.hpp"


CompoundBinaryFile::CompoundBinaryFile(string filename){

  BYTE magic[]= {0xd0, 0xcf, 0x11, 0xe0, 0xa1, 0xb1, 0x1a, 0xe1};
  d_filename = filename;
  d_knowntype = false;
  d_direntry = 0;
  d_ifstr = 0;
  d_maxdirentries = 0;
  struct stat buf;
  if (stat(filename.c_str(), &buf) != 0){
    throw string("Stat failed.");    
  }
  d_filesize = buf.st_size;

  d_ifstr = new ifstream(filename.c_str(), ios::in | ios::binary);
  if (!d_ifstr){
    throw string("Could not open file " + filename);
  }
  d_ifstr->read(reinterpret_cast<char *>(&d_header), sizeof(d_header));
  for (int i = 0; i< 8; i++){
    if (magic[i] != d_header._abSig[i]){
      throw string("Magic test failed.");
    }
  }
  d_sectorsize = (1 << d_header._uSectorShift);
  d_maxsectors = (d_filesize/d_sectorsize);
  d_ic = iconv_open("UTF-8", "UTF-16LE");

}

CompoundBinaryFile::~CompoundBinaryFile(){

   if (d_direntry) delete []d_direntry;
   delete d_ifstr;
   iconv_close(d_ic);
	
}


void CompoundBinaryFile::readdirchain(){

  unsigned int dirbytes = d_filesize - ((d_header._sectDirStart + 1) * d_sectorsize);
  d_maxdirentries = (static_cast<unsigned int>(dirbytes/d_sectorsize)) * (static_cast<unsigned int>(d_sectorsize/0x80));
  // if (d_header._csectMiniFat > 0){
  //   if (d_header._sectMiniFatStart > d_header._sectDirStart){
  //     d_maxdirentries = (d_header._sectMiniFatStart - d_header._sectDirStart) * 4;   
  //   } 
  // }
  d_direntry = new StructuredStorageDirectoryEntry[d_maxdirentries];
  for (unsigned int i = 0; i < d_maxdirentries/4; i++){
    readsector(d_header._sectDirStart+i, reinterpret_cast<unsigned char *>(&d_direntry[i*4]));
  }
}

void CompoundBinaryFile::readsector(unsigned int sector, unsigned char *buf){

  if (sector + 1 > d_maxsectors - 1){
    throw string("readsector out of range");
  }

  d_ifstr->seekg((sector + 1) * d_sectorsize, ios::beg);
  d_ifstr->read(reinterpret_cast<char *>(buf), d_sectorsize);
}

bool CompoundBinaryFile::getApplication(string &type){
 // this is some sort of hack to read the directoryentries if the tree structure is f*cked up.
  if (d_direntry == 0){
    readdirchain();
  }

  type = "";
  for (unsigned int currindex = 0; currindex < d_maxdirentries; currindex++){
    StructuredStorageDirectoryEntry *curr = &(d_direntry[currindex]);
    
    if (curr->_mse == STGTY_STREAM){
      if (d_ic != reinterpret_cast<iconv_t>(-1)){
	char outstr[64];
	char *outstart = &outstr[0];
	size_t outstrl = 64;
	char *strin = reinterpret_cast<char *>(&(curr->_ab[0]));
        size_t strinsize = 0;
        if (curr->_cb <= 32){
	  strinsize = (curr->_cb);
	} else continue;
	iconv(d_ic, 0, 0, 0, 0);
	if (iconv(d_ic, &strin, &strinsize, &outstart, &outstrl) == static_cast<size_t>(-1)) continue;
	
        if (strinsize == 0){
	  outstr[64 - outstrl - 1] = 0;
	  d_labels.push_back(string(outstr));
	  if ((string(outstr).compare(string("WordDocument"))) == 0){
	    d_knowntype = true;	   
	    type = "WordDocument";
	    return true;
	  } 
	  if ((string(outstr).compare(string("Workbook"))) == 0 || (string(outstr).compare(string("Book"))) == 0){
	    d_knowntype = true;	   
	    type = "Excel";
	    return true;
	  }
	  if ((string(outstr).compare(string("PowerPoint Document"))) == 0){
	    d_knowntype = true;  
	    type = "PowerPoint";
	    return true;
	    
	  }
	  if ((string(outstr).compare(string("VisioDocument"))) == 0){
	    d_knowntype = true;
	    type = "Visio";
	    return true;
	    
          }
	  
	  
	}
	
      }
    } 
  }
  
  type = "Unknown";
  return false;
  
}

void CompoundBinaryFile::parsetree(unsigned int currindex){
  if (currindex > d_maxdirentries){
    return;
  }

  if (d_beenhere.count(currindex) == 0){
    d_beenhere.insert(currindex);  
  } else {
    return;
  }

  StructuredStorageDirectoryEntry *curr = &(d_direntry[currindex]);

  switch (curr->_mse){
  case STGTY_INVALID:
    return;
  case STGTY_ROOT:
  // if this is the root or storage object then process child and right sibling, if any ...    
    if (curr->_sidChild != 0xffffffff){
      parsetree(curr->_sidChild);
    }
    return; 
  case STGTY_STORAGE:
    if (curr->_sidChild != 0xffffffff){
      parsetree(curr->_sidChild);
    }
    if (curr->_sidRightSib != 0xffffffff){
      parsetree(curr->_sidRightSib);
    }
    if (curr->_sidLeftSib != 0xffffffff){
      parsetree(curr->_sidLeftSib);
    }
    return;
  case STGTY_STREAM:
    // streams shouldn't have children but can have siblings
    // print label of stream
    if (curr->_cb != 0xffff){
      
      if (d_ic != reinterpret_cast<iconv_t>(-1)){
	char outstr[64];
	char *outstart = &outstr[0];
	size_t outstrl = 64;
	char *strin = reinterpret_cast<char *>(&(curr->_ab[0]));
	size_t strinsize = (curr->_cb);
	iconv(d_ic, 0, 0, 0, 0);
	iconv(d_ic, &strin, &strinsize, &outstart, &outstrl);

	if (strinsize == 0){
          outstr[64 - outstrl - 1] = 0;
          d_labels.push_back(string(outstr));

	} else {
	  switch(errno){
	  case E2BIG:
	    d_labels.push_back("E2BIG");
	    break;
	  case EILSEQ: 
	    d_labels.push_back("EILSEQ");
	    break;
	  case EINVAL: 
	    d_labels.push_back("EINVAL");
	    break;
	  }
	}
      }
    }
    if (curr->_sidRightSib != 0xffffffff){
      parsetree(curr->_sidRightSib);
    }
    if (curr->_sidLeftSib != 0xffffffff){
      parsetree(curr->_sidLeftSib);
    }
    break;
  default:
    break;
  }
}




string CompoundBinaryFile::getCLSID(){
  char clsidbuf[128];
  ostrstream clsid(clsidbuf,128);

  BYTE *clsarr = &((*d_direntry)._clsId[0]); 

  for (int i = 0; i < 16; i++){
    clsid << hex << static_cast<unsigned int>(clsarr[i]);
  } 
  
  clsid << '\0';

  return string(clsidbuf);

}

void CompoundBinaryFile::parseTree(){
   if (d_direntry == 0){
      readdirchain();
   }
   d_beenhere.clear();
   parsetree(0);
}


void CompoundBinaryFile::readfatchain(){

  FAT *fatarray = new FAT[d_header._csectFat];
  for (unsigned int i = 0; i < d_header._csectFat; i++){
    if (d_header._sectFat[i] != FREESECT){
      //cout << "FATSector " << i << " starts at offset 0x" << hex << ((d_header._sectFat[i] + 1 ) * 512)<< endl;
    }
    d_ifstr->seekg((d_header._sectFat[i] + 1 ) * 512, ios::beg);
    d_ifstr->read(reinterpret_cast<char *>(fatarray[i]), 512);
  }

}




int CompoundBinaryFile::getLabelCount(){
  return d_labels.size();
}

string CompoundBinaryFile::getLabel(int i){
  return d_labels[i];
}



void  CompoundBinaryFile::printHeader(){
  
  cout << "Byte order: 0x" << hex << d_header._uByteOrder << endl
       << d_header._csectFat << " Fat sectors in this file" << endl
       << d_header._sectFat[0] << " is the first FAT sector" << endl
       << d_header._csectMiniFat << " miniFats in this file" << endl
       << (d_header._sectMiniFatStart+1)*512 << " is the offset of the miniFAT chain" << endl
       << "DirStart Offset: "<< d_header._sectDirStart << "  0x" << hex << (d_header._sectDirStart +1) * 512 << endl
       << "SectorShift:0x" << d_header._uSectorShift << ", 0x" << d_header._uMiniSectorShift << endl ; 
}


void CompoundBinaryFile::printDirEntries(){


  for (unsigned int c = 0; c < d_maxdirentries; c++){
    StructuredStorageDirectoryEntry *curr = &(d_direntry[c]);
    cout << "--------------" << endl << "Node " << c << " (File offset: " << (d_header._sectDirStart +1) * 512 + (c * 128) << ")"<< endl ;
   
    if (curr->_mse == STGTY_STORAGE || curr->_mse == STGTY_STREAM || curr->_mse == STGTY_ROOT){
      cout << "Label: ";
      for (int i =0; i< curr->_cb; i++){
	
	//if (reinterpret_cast<char>(curr->_ab[i]) != 0) cout << reinterpret_cast<char>(curr->_ab[i]) ;
      };
      int stype = curr->_mse;
      
      cout << endl 
	   << "STGTY:" << hex << stype  << endl
	   <<  "Left sib: " <<  curr->_sidLeftSib << endl
	   << "Right sib: "<< curr->_sidRightSib << endl 
	   << "Child: " << hex << curr->_sidChild << endl;
    } else cout << "Invalid" << endl;
    cout << endl << "----------------" << endl; 
  }
  
  
}
