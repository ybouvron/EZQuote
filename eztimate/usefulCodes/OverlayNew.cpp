#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
	//Mat foreground = imread("foreground.jpg", -1);	//Load the foreground image
	Mat foreground = imread("foreground.jpg");	
	Mat background = imread("background.jpg");	//Load the background image
	Mat foreground2;	//Used to store resized foreground
	Mat background2;	//Used to store new background image
	Size size( 2935, 2072 );	//Manual input dimension to resize the foreground image
	Point2i point( 970, 279 );	//Manual input of top left corner of window in background
	background2 = background.clone(); //Copies background image to background2
	resize(foreground, foreground2, size);	//Resizes foreground to background's window size and outputs it to background

	cv::Rect region( point, foreground2.size() );
	foreground2.copyTo( background2( region ) );

	namedWindow("result", WINDOW_NORMAL); //Displays new overlaid image image
	cv::imshow("result", background2);

	namedWindow("foreground", WINDOW_NORMAL); //Displays foreground image
	cv::imshow("foreground", foreground);

	namedWindow("background", WINDOW_NORMAL); //Displays original background image
	cv::imshow("background", background);
	
	imwrite( "result.jpg", background2 );	// Saves new overlay as image

	waitKey(0);
}
