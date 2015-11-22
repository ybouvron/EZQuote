/*
Project 195 SJSU
EZtimate Finder Subsystem

Members:
Yohan Bouvron
Marvin Flores
Brian Nguyen

description:



*/

//VERSION 0.01 07/20/2015

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>

#include <opencv2/highgui.hpp>

#define CUDA 1

#if 0
//ignore this
//sample code for uploading an image too OpenCV, but we have a better solution
//keep this, we might need it later on
int main()
{
	IplImage* img = cvLoadImage("C:\\Users\\Marvin\\Desktop\\CMPE195Project\\ImageSource\\window_01.jpg");
	cvNamedWindow("Example1", 10);
	cvShowImage("Example1", img);
	cvWaitKey(0);
	cvReleaseImage(&img);
	cvDestroyWindow("Example1");
	return 0;
}

#endif // 0

using namespace std;
using namespace cv;
using namespace cv::cuda;

//current use case:
//compile the code and run the executable file - this will be OS dependent so you need to configure your compiler appropriately
//<executable file> <input image> <output image>
//
//example use case (WINDOWS): Project195_v1.exe C:\Users\Marvin\Desktop\PROJECT195\Project195_sourceFiles\window_01c.jpg C:\Users\Marvin\Desktop\PROJECT195\Project195_sourceFiles\wb.jpg
//Note: this assumes that you are in the directory where Project195_v1.exe lives
int main(int argc, char* argv[])
{
	Mat inputImage, outputImage; 
	//GpuMat src(inputImage) ; //currently useless
	double t0; //used as start time

	//take an input image as argument
	inputImage = imread(argv[1], IMREAD_UNCHANGED);
	if (inputImage.empty())	{
		cout << "Error reading image!\n";
		exit(1);
	}
	
	t0 = (double)getTickCount(); // get current time

	//create two windows with the names of the images
	namedWindow(argv[1], WINDOW_NORMAL);
	namedWindow(argv[2], WINDOW_NORMAL);
	
	//show image into the just create namedWindow instances
	cvtColor(inputImage, outputImage, COLOR_BGR2GRAY); //convert the image to B&W
	imshow(argv[1], inputImage);
	imshow(argv[2], outputImage);

	double elapseTime = ((double)getTickCount() - t0) / getTickFrequency();

	cout << "Elapse time: " << elapseTime << endl;
	
	FileStorage outFile("EZtimateConfig.xml", FileStorage::WRITE);
	outFile << "elapseTime" << elapseTime;
	outFile.release();

	cout << "press any key to exit\n";
	waitKey(); // wait key function only works when there's at least an active window created

	//Writing image
	imwrite(argv[2], outputImage); //this line creates a new image file
	return 0;
}
