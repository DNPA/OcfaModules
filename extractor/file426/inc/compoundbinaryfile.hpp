
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

#include<iconv.h>
#include<iostream>
#include<fstream>
#include<string>
#include<set>
#include<vector>

#ifndef INCLUDED_COMPOUNDBINARYFILE_H
#define INCLUDED_COMPOUNDBINARYFILE_H

using namespace std;

typedef unsigned long ULONG; // 4 bytes
typedef unsigned short USHORT; // 2 bytes
typedef short OFFSET; // 2 bytes
typedef ULONG SECT; // 4 bytes
typedef ULONG FSINDEX; // 4 bytes
typedef USHORT FSOFFSET; // 2 bytes
typedef USHORT WCHAR; // 2 bytes
typedef ULONG DFSIGNATURE; // 4 bytes
typedef unsigned char BYTE; // 1 byte
typedef unsigned short WORD; // 2 bytes
typedef unsigned long DWORD; // 4 bytes
typedef ULONG SID; // 4 bytes
typedef BYTE GUID[16]; // ozz def
typedef GUID CLSID; // 16 bytes
typedef SECT FAT[128]; // ozz def

// 64-bit value representing number of 100 nanoseconds since January 1, 1601
typedef struct tagFILETIME { 
DWORD dwLowDateTime;
DWORD dwHighDateTime;
} FILETIME;


const SECT MAXREGSECT = 0xFFFFFFFA; // maximum SECT
const SECT DIFSECT = 0xFFFFFFFC; // denotes a DIFAT sector in a FAT
const SECT FATSECT = 0xFFFFFFFD; // denotes a FAT sector in a FAT
const SECT ENDOFCHAIN = 0xFFFFFFFE; // end of a virtual stream chain
const SECT FREESECT = 0xFFFFFFFF; // unallocated sector
const SID MAXREGSID = 0xFFFFFFFA; // maximum directory entry ID
const SID NOSTREAM = 0xFFFFFFFF; // unallocated directory entry

struct StructuredStorageHeader { // [offset from start (bytes), length (bytes)]
  BYTE _abSig[8];                // [00H,08] {0xd0, 0xcf, 0x11, 0xe0, 0xa1, 0xb1, 
  // 0x1a, 0xe1} for current version
CLSID _clsid; // [08H,16] reserved must be zero (WriteClassStg/
// GetClassFile uses root directory class id)
USHORT _uMinorVersion; // [18H,02] minor version of the format: 33 is 
// written by reference implementation
USHORT _uDllVersion; // [1AH,02] major version of the dll/format: 3 for
// 512-byte sectors, 4 for 4 KB sectors
USHORT _uByteOrder; // [1CH,02] 0xFFFE: indicates Intel byte-ordering
USHORT _uSectorShift; // [1EH,02] size of sectors in power-of-two; 
// typically 9 indicating 512-byte sectors
USHORT _uMiniSectorShift; // [20H,02] size of mini-sectors in power-of-two; 
// typically 6 indicating 64-byte mini-sectors
USHORT _usReserved; // [22H,02] reserved, must be zero
ULONG _ulReserved1; // [24H,04] reserved, must be zero
FSINDEX _csectDir; // [28H,04] must be zero for 512-byte sectors,
// number of SECTs in directory chain for 4 KB
// sectors
FSINDEX _csectFat; // [2CH,04] number of SECTs in the FAT chain
SECT _sectDirStart; // [30H,04] first SECT in the directory chain
DFSIGNATURE _signature; // [34H,04] signature used for transactions; must
// be zero. The reference implementation
// does not support transactions
ULONG _ulMiniSectorCutoff; // [38H,04] maximum size for a mini stream; 
// typically 4096 bytes
SECT _sectMiniFatStart; // [3CH,04] first SECT in the MiniFAT chain
FSINDEX _csectMiniFat; // [40H,04] number of SECTs in the MiniFAT chain
SECT _sectDifStart; // [44H,04] first SECT in the DIFAT chain
FSINDEX _csectDif; // [48H,04] number of SECTs in the DIFAT chain
SECT _sectFat[109]; // [4CH,436] the SECTs of first 109 FAT sectors
};

typedef enum tagSTGTY {
STGTY_INVALID = 0, // unknown storage type
STGTY_STORAGE = 1, // element is a storage object
STGTY_STREAM = 2, // element is a stream object
STGTY_LOCKBYTES = 3, // element is an ILockBytes object
STGTY_PROPERTY = 4, // element is an IPropertyStorage object
STGTY_ROOT = 5 // element is a root storage
} STGTY;

typedef enum tagDECOLOR {
DE_RED = 0,
DE_BLACK = 1
} DECOLOR;
// [offset from start (bytes), length (bytes)]
struct StructuredStorageDirectoryEntry { 
WCHAR _ab[32]; // [00H,64] 64 bytes. The element name in Unicode, padded
// with zeros to fill this byte array. Terminating 
// Unicode NULL is required.
WORD _cb; // [40H,02] Length of the Element name in bytes, 
// including the Unicode NULL
BYTE _mse; // [42H,01] Type of object. Value taken from the 
// STGTY enumeration
BYTE _bflags; // [43H,01] Value taken from DECOLOR enumeration
SID _sidLeftSib; // [44H,04] SID of the left-sibling of this entry 
// in the directory tree
SID _sidRightSib; // [48H,04] SID of the right-sibling of this entry 
// in the directory tree
SID _sidChild; // [4CH,04] SID of the child acting as the root of all 
// the children of this element 
// (if _mse=STGTY_STORAGE or STGTY_ROOT)
CLSID _clsId; // [50H,16] CLSID of this storage 
// (if _mse=STGTY_STORAGE or STGTY_ROOT)
DWORD _dwUserFlags; // [60H,04] User flags of this storage 
// (if _mse=STGTY_STORAGE or STGTY_ROOT)
FILETIME _time[2]; // [64H,16] Create/Modify time-stamps 
// (if _mse=STGTY_STORAGE)
SECT _sectStart; // [74H,04] starting SECT of the stream 
// (if _mse=STGTY_STREAM)
ULONG _ulSizeLow; // [78H,04] size of stream in bytes 
// (if _mse=STGTY_STREAM)
ULONG _ulSizeHigh; // [7CH,02] must be zero for 512-byte sectors, 
// high part of 64-bit size for 4 KB sectors
};

class CompoundBinaryFile {
public:
  CompoundBinaryFile(std::string filename);
  ~CompoundBinaryFile();
  void printHeader();
  void printDirEntries();
  void parseTree();
  string getCLSID(); 
  string getLabel(int i);
  int getLabelCount(); 
  bool getApplication(string &type);
  bool knownType(){return d_knowntype;};
protected:
  void readsector(unsigned int sector, unsigned char *buf);
  void readfatchain();
  void readdirchain();
  void parsetree(unsigned int currindex);
  void scandir();
private:
  StructuredStorageHeader d_header;
  StructuredStorageDirectoryEntry *d_direntry;  
  ifstream *d_ifstr;
  unsigned int d_filesize;
  string d_filename;
  set<unsigned int> d_beenhere;
  vector<string> d_labels; 
  iconv_t d_ic;
  bool d_knowntype;
  unsigned int d_maxdirentries;
  unsigned int d_maxsectors;
  unsigned int d_sectorsize;
};


#endif
