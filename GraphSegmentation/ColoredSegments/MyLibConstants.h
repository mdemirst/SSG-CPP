#ifndef MYLIB_CONSTANTS_H
#define MYLIB_CONSTANTS_H

using namespace cv;
//  File Name : LibConstants.hpp    Purpose : Global Constants for Lib Utils

//namespace LibConstants
//{

  const int pyrHeight	=	240;
  const int pyrWidth	=	320;
  const int hypo		=	400;
  const int myzero		=	0;
  const int background = 100;
  const int UpObjTh	 = pyrHeight/3;
  const int MidObjTh = 2*(pyrHeight/3);
  const int TOTAL_COLOR_Nr = 25;
  const int ALONE_PxNr = 1;
  const int MAX_PxNr_SMALL_AREA =70; //N_1
  const int MIN_PxNr_BIG_AREA =40;//pyrHeight*pyrWidth*0.01200; //0.01320 OF TOTAL PIXEL NUMBER //N_2
  const int MAX_EXP_NrOf_LABELS = 8000;
  int dilation_size = 3; // adjust with you application
  Mat dilation_element = getStructuringElement( MORPH_ELLIPSE,Size( 2*dilation_size + 1, 2*dilation_size+1 ),Point( dilation_size, dilation_size ) );
  const string name = "out-";
  const string imtype = ".jpg";
  const String imagename1 = "rgbImage_" ;
  const string txttype = ".txt";
  const String quant = "-quantized";
  const char *cstr = imagename1.c_str();


  //const char imgname[50]= "rgbImage_060";
  //const string type = ".jpeg";
  //const String imagename = "Img2" ; 
//}
#endif

// ADD TO .CPP --->> int dilation_size = LibConstants::dilation_size; 
//IF
//  namespace LibConstants
//					{.
//					...
//					}
// USED.
