
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

#include "../inc/exif.hpp"
#include "../../../binaries.hpp"
using namespace std;
using namespace ocfa::misc;
using namespace ocfa::evidence;

/**
 * Exif constructor
 */
Exif::Exif() 
         : ocfa::facade::EvidenceFileAccessor("exif","default"),
	   szCommand(BINARY_EXIFTAGS) {

  //test presence of exiftags on system and execute persmissions
  if(access(szCommand.c_str(),X_OK)!=0) {
	
     ocfaLog(LOG_ERR,"Exiftags executable not found on system");
     throw OcfaException("Exiftags executable not found on system", this);
  }
}

/**
 * Exif destructor
 */
Exif::~Exif() {
}

/**
 * Exif processEvidence
 */
void Exif::processEvidence(){
  string         szDoIt;
  FILE          *fpExiftags;
  char           szLine[MAX_LINE_LENGTH];
  int            rval=0; //return value of pclose
  
  ocfa::store::EvidenceStoreEntity* evidence = fetchEvidenceStoreObject();
  if(evidence != 0) {
     string szFilePath = evidence->getAsFilePath();
     szDoIt = szCommand + " " + szFilePath + " 2>&1";
     if( (fpExiftags = popen(szDoIt.c_str(), "r") ) != NULL) {
        while(fgets(szLine, MAX_LINE_LENGTH-1, fpExiftags) !=NULL){ 
	   /* read while not end of pipe */
           processLine(szLine);
        };
        rval = pclose(fpExiftags);
	
        if(rval == -1) {
           logModule(LOG_ERR, string("ERROR return value pclose"));
           //setMeta("brokenmodule", Scalar(-1));
        }
     }
     else {
        getLogStream(LOG_WARNING) << "Not able to spawn " << szDoIt << "\n";
     }
     delete evidence;
  } //evidence!=0
  else {
     getLogStream(LOG_WARNING) << "No evidence found" ;
  }
} //processEvidence


void Exif::processLine(char *inLine)
{
  char             *pString, 
		   *szTemp;
  long int         dwValue;
  bool             parsed=false;
  static string    TimeSource = "CAMERA_NONAME";


  if(strstr(inLine, "Equipment Make") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    setMeta("CameraBrand", ocfa::misc::Scalar(pString,"LATIN1"));
    parsed=true;
  }

  if(strstr(inLine, "Camera Model") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL) 
    {
       do pString++; while(*pString == 32);
       
    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    setMeta("CameraModel", ocfa::misc::Scalar(pString,"LATIN1"));
    TimeSource = pString;
    parsed=true;
  }

  if(strstr(inLine, "Camera Software") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    setMeta("CameraSoftware", ocfa::misc::Scalar(pString,"LATIN1"));
    parsed=true;
  }

  if(strstr(inLine, "Owner Name") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    setMeta("OwnerName", ocfa::misc::Scalar(pString,"LATIN1"));
    parsed=true;
  }

  if(strstr(inLine, "Image Orientation") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    setMeta("ImageOrientation", ocfa::misc::Scalar(pString));
    parsed=true;
  }

  if(strstr(inLine, "Horizontal Resolution") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    setMeta("HorizontalResolution", ocfa::misc::Scalar(pString));
    parsed=true;
  }

  if(strstr(inLine, "Vertical Resolution") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    setMeta("VerticalResolution", ocfa::misc::Scalar(pString));
    parsed=true;
  }

  if(strstr(inLine, "Image Created") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    /*  We expect here: 'yyyy:mm:dd hh:mm:ss'
	This should be converted to something like the ISO8601
	'yyyy-mm-dd-ddThh:mm:ss' */
    const DateTime *dt = new DateTime(DateTime::translate(pString), TimeSource);
    //MetaValue *sv = new ScalarMetaValue(Scalar(&dt));
    setMeta(string("dtPictureTaken"),Scalar(&dt));
    TimeSource = "CAMERA_NONAME";
    parsed=true;
  }

  if(strstr(inLine, "Exposure Time") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    setMeta("ExposureTime", ocfa::misc::Scalar(pString));
    parsed=true;
  }

  if(strstr(inLine, "F-Number") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    setMeta("FNummer", ocfa::misc::Scalar(pString));
    parsed=true;
  }

  if(strstr(inLine, "Exposure Program") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    setMeta("ExposureProgram", ocfa::misc::Scalar(pString));
    parsed=true;
  }

  if(strstr(inLine, "ISO Speed Rating") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    dwValue = strtol(pString, &szTemp , 10);
    setMeta("ISORating", ocfa::misc::Scalar(dwValue));
    parsed=true;
  }

  if(strstr(inLine, "Exposure Bias") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    setMeta("ExposureBias", ocfa::misc::Scalar(pString));
    parsed=true;
  }

  if(strstr(inLine, "Metering Mode") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    setMeta("MeteringMode", ocfa::misc::Scalar(pString));
    parsed=true;
  }

  if(strstr(inLine, "Light Source") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    setMeta("LightSource", ocfa::misc::Scalar(pString));
    parsed=true;
  }

  if(strstr(inLine, "Flash:") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    setMeta("Flash", ocfa::misc::Scalar(pString));
    parsed=true;
  }

  if(strstr(inLine, "Focal Length") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    setMeta("FocalLength", ocfa::misc::Scalar(pString));
    parsed=true;
  }

  if(strstr(inLine, "Color Space Information") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    setMeta("ColorSpaceInformation", ocfa::misc::Scalar(pString));
    parsed=true;
  }

  if(strstr(inLine, "Image Width") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/

    dwValue = strtol(pString, &szTemp , 10);
    setMeta("Width", ocfa::misc::Scalar(dwValue));
    parsed=true;
  }

  if(strstr(inLine, "Image Height") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    dwValue = strtol(pString, &szTemp , 10);
    setMeta("Height", ocfa::misc::Scalar(dwValue));
    parsed=true;
  }

  if(strstr(inLine, "Compression Setting") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    setMeta("CompressionSetting", ocfa::misc::Scalar(pString));
    parsed=true;
  }

  if(strstr(inLine, "Macro Mode") != NULL)
  { /*substring found*/
    pString = strchr(inLine , DELIMITER);
    if(pString != NULL)
    {
       do pString++; while(*pString == 32);

    }
    if((szTemp = strchr(pString , 10) ) != NULL) *szTemp = 0; /*replace LF*/
    if((szTemp = strchr(pString , 13) ) != NULL) *szTemp = 0; /*replace CR*/
    setMeta("MacroMode", ocfa::misc::Scalar(pString));
    parsed=true;
  }
  
  if (!parsed) {
     logEvidence(LOG_ERR,inLine);
  }
} /*end function void processLine(char *inLine)*/


int main(int, char *[]) {
  Exif *e=0;
  try {
     e = new Exif();
     e->run();
  }
  catch (OcfaException &ex){

    ex.logWhat();
    ocfa::misc::OcfaLogger::Instance()->syslog(LOG_ERR, "ERROR") << ex.what() << endl;

 }	
  catch(...) {
    ocfa::misc::OcfaLogger::Instance()->syslog(LOG_ERR, "ERROR") << "Ging fout!\n";
  }
  delete e;
  return 1;

}

