#ifndef IMAGEOVERLAY_H_
#define IMAGEOVERLAY_H_

/*
This class will handle all the data saving
*/


#include<iostream>
#include<string>
#include <opencv2/opencv.hpp>
//#include <opencv2/core/cuda.hpp>

using namespace std;
using namespace cv;
//using namespace cv::cuda;

class ImageOverlay {

public:

	ImageOverlay(Mat back, Mat front, Mat cropped);
	~ImageOverlay();
	void cornerForeground();
	void cornerBackground();
	void find_homography();
	void warp_perspective();
	Mat combine_images();
	void showImage(Mat& img, String nameOfWindow);
	Mat overlayImage();
	
private:
	Mat background;
	Mat foreground;
	Mat window_cropped;
	Mat foreWarped;		//Used to store the warped foreground image
	Mat output;		//Used to store the final overlaid image
	Mat Homography;
	vector<Point2f> FG;     //Stores 4 points(x,y) of the corners of the foreground image.
	vector<Point2f> BG;	//Stores 4 points(x,y) of the corners of the window in the background image
	
};



#endif
