#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "dist/jsoncpp.cpp"
#include "dist/json/json.h"


using namespace std;
using namespace cv;
typedef unsigned char byte;

static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

static size_t write_data(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_decode(std::string const& encoded_string) {
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (j = 0; j < 4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }

    return ret;
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}

void uploadImage(std::string readBuffer){

    CURL *curl;
    CURLcode res;
    struct curl_slist *headers=NULL;

  headers = curl_slist_append(headers, "Content-Type: text/json");
  curl = curl_easy_init();
 if(curl) {
   curl_easy_setopt(curl, CURLOPT_URL, "http://ezquote-server.herokuapp.com/uploadProcessedImage");
   curl_easy_setopt(curl, CURLOPT_POSTFIELDS, readBuffer.c_str());
   curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
   curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, readBuffer.size());
   res = curl_easy_perform(curl);
   /* Check for errors */
   if(res != CURLE_OK)
     fprintf(stderr, "curl_easy_perform() failed: %s\n",
             curl_easy_strerror(res));

   /* always cleanup */
   curl_slist_free_all(headers);
   curl_easy_cleanup(curl);
 }
}

Mat image_download(std::string *id, std::string *frame)
{
  CURL *curl;
  CURLcode res;
  const cv::_InputArray data;
  std::string readBuffer;

  curl = curl_easy_init();
  if (curl) {

    curl_easy_setopt(curl, CURLOPT_URL, "http://ezquote-server.herokuapp.com/requestImageFile");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    readBuffer.clear();
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
 }

  // printf("%s\n\n", readBuffer.c_str());

  printf("Image retrieved.\n");
  Json::Value values;
  Json::Reader reader;
  reader.parse(readBuffer, values);

  Json::Value imageArray = values.get("userUploadedImage", "default value");
  Json::Value idNumber = values.get("id","default value");
  Json::Value frameNumber = values.get("selectedFrame","default value");
  string out = imageArray.asString();
  *id = idNumber.asString();
  *frame = frameNumber.asString();
  string decoded_string = base64_decode(out);
  vector<uchar> image(decoded_string.begin(), decoded_string.end());

  Mat img = imdecode(image, IMREAD_UNCHANGED);

  return img;
}

string encode(Mat img, std::string id, std::string length, std::string width)
{
  vector<uchar> buff;//buffer for coding
  vector<int> param = vector<int>(2);
  param[0]=CV_IMWRITE_JPEG_QUALITY;
  param[1]=95;

  imencode(".jpg", img, buff, param);

  std::string str(buff.begin(), buff.end());

  string encoded_string = base64_encode(reinterpret_cast<const unsigned char*>(str.c_str()), buff.size());

  Json::Value newValue;
  Json::FastWriter fastWritter;

  newValue["overLayedImage"] = encoded_string;
  
  newValue["id"] = id;
  newValue["length"] = length;
  newValue["width"] = width;

  string serialized = fastWritter.write(newValue);

  //printf("Serialized: %s\n\n", serialized.c_str());

  return serialized;
}

int main(int argc, char* argv[])
{
  std::string id = "";
  std::string frame ="";
std::string height ="";
std::string width ="";

  Mat image;
  //Mat uploadme;

  if (argc == 1 | argc > 2) {
    cout << "You must specify either download or upload while running this command" << endl;
  }


  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i],"download") == 0) {
      ofstream myfile;
      myfile.open("images/id.txt");

      image = image_download(&id, &frame);

      if (image.empty()){
	printf("NO image to download.\n");
      }

      myfile << frame << endl;
      myfile << id << endl;
      myfile.close();
      if ( ! imwrite( "images/downloaded.jpg", image ) ) {
	cout << "image write failed\n";
}
  
    } 

    else if (strcmp(argv[i],"upload") == 0){
      ifstream myfile;
      myfile.open("images/id.txt");
      
      getline(myfile,frame);
      getline(myfile,id);
      getline(myfile,height);
      getline(myfile,width);
     cout << "ID is " << id << endl; 
      image=imread("images/newest.jpg");

      string serialized = encode(image, id, height, width);
      uploadImage(serialized);
      imwrite( "images/upload.jpg", image );

      remove( "images/id.txt" );

    } //else {

      //cout << "ATTENTION!!!" << endl << "You did not enter a valid argument." << endl << "Please enter either upload or download." << endl;
    //}
  
  }

  return 0;
}
