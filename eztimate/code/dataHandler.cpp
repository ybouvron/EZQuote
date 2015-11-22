
#include"dataHandler.h"

#include<iostream>
#include<fstream>


#include <opencv2/opencv.hpp>
//#include <opencv2\core\cuda.hpp>
//#include <opencv2\highgui.hpp>

using namespace std;


DataHandler::DataHandler(string output_file)	{

  
  out_fn_dimensions = output_file;

}

//save our dimensions as .json file
void DataHandler::saveDimensions(float width, float height)	{

	//write our width and height output (cm)
	ofstream outfile;
	char *out = new char[out_fn_dimensions.length() + 1];
	strcpy(out, out_fn_dimensions.c_str());
	
	outfile.open(out);
	
	outfile << (int) width << " " << (int) height;
			
	outfile.close();

	delete [] out;

}

string DataHandler::loadImage(char* img_name){
	string image_filename = "";

	ifstream infile(img_name);
	if(infile.is_open()) {
		getline(infile, image_filename);
		
	}
	
	infile.close();

	return image_filename;
}


void DataHandler::saveImage(Mat& img, string name)	{

	cv::imwrite(name, img);

}

DataHandler::~DataHandler()	{


}
