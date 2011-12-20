#include<string>
#include<iostream>
#include<stdlib.h>
#include"store/EvidenceStoreEntity.hpp"
#include"misc/OcfaLogger.hpp"
#include "fivesfaces.hpp"
#include<opencv/cv.h>
#include<opencv/highgui.h>
using namespace ocfa::module;
using namespace ocfa::misc;

Fivesfaces::Fivesfaces(): EvidenceFileAccessor("fivesfaces","core")
{
   // put thirdparty-library initializations here ...
    storage = cvCreateMemStorage(0);
    char *ocfaroot = getenv("OCFAROOT");
    string cascadepath;
    if (ocfaroot){
       cascadepath = string(ocfaroot) + string("/etc/haarcascade_frontalface_default.xml");
    } else {
      throw OcfaException("OCFAROOT not set"); 
    }
    cascade = (CvHaarClassifierCascade*)cvLoad( cascadepath.c_str(), 0, 0, 0 );
    
    if (!cascade){
       throw OcfaException("Failed to load cascadefile");
    }

}

Fivesfaces::~Fivesfaces()
{
   // clean up here ...
}

void Fivesfaces::processEvidence()
{
   ocfa::store::EvidenceStoreEntity *evidence = fetchEvidenceStoreObject();
   if(evidence){
   std::string filename = evidence->getAsFilePath();
   getLogStream(LOG_DEBUG) << "Processing " << filename << endl;

   // do whatever you want to do with the evidence
    
    // Load the image from that filename
    IplImage* image = cvLoadImage( filename.c_str(),1 );
    if (image){
       Fives_DetectFaces( image );
       cvReleaseImage(&image);
    } else {
       getLogStream(LOG_WARNING) << "Could not load image " << filename << endl;
       setMeta("valid_img", "false");   
    }

   }else{
	getLogStream(LOG_ERR)<<"no evidence provided"<<endl;
   }

}


int main(int argc, char *argv[])
{
  Fivesfaces *pFivesfaces;
  try {
    pFivesfaces = new Fivesfaces();
    pFivesfaces->run();
  } catch(OcfaException &e){
    std::cerr << e.what() << std::endl;
    return 1;
  }
  delete pFivesfaces;
  return 0;
}

void Fivesfaces::Fives_DetectFaces( IplImage* img )
{
    int scale = 1;
    int scale4r = 3;

    // Create a new image based on the input image
    IplImage* temp = cvCreateImage( cvSize(img->width/scale4r ,img->height/scale4r ), 8, 3 );

    // Create two points to represent the face locations
    CvPoint pt1, pt2;

    // Clear the memory storage which was used before
    cvClearMemStorage( storage );

    // Find whether the cascade is loaded, to find the faces. If yes, then:
    if( cascade )
    {

        // There can be more than one face in an image. So create a growable sequence of faces.
        // Detect the objects and store them in the sequence
        CvSeq* faces = cvHaarDetectObjects( img, cascade, storage,
                                            1.1, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(40, 40) );

        if (faces->total == 0){
           getLogStream(LOG_DEBUG) << "No faces found" << endl;
        } else {
          setMeta("nmbroffaces", faces->total);

        // prepare the columnnames of the metadata table
        const char *colnames[] = {"pt1x", "pt1y", "pt2x", "pt2y", 0};
        unsigned int colcount = 4;
        ArrayMetaValue *amvcolnames = new ArrayMetaValue(colcount); 
        for (unsigned int i = 0; i < colcount; i++){
            Scalar col(colnames[i]);
            amvcolnames->addMetaValue(col);
        }
        // Instantiate metadata table with column names
        TableMetaValue tmvcoordinates(&amvcolnames);
        
        // Loop the number of faces found.
        for(unsigned int i = 0; i < (unsigned int)(faces ? faces->total : 0); i++ )
        {
           // Create a new rectangle for drawing the face
            CvRect* r = (CvRect*)cvGetSeqElem( faces, i );

            // Find the dimensions of the face,and scale it if necessary
            ArrayMetaValue *row = new ArrayMetaValue(colcount); 
            Scalar pt1x(r->x*scale);
            Scalar pt1y(r->y*scale);
            Scalar pt2x((r->x+r->width)*scale);
            Scalar pt2y((r->x+r->width)*scale);
            row->addMetaValue(pt1x);
            row->addMetaValue(pt1y);
            row->addMetaValue(pt2x);
            row->addMetaValue(pt2y);
            tmvcoordinates.addRow(&row);
            
            getLogStream(LOG_DEBUG) << "Face detected NR:" << (i+1) 
                                    << " TopLeft X: " << r->x*scale 
                                    << " TopLeft Y: "  << r->y*scale 
                                    << " Width: " << (r->width)*scale 
                                    << " Height: " << (r->height)*scale << endl;
        }
         setMeta("facecoordinates",tmvcoordinates);
        }
    }

    cvReleaseImage( &temp );
}
