

#include"imageProcess.h"

#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<sstream>
#include<math.h>

#include <opencv2/opencv.hpp>
//#include <opencv2/core/cuda.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


using namespace cv;

//used for label creation
int kernel_size = 3;

ImageProcess::ImageProcess(const Mat& in, Mat& out)
{
	
	inputImage = in;
	outputImage = out;
	
}

ImageProcess::ImageProcess() : rawCornerPoints(4), rectangleCornerPoints(4)	{

	HORIZONTAL = 0;
	VERTICAL = 1;
}

ImageProcess::~ImageProcess()
{

}

//This is not used but might be handy in the future
Mat ImageProcess::convertToGray(const Mat& img)	{
	
	Mat temp = img.clone();
	cv::cvtColor(temp, temp, COLOR_BGR2GRAY); //convert to grayscale
	blur(temp, temp, Size(kernel_size, kernel_size), Point(-1, -1));	//apply blur with kernel of size(3,3) to smoothen and remove noise
	GaussianBlur(temp, temp, Size(kernel_size, kernel_size), 0, 0, BORDER_DEFAULT);
	return temp;
}

//Show a given Mat object
void ImageProcess::showImage(Mat& img, String nameOfWindow)	{

	namedWindow(nameOfWindow, WINDOW_NORMAL);
	imshow(nameOfWindow, img);

}

//checks whether we actually detected the reference paper
bool ImageProcess::isDetectionTrue(vector<Point2f> vertices)	{

	bool detected = false;

	float line1, line2;

	//calculate line distances
	line1 = pow(vertices[0].x - vertices[3].x, 2) + pow(vertices[0].y - vertices[3].y, 2);
	line1 = sqrt(line1);
	cout << "line1 : " << line1 << endl;

	line2 = pow(vertices[1].x - vertices[2].x, 2) + pow(vertices[1].y - vertices[2].y, 2);
	line2 = sqrt(line2);
	cout << "line2 : " << line2 << endl;

	float min, max;
	
	//find minimum
	if (line1 <= line2) {
		min = line1;
		max = line2;
	} else {
		min = line2;
		max = line1;
	}

	float ratio = 0;
	ratio = min/max;
	
	if (ratio > 0.8 && (estWidthOfObject > 50) && (estHeightOfObject > 50))	{
		detected = true;
	} else detected = false;
	
	cout << "detected : " << detected << endl;
	return detected;
}


/**
* Helper function to display text in the center of a contour
*/
void ImageProcess::setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour)
{
	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
	cv::Rect r = cv::boundingRect(contour);

	cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255, 255, 255), CV_FILLED);
	cv::putText(im, label, pt, fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
}


//source is the object to detect, scene is the image that contains the reference object
void ImageProcess::objectDetect(const Mat& source, const Mat& input_scene)	{
	  
  //1. Detect the keypoints using SURF (Speeded Up Robust Features) Detector
  int minHessian = 400; //Hessian threshold 

  SurfFeatureDetector detector( minHessian ); //create our detector

  std::vector<KeyPoint> keypoints_object, keypoints_scene; //create keypoints containers

  Mat img_object = source; //our reference object

  Mat img_scene = input_scene; //object with the reference object to detect (i.e. cropped window)

  //we set the overall width and height of window in pixels
  setWindowDimensions(img_scene);
  
  //saves the keypoints from our reference object
  detector.detect( img_object, keypoints_object );
  
  //saves the keypoints from our scene object (cropped window that has reference image)
  detector.detect( img_scene, keypoints_scene );

  //2. Calculate descriptors (feature vectors)
  SurfDescriptorExtractor extractor; //need to extract our keypoints (features)

  Mat descriptors_object, descriptors_scene; //create our descriptors, these are vectors that describe
						//our keypoints

  //we compute to obtain the descriptors for both our object and scene
  extractor.compute( img_object, keypoints_object, descriptors_object );
  extractor.compute( img_scene, keypoints_scene, descriptors_scene );

  //3. Match our descriptor vectors using FLANN matcher (Fast Library for Approximate Nearest Neighbor)
  FlannBasedMatcher matcher; //create our matcher
  std::vector< DMatch > matches; //create container for our matches
  matcher.match( descriptors_object, descriptors_scene, matches ); //match our descriptors within the two Mat objects

  double max_dist = 0; double min_dist = 100; //set our default max and min distances between keypoints

  //Calculate max and min distances between keypoints
  for( int i = 0; i < descriptors_object.rows; i++ )
  { double dist = matches[i].distance;
    if( dist < min_dist ) min_dist = dist;
    if( dist > max_dist ) max_dist = dist;
  }

  //print to check if we get decent max and min values
  printf("-- Max dist : %f \n", max_dist );
  printf("-- Min dist : %f \n", min_dist );

  //Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
  std::vector< DMatch > good_matches;

  for( int i = 0; i < descriptors_object.rows; i++ )
  { if( matches[i].distance < 3*min_dist )
     { good_matches.push_back( matches[i]); }
  }

  //This is to show the matching between two images
  Mat img_matches;
  drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
               good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

  //Localize the object
  std::vector<Point2f> obj;
  std::vector<Point2f> scene;

  for( int i = 0; i < good_matches.size(); i++ )
  {
    //Get the keypoints from the good matches
    obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
    scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
  }

  //Use homography to find perspective transformation
  Mat H = findHomography( obj, scene, CV_RANSAC );

  //Get the corners from the object image ( the object to be "detected" )
  std::vector<Point2f> obj_corners(4);
  obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( img_object.cols, 0 );
  obj_corners[2] = cvPoint( img_object.cols, img_object.rows ); obj_corners[3] = cvPoint( 0, img_object.rows );
  std::vector<Point2f> scene_corners(4);
 
  //Calculates a perspective transform from four pairs of the corresponding points.
  perspectiveTransform( obj_corners, scene_corners, H);

	setOutputImage(img_matches);

  std::vector<Point2f> final_corners(4); //corner points container
	
	//save final point coordinates
	rawCornerPoints[0] = scene_corners[0] + Point2f( img_object.cols, 0);
	rawCornerPoints[1] = scene_corners[1] + Point2f( img_object.cols, 0);
	rawCornerPoints[2] = scene_corners[2] + Point2f( img_object.cols, 0);
	rawCornerPoints[3] = scene_corners[3] + Point2f( img_object.cols, 0);
	
	
	//(top_left, lower_right)
	//This one uses only two points of the saved points
	cv::Rect objectRectangle(rawCornerPoints[0], rawCornerPoints[2]);
	

	//void rectangle(Mat& img, Point pt1, Point pt2, const Scalar& color, int thickness=1, int lineType=8, int shift=0)
	cv:rectangle(img_matches, rawCornerPoints[0], rawCornerPoints[2], Scalar(255,0,0),3,8,0); //draw rectangle
	
	//width and height of the paper in pixels
	estWidthOfObject = objectRectangle.width;
	estHeightOfObject = objectRectangle.height;
	
	//This avoids false positive detection
	isDetected = isDetectionTrue(rawCornerPoints);	
	
	//reference detection only
	//we set the width and height of the reference object
	widthOfPaperInPixels = estWidthOfObject;
	heightOfPaperInPixels = estHeightOfObject;

	//used only for reference detection
	if (estWidthOfObject > estHeightOfObject) paperOrientation = HORIZONTAL;
	else paperOrientation = VERTICAL;

	std::cout<< "Reference width: " << estWidthOfObject << endl;
	std::cout<< "Reference height: " << estHeightOfObject << endl;

	printOrientation();
  	
  	//Show detected matches, not needed for the overall system	
	showImage(img_matches, "Good Matches & Object detection");

	
}// end objectDetect()

void ImageProcess::printOrientation()	{
	switch (paperOrientation)	{
	case 0: std::cout << "Orientation HORIZONTAL" << endl; break;
	case 1: std::cout << "Orientation VERTICAL" << endl; break;
	default: break;
	}

}

/*
	Getters
*/
float ImageProcess::getWidthOfWindowInPixels()	{
	return widthOfWindowInPixels;
}

float ImageProcess::getHeightOfWindowInPixels()	{
	return heightOfWindowInPixels;
}

float ImageProcess::getWidthOfWindowCm()	{

  if (widthOfPaperInPixels == 0) return 0;
	//calculate the actual width of the window in centimeters
	// 8.5 inches = 21.59
	// 11 inches = 27.94
	float x;
	
	if (paperOrientation == VERTICAL)	{
		x = (float) (21.59 * widthOfWindowInPixels);
		x = (float) (x / widthOfPaperInPixels);
	}
	else if (paperOrientation == HORIZONTAL)	{
		x = (float) (27.94 * widthOfWindowInPixels);
		x = (float) (x / widthOfPaperInPixels);
	}


	widthOfWindowCm = x;
	return widthOfWindowCm;
}

float ImageProcess::getHeightOfWindowCm()	{

  if ( heightOfPaperInPixels == 0) return 0;
	//calculate the actual height of the window in centimeters
	// 8.5 inches = 21.59
	// 11 inches = 27.94
	float y;
	if (paperOrientation == VERTICAL)	{
		y = (float) (27.94 * heightOfWindowInPixels);
		y = (float) (y / heightOfPaperInPixels);
	}
	else if (paperOrientation == HORIZONTAL)	{
		y = (float) (21.59 * heightOfWindowInPixels);
		y = (float) (y / heightOfPaperInPixels);
	}

	heightOfWindowCm = y;
	return heightOfWindowCm;
}


void ImageProcess::setWindowDimensions(Mat& img)	{

	
	//cols is width in pixels
	setWidthOfWindowInPixels(img.cols); 

	//rows is height in pixels
	setHeightOfWindowInPixels(img.rows);				
}

/*
	Setters
*/
void ImageProcess::setWidthOfWindowInPixels(int w)	{
	
	widthOfWindowInPixels = w;
	cout << "Window width in pixels " << widthOfWindowInPixels << endl;
}

void ImageProcess::setHeightOfWindowInPixels(int h)	{
	heightOfWindowInPixels = h;
	cout << "Window height in pixels " << heightOfWindowInPixels << endl;
}


void ImageProcess::setInputImage(Mat& image)	{
	inputImage = image;
}

void ImageProcess::setOutputImage(Mat& image)	{
	image.copyTo(outputImage);	
	
}




