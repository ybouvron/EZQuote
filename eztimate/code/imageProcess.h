#ifndef IMAGEPROCESS_H_
#define IMAGEPROCESS_H_
#ifdef __cplusplus
extern "C" {
#endif
/*
	This class handles all the processing process
	Preprocessing -> object detection -> dimensions calculation
*/

#include<iostream>
#include<string>
#include <opencv2/opencv.hpp>
//#include <opencv2\core\cuda.hpp>
//#include <opencv2\highgui.hpp>

using namespace std;
using namespace cv;
//using namespace cv::cuda;


class ImageProcess
{


public:
	
	ImageProcess();
	~ImageProcess();

	Mat convertToGray(const Mat& img); //convert the input image to grayscale; returns a cloned image
	
	//show a Mat object in a window
	void showImage(Mat& img, String nameOfWindow);

	//detect our source object in the input_scene
	//This also calculates the dimensions of the window
	void objectDetect(const Mat& source, const Mat& input_scene);
	
	//set our overall window dimensions in pixel (taken from the cropped image)
	void setWindowDimensions(Mat& img);

	//prints whether Horizontal or Vertical orientation
	void printOrientation();

	//checks whether or not we actually detected the reference object (paper)
	bool isDetectionTrue(vector<Point2f> vertices);
	
	//not used, just to show some lables within rectangles
	void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour);
	
	//returns a vector of detected corner points; raw corners
	//used for imageOverlay
	vector<Point2f> getRawCornerPoints() { return rawCornerPoints;}

	//returns a vector of detected corner points but already rotated
	//used in reference detection
	vector<Point2f> getRectangleCornerPoints() {return rectangleCornerPoints;}
	

	//getters
	float getWidthOfWindowInPixels();
	float getHeightOfWindowInPixels();

	float getWidthOfWindowCm();
	float getHeightOfWindowCm();
	
	bool getIsPaperDetected() {return isDetected;}
	

	//setters
	void setWidthOfWindowInPixels(int w);
	void setHeightOfWindowInPixels(int h);

	void setWidthOfWindowCm(float w);
	void setHeightOfWindowCm(float h);

	void setOutputImage(Mat& image);
	void setInputImage(Mat& iamge);
	
private:

	//for checking if object is detected
	bool isDetected;
	
	//For orientation
	int HORIZONTAL;
	int VERTICAL;

	int paperOrientation;

	Mat inputImage, outputImage;
	
	int widthOfPaperInPixels;
	int heightOfPaperInPixels;

	//estimated width and height of reference object, measured after create a rectangle out of
		//raw corner points values
	int estWidthOfObject; 
	int estHeightOfObject;
	
	float widthOfWindowCm;
	float heightOfWindowCm;

	float widthOfWindowInPixels; //width of the window including the border
	float heightOfWindowInPixels; // height of the window including the border
	std::vector<Point2f> rawCornerPoints; //contains raw corner points; for imageOverlay
	std::vector<Point2f> rectangleCornerPoints;
};

#ifdef __cplusplus
}
#endif
#endif /* IMAGEPROCESS */
