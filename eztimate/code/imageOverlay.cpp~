
#include"imageOverlay.h"

#include<iostream>
#include <opencv2/opencv.hpp>
//#include <opencv2/core/cuda.hpp>


using namespace std;
using namespace cv;
//using namespace cv::cuda;

ImageOverlay::ImageOverlay(Mat back, Mat front, Mat cropped)	{
	background = back;
	foreground = front;
	window_cropped = cropped;
}

ImageOverlay::~ImageOverlay()	{

}

void ImageOverlay::cornerForeground()
{
	// Push the 4 corners of the new window as the 4 points for correspondence to calculate homography.
	FG.push_back(Point2f(float(0),float(0)));				//top left corner	
	FG.push_back(Point2f(float(0),float(foreground.rows)));			//bot left corner
	FG.push_back(Point2f(float(foreground.cols),float(foreground.rows)));	//bot right corner
	FG.push_back(Point2f(float(foreground.cols),float(0)));			//top right corner
}

void ImageOverlay::cornerBackground()
{
	//-- Step 1: Detect the keypoints using SURF Detector
	int minHessian = 400;

	SurfFeatureDetector detector( minHessian );

	std::vector<KeyPoint> keypoints_object, keypoints_scene;

	detector.detect( window_cropped, keypoints_object );
	detector.detect( background, keypoints_scene );

	//-- Step 2: Calculate descriptors (feature vectors)
	SurfDescriptorExtractor extractor;

	Mat descriptors_object, descriptors_scene;

	extractor.compute( window_cropped, keypoints_object, descriptors_object );
	extractor.compute( background, keypoints_scene, descriptors_scene );

	//-- Step 3: Matching descriptor vectors using FLANN matcher
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match( descriptors_object, descriptors_scene, matches );

	double max_dist = 0; double min_dist = 100;

	//-- Quick calculation of max and min distances between keypoints
	for( int i = 0; i < descriptors_object.rows; i++ )
	{ double dist = matches[i].distance;
	if( dist < min_dist ) min_dist = dist;
	if( dist > max_dist ) max_dist = dist;
	}

	printf("-- Max dist : %f \n", max_dist );
	printf("-- Min dist : %f \n", min_dist );

	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	std::vector< DMatch > good_matches;

	for( int i = 0; i < descriptors_object.rows; i++ )
	{ if( matches[i].distance < 3*min_dist )
	{ good_matches.push_back( matches[i]); }
	}

	Mat img_matches;
	drawMatches( window_cropped, keypoints_object, background, keypoints_scene,
	       good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
	       vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

	//-- Localize the object
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for( int i = 0; i < good_matches.size(); i++ )
	{
	//-- Get the keypoints from the good matches
	obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
	scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
	}

	Mat H = findHomography( obj, scene, CV_RANSAC );

	//-- Get the corners from the image_1 ( the object to be "detected" )
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0,0); 						//top left corner
	obj_corners[3] = cvPoint( window_cropped.cols, 0 );			//bot left corner
	obj_corners[2] = cvPoint( window_cropped.cols, window_cropped.rows ); 	//bot right corner
	obj_corners[1] = cvPoint( 0, window_cropped.rows );			//top right corner
	std::vector<Point2f> scene_corners(4);

	perspectiveTransform( obj_corners, scene_corners, H);	

	BG.push_back(Point2f(scene_corners[0].x, scene_corners[0].y));	//top left corner
	BG.push_back(Point2f(scene_corners[1].x, scene_corners[1].y));	//bot left corner
	BG.push_back(Point2f(scene_corners[2].x, scene_corners[2].y));	//bot right corner
	BG.push_back(Point2f(scene_corners[3].x, scene_corners[3].y));	//top right corner	
	
	//printf("corner[0] = %f %f\n", scene_corners[0].x, scene_corners[0].y );
	//printf("corner[1] = %f %f\n", scene_corners[1].x, scene_corners[1].y );
	//printf("corner[2] = %f %f\n", scene_corners[2].x, scene_corners[2].y );
	//printf("corner[3] = %f %f\n", scene_corners[3].x, scene_corners[3].y );
}

void ImageOverlay::find_homography()//vector<Point2f> foreground,vector<Point2f> background)
{
	//finds a perspective transformation between two planes
	Homography = findHomography(FG, BG, 0);
}

void ImageOverlay::warp_perspective()
{
       	// Warp the foreground image to change its perspective
	warpPerspective(foreground,foreWarped,Homography,background.size() );
}

Mat ImageOverlay::combine_images()
{
	Mat gray, gray_inv, temp_back, temp_fore;
	cvtColor(foreWarped, gray, CV_BGR2GRAY);
	threshold(gray,gray,0,255,CV_THRESH_BINARY);	
        bitwise_not ( gray, gray_inv );
	background.copyTo(temp_back,gray_inv);
    	foreWarped.copyTo(temp_fore,gray);
    	output = temp_back+temp_fore;
	return output;
}

void ImageOverlay::showImage(Mat& img, String nameOfWindow)	{

	namedWindow(nameOfWindow, WINDOW_NORMAL);
	imshow(nameOfWindow, img);

}

Mat ImageOverlay::overlayImage()
{
  Point2i location  = cv::Point(0, 0);
  background.copyTo(output);


//	namedWindow("ForeWarped", WINDOW_NORMAL);
//	imshow("ForeWarped", foreWarped);


  // start at the row indicated by location, or at row 0 if location.y is negative.
  for(int y = std::max(location.y , 0); y < background.rows; ++y)
  {
    int fY = y - location.y; // because of the translation

    // we are done of we have processed all rows of the foreground image.
    if(fY >= foreWarped.rows)
      break;

    // start at the column indicated by location, 

    // or at column 0 if location.x is negative.
    for(int x = std::max(location.x, 0); x < background.cols; ++x)
    {
      int fX = x - location.x; // because of the translation.

      // we are done with this row if the column is outside of the foreground image.
      if(fX >= foreWarped.cols)
        break;

      // determine the opacity of the foregrond pixel, using its fourth (alpha) channel.
      double opacity =
        ((double)foreWarped.data[fY * foreWarped.step + fX * foreWarped.channels() + 3])

        / 255.;


      // and now combine the background and foreground pixel, using the opacity, 

      // but only if opacity > 0.
      for(int c = 0; opacity > 0 && c < output.channels(); ++c)
      {
        unsigned char foreWarpedPx =
          foreWarped.data[fY * foreWarped.step + fX * foreWarped.channels() + c];
        unsigned char backgroundPx =
          background.data[y * background.step + x * background.channels() + c];
        output.data[y*output.step + output.channels()*x + c] =
          backgroundPx * (1.-opacity) + foreWarpedPx * opacity;
      }
    }
  }
	namedWindow("Output", WINDOW_NORMAL);
	imshow("Output", output);
	//imwrite("Output.jpg", output);

	return output;
}
