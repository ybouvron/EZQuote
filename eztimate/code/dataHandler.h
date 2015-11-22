#ifndef DATAHANDLER_H_
#define DATAHANDLER_H_

/*
	This class will handle all the data saving
*/

#ifdef __cplusplus
extern "C" {
#endif

#include<iostream>
#include<string>
#include<fstream>

#include <opencv2/opencv.hpp>
//#include <opencv2\core\cuda.hpp>
//#include <opencv2\highgui.hpp>

using namespace std;

//using namespace cv::cuda;

class DataHandler {

public:

	DataHandler(string="");

	void saveDimensions(float=0, float=0);

	string loadImage(char* img_name);

	void saveImage(Mat& img, string name);

	~DataHandler();

private:

	string out_fn_dimensions;

};


#ifdef __cplusplus
}
#endif
#endif //end DATAHANDLER_H_
