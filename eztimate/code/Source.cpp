/*
Project 195 SJSU
EZtimate Finder Subsystem
Members:
Yohan Bouvron
Marvin Flores
Brian Nguyen
description:
*/

/*	VERSION 0.01 07/20/2015
	- added foundation source code
	- added convertBW function
	- added to be able to enter two arguments
	- now able to ouput xml file
	- now able to calculate process time
	
	VERSION 0.02 07/26/2015
	- code overhaul, created classes for code clarity and easy debugging

	input that works ; no image overlaying
	./Source source2.jpg t28-33a.jpg out-new.jpg



	11/01/2015
	TO DO:

		reference object = EZ logo

		//Brian
		fix ImageOverlay; implement using ImageProcess functions ( getRawCornerPoints() )
		use argc[3] for the original-image container

		//Yohan
		update DataHandler for image/file Saving and Handling;

		//Marvin
		*resize images but respect their aspect ratio; currently only works when images are in ~1024x1024 pixels
			-->This can be done using script, no need to do it in our code
		*try other reference object, currently uses the EZ logo
			-->EZ logo is the final reference pattern, other patterns did not work



	11/12/2015
	TO DO:

		reference object = EZ logo

		Working version
		See readme() for the usage
		image_name.json is the file that contains the image_overlay filename
			this file would be received from the SE group

		output file is in json format
		file name: dimensions.json

	11/15/2015
	VERSION 1.1

		implemented dataHandler class
		using ez_v2.jpg as reference object

*/


#include <iostream>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"


#include  "opencv2/imgproc/imgproc.hpp"

#include "imageProcess.h"
#include "imageOverlay.h"
#include "dataHandler.h"

#include "imageProcess.cpp"
#include "imageOverlay.cpp"
#include "dataHandler.cpp"

// Current Version 1.0

int CURRENT_THRESH = 220;
int MAX_THRESH = 250;	//255: white, 0 : black



using namespace cv;

void readme();

/** @function main */
int main( int argc, char** argv )
{

  ImageProcess detectReference;
  if( argc < 4 || argc > 6 )  { 
	readme(); 
  	return -1; 
  }
   else if (argv[1] == "--help") {
	readme();
	return -1; 
   }

  //create data handler object
  DataHandler handler("dimensions.json");

  Mat img_object = imread( argv[1], CV_LOAD_IMAGE_COLOR );
  Mat img_scene = imread( argv[2], CV_LOAD_IMAGE_COLOR );
  
  Mat img_orig; //declare but do not use yet

  string image_overlay_filename = "";

  if ( argc == 6) { 
	Mat img_output = imread( argv[4], CV_LOAD_IMAGE_COLOR ); 
	
	//Brian, this will be the original image
	//I put it in grayscaled, change it according to your implementation
	//This will only work if the number of input images is 4
	// look at the readme() function for info		
	img_orig = imread(argv[3], CV_LOAD_IMAGE_COLOR);

	//retrieve the image overlay filename	
	image_overlay_filename = handler.loadImage(argv[5]);
	//cout << "Hello " << image_overlay_filename << endl;
	 
  }

  else { 

	Mat img_output = imread( argv[3], CV_LOAD_IMAGE_COLOR ); 
		
  }


   //for image overlaying
  Mat newWindow = imread(image_overlay_filename, -1);
  


  if( !img_object.data || !img_scene.data )
  { std::cout<< " --(!) Error reading images " << std::endl; return -1; }

  //This is used for object detection
  detectReference.objectDetect(img_object, img_scene);

  //Prints the output
  std::cout <<"Estimated Window width: " << (int) detectReference.getWidthOfWindowCm() << " cm" << endl;
  std::cout <<"Estimated Window height: " << (int) detectReference.getHeightOfWindowCm() << " cm" << endl;

	/*sample for image overlaying */
	/*
		cropped_window --> window to be detected (from grabcut team)
		original_window --> original window, the image we want to paste the new window into
	*/
	// Mat cropped_window, original_window;
	// ImageProcess detectWindow;
	// detectWindow.objectDetect(cropped_window, original_window);
	
	/* get the four vertices as Point2f vector */
	//std::vector<Point2f> testVector = detectReference.getRawCornerPoints();
	
	/* call your ImageOverlay for overlaying implementation */
	
	//This block shows the getRawCornerPoints() in action		
	//Mat test_image = detectReference.getOutputImage();
	//std::vector<Point2f> testVector = detectReference.getRawCornerPoints();

	//-- Draw lines between the corners (the mapped object in the scene - image_2 )
  	//cv::line( test_image, testVector[0], testVector[1], Scalar(0, 255, 0), 4 );
	//cv::line( test_image, testVector[1], testVector[2], Scalar(0, 255, 0), 4 );
	//cv::line( test_image, testVector[2], testVector[3], Scalar(0, 255, 0), 4 );
	//cv::line( test_image, testVector[3], testVector[0], Scalar(0, 255, 0), 4 );

	//detectReference.showImage(test_image, "test image");


	//OverlayCode
	ImageOverlay Overlay(img_orig, newWindow, img_scene);
	Overlay.cornerForeground();
	Overlay.cornerBackground();	
	Overlay.find_homography();
	Overlay.warp_perspective();
	Mat final_image = Overlay.overlayImage(); 
 	//End Overlay Code

	float widthInCm;
	float heightInCm;
	
	if (detectReference.getIsPaperDetected()) {
	
	widthInCm = detectReference.getWidthOfWindowCm();
	heightInCm = detectReference.getHeightOfWindowCm();	

	} else {

	widthInCm = 0.0;
	heightInCm = 0.0;

	}
	

	handler.saveDimensions(widthInCm, heightInCm );

	handler.saveImage(final_image, argv[4]);
	//imwrite("new_output.jpg", final_image);


  waitKey(0);
  return 0;
  }

  /** @function readme */
  void readme()
  { 	std::cout << "\n\n Usage for Object detection and image overlay:\n" 
		  << "./Source <object-source.jpg> <window-with-object-source> <window-original-image> <output jpg> <overlay_image.json>"	 
		  << std::endl ;
	std::cout << "all parameters should be in .jpg format except the final argument which should be in .json format" << std::endl; }



