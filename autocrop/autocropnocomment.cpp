/*
 Autocrop.cpp
 
 ****This implements center gravity checks for found rectangles
 
 After loading image, program goes through a *very* long loop of brightness, contrast, and shape thresholding values.
 Once the loop is complete, all the detected windows are averaged and the shape closest to average is assumed to be the window.
 The shape edges are extended and "straightened" through estimation.
 Intersects of extended edges are determined, along with the center mass.
 The lenght of all sides are estimated by picking largest height and a diagonal, then calculating the width.
 New matrix is created with the above dimensions, and the detected shape is fit to the matrix.
 
 */


//#include <opencv2/imgcodecs.hpp>
//#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/core/core.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

using namespace cv;
using namespace std;

Mat src, edges;
Mat src_gray;
Mat standard_hough;
Point2f center(0,0); // Center point of image

/****** Function headers ******/

//Determines angle of contour
double angle(Point pt1, Point pt2, Point pt0 );

//Draws all detected rectangles
static void drawRectangles(Mat& image, const vector<vector<Point> >& rectangles );

//Removes all rectangles that are  < 80% or > 10% of image area
vector<vector<Point> > filterRectangles(Mat image, const vector<vector<Point> >& rectangles );

//Removes repetitive rectangles with similar center points
vector<vector<Point> > removeRepetition(Mat image, const vector<vector<Point> >& rectangles );

//Finds all rectangles
static void findRectangles( const Mat& image, vector<vector<Point> >& rectangles, int thresh );

//Finds the rectangle closest to average
static void chooseBestRectangle(vector<vector<Point> >& rectangles);

// Adjusts the brightness and contrast of image
// alpha = contrast, beta = brightness
Mat brightnessContrast (Mat image, double alpha, int beta);

//Scans image with brightness/contrast/shape detection. Slow for higher resolution images.
Mat imageScan(Mat src, vector<vector<Point> >& rectangles, double alpha, int beta);

//Convert end points to useable format
vector<Vec4i> convertLines(vector<vector< Point> > end_points);

//Find straight edges in rectangle
void Standard_Hough( int, void* , vector<vector<Point> >& end_points);

//find intersect of all ines
Point2f computeIntersect(Vec4i, Vec4i);

//Sorts corners in a specific order
void sortCorners(vector<Point2f>& corners, Point2f center);

//Checks if detected shape is a quadrilateral. End program if not.
void checkQuadrilateral(vector<Point2f> approx, Mat src, vector<Point2f> corners, vector<Vec4i> lines);

// Finds center point of rectangle
vector<Point2f> findCenter(vector<vector<Point> > rectangles);

//** main **//

int main(int ac, char** av)
{
    Mat src, imageProcessed, blackimage, dst;
    vector<vector<Point> > rectangles; // Stores detected rectangles
    vector<vector<Point> > end_points; // End points of linesf
    vector<Point2f> corners;
    vector<Vec4i> lines;
    Point2f pt;	//Temporary points to store into vector<Point2f> corners
    vector<Point2f> approx;	//Approximate straight edges
    
    double alpha = 1.0;	//contrast control
    int beta = 0;		//brightness value to be calculated
    
    src=imread("11.jpeg");	// reads image
    
    //reduces image size if needed. Only for viewing purposes. Remove in final code
    float reduceFactor;
    int imageWidth = 900;
    if(src.cols > imageWidth){
        reduceFactor = src.cols / imageWidth;
        resize(src, src, Size(src.cols/reduceFactor, src.rows/reduceFactor));
    }
    
    //imshow("Original", src);
    
    //Copy original image to destination
    dst = src.clone();
    
    /*
     int erosion_size = src.cols/300;
     Mat element = getStructuringElement(cv::MORPH_CROSS,
     cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
     cv::Point(erosion_size, erosion_size) );
     
     // Apply erosion or dilation on the image
     erode(src,src,element);  // dilate(image,dst,element);
     
     //imshow("eroded", src);
     */
    
    //Scans image with brightness/contrast/shape detection. Very slow - room for performance improvements here.
    //cout << endl << "Scanning image for window, please wait. Countdown..." << endl;
    imageProcessed = imageScan(src, rectangles, alpha, beta);	//draws rectangle(s) on passed Mat
    
    
    chooseBestRectangle(rectangles);
    
    //draws rectangle(s) on passed Mat
    drawRectangles(imageProcessed, rectangles);
    
    //create black image for perspective correction
    blackimage = brightnessContrast(imageProcessed, 1, -300);
    
    //If no rectangles found, end program.
    if(rectangles.size() < 1) {
        cout << "Error! No valid rectangles found in image." << endl;
        waitKey(0);
        return -1;
        
    }
    drawRectangles(blackimage, rectangles);
    
    //imshow("Rectangle drawn", imageProcessed);
    //imshow("blackimage", blackimage);
    
    
    /****** Perspective Correction ******/
    
    cvtColor( blackimage, src_gray, COLOR_RGB2GRAY );
   	blur(src_gray, src_gray, Size(3, 3));
   	// Apply Canny edge detector
   	Canny( src_gray, edges, 50, 200, 3 );
    
    //namedWindow( "Hough Lines", WINDOW_AUTOSIZE );
    // Find Hough lines
   	Standard_Hough(100, 0, end_points); //Call Houghlines function
    
    //Convert end points to useable type
    lines = convertLines(end_points);
    
    // Find Corners
    for (int i = 0; i < lines.size(); i++)
    {
        for (int j = i+1; j < lines.size(); j++)
        {
            pt = computeIntersect(lines[i], lines[j]);
            //cout << "Intersects computed: " << pt << endl;
            if (pt.x >= 0 && pt.y >= 0 && pt.x < src.cols && pt.y < src.rows)
                corners.push_back(pt);
        }
    }
    
    //Approximates sides of the detected shape
    approxPolyDP(corners, approx, arcLength(Mat(corners), true) * 0.05, true);
    
    //If shape is not a quadrilateral, stop running.
    checkQuadrilateral(approx, src, corners, lines);
    
    // Get mass center
    for (int i = 0; i < corners.size(); i++)
        center += corners[i];
    center *= (1. / corners.size());
    
    //Sort corners
    sortCorners(corners, center);
    if (corners.size() == 0){
        cout << "The corners were not sorted correctly!" << endl;
        return -1;
    }
    
    //Display corner coordinates
    //cout << "Corners: " << endl << corners << endl;
    
    // Draw lines on destination image
    for (int i = 0; i < lines.size(); i++)
    {
        Vec4i v = lines[i];
        line(dst, Point(v[0], v[1]), Point(v[2], v[3]), CV_RGB(0,255,0));
    }
    
    // Draw corner points
    circle(dst, corners[0], 6, CV_RGB(255,0,0), 2);
    circle(dst, corners[1], 6, CV_RGB(0,255,0), 2);
    circle(dst, corners[2], 6, CV_RGB(0,0,255), 2);
    circle(dst, corners[3], 6, CV_RGB(255,255,255), 2);
    
    // Draw mass center
    circle(dst, center, 6, CV_RGB(255,255,0), 2);
    
    
    //Calculate length and height of rectangle
    //Corners order: top left (x,y), top right, bottom right, bottom left
    float heightRight = sqrt(
                             pow((corners[1].x - corners[2].x), 2.0) +
                             pow((corners[1].y - corners[2].y), 2.0)  );
    //calculate diagonal length from bottom right to top left
    float diagonal = sqrt(
                          pow((corners[2].x - corners[0].x), 2.0) +
                          pow((corners[2].y - corners[0].y), 2.0)  );
    
    //calculate length
    float rectangleLength = sqrt( pow(diagonal,2) - pow(heightRight,2)  );
    
    //display height and length.
    //cout << "height of cropped window: " << heightRight << endl;
    //cout << "length of cropped window: " << rectangleLength << endl;
    
    //create new matrix in the size of the calculated height and length
    Mat quad = Mat::zeros((int)(heightRight + 0.5f), (int)(rectangleLength + 0.5f), CV_8UC3);
    
    //store corner points of new matrix into a vector
    vector<Point2f> quad_pts;
    quad_pts.push_back(Point2f(0, 0));
    quad_pts.push_back(Point2f(quad.cols, 0));
    quad_pts.push_back(Point2f(quad.cols, quad.rows));
    quad_pts.push_back(Point2f(0, quad.rows));
    
    //Warp window to fit into perfect rectangle matrix
    Mat transmtx = getPerspectiveTransform(corners, quad_pts);
    warpPerspective(src, quad, transmtx, quad.size());
    
    //imshow("Corners", dst);
    //imshow("Autocropped image", quad);
    
    waitKey(0);
    return 0;
}


//Determines angle of contour
double angle(Point pt1, Point pt2, Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

//Draws all detected rectangles no matter what
static void drawRectangles(Mat& image, const vector<vector<Point> >& rectangles )
{
    
    for( size_t i = 0; i < rectangles.size(); i++ )
    {
        const Point* p = &rectangles[i][0];
        int n = (int)rectangles[i].size();
        polylines(image, &p, &n, 1, true, Scalar(0,255,0), 1);
    }
}

//Removes all rectangles that are  < 70% or > 10% of image area
vector<vector<Point> > filterRectangles(Mat image, const vector<vector<Point> >& rectangles ) {
    //area of original image
    double imageSize = image.rows * image.cols;
    Point2f imageCenter = Point2f(image.cols/2.0, image.rows/2.0);
    
    vector<Point2f> centerPoints = findCenter(rectangles);
    
    vector<vector<Point> > tempRectangles;  //temporary
    for(size_t i = 0; i < rectangles.size(); i++) {
        double rectangleArea = contourArea(rectangles[i]);	//area of detected rectangles
        if(rectangleArea < (imageSize * 0.8) && rectangleArea > (imageSize * 0.1)
           && centerPoints[i].x >= imageCenter.x - image.rows/6
           && centerPoints[i].x <= imageCenter.x + image.rows/6
           && centerPoints[i].y >= imageCenter.y - image.cols/6
           && centerPoints[i].y <= imageCenter.y + image.cols/6) {
            tempRectangles.push_back(rectangles[i]);	//Push rectangle to temporary vector<vector<Point>>
        }
        
    }
    
    return tempRectangles;
}

//Removes repetitive rectangles with similar center points
vector<vector<Point> > removeRepetition(Mat image, const vector<vector<Point> >& rectangles ) {
    //area of original image
    double imageSize = image.rows * image.cols;
    Point2f imageCenter = Point2f(image.cols/2.0, image.rows/2.0);
    
    vector<Point2f> centerPoint = findCenter(rectangles);
    vector<vector<Point> > tempRectangles;  //temporary
    for(size_t i = rectangles.size(); i > 0; i--) {
        for(size_t j = i; j > 0; j--) {
            if(centerPoint[i].x <= centerPoint[j].x + image.cols/5
               && centerPoint[i].x >= centerPoint[j].x - image.cols/5
               && centerPoint[i].y <= centerPoint[j].y + image.rows/5
               && centerPoint[i].y >= centerPoint[j].y - image.rows/5 ) {
                break;
            }
            
        }
        tempRectangles.push_back(rectangles[i]);
    }
    
    return tempRectangles;
}

//Finds all rectangles
static void findRectangles( const Mat& image, vector<vector<Point> >& rectangles, int thresh )
{
    rectangles.clear();
    
    Mat pyr, timg, gray, gray0(image.size(), CV_8U);
    
    vector<vector<Point> > contours;
    
    // down-scale and upscale the image to filter out the noise
    pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
    pyrUp(pyr, timg, image.size());
    
    // find rectangles in every color plane of the image
    for( int c = 0; c < 3; c++ )
    {
        int ch[] = {c, 0};
        mixChannels(&timg, 1, &gray0, 1, ch, 1);
        
        // try several threshold levels
        for( int l = 0; l < 12; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch rectangles with gradient shading
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                Canny(gray0, gray, 0, thresh, 3);   //CHANGE THIS TOO
                // dilate canny output to remove potential
                // holes between edge segments
                dilate(gray, gray, Mat(), Point(-1,-1));
            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l+1)*255/thresh;
            }
            
            // find contours and store them all as a list
            findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
            
            vector<Point> approx;
            
            // test each contour
            for( size_t i = 0; i < contours.size(); i++ )
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);
                
                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if( approx.size() == 4 &&
                   fabs(contourArea(Mat(approx))) > 1000 &&
                   isContourConvex(Mat(approx)))
                {
                    double maxCosine = 0;
                    
                    for( int j = 2; j < 5; j++ )
                    {
                        // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }
                    
                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if( maxCosine < 0.3) {
                        rectangles.push_back(approx);
                        
                    }
                }
            }
        }
    }
    
    
}

// Adjusts the brightness and contrast of image
// alpha = contrast, beta = brightness
Mat brightnessContrast (Mat image, double alpha, int beta) {
    Mat new_image = Mat::zeros( image.size(), image.type() );
    
    /// Do the operation new_image(i,j) = alpha*image(i,j) + beta
    for( int y = 0; y < image.rows; y++ ) {
        for( int x = 0; x < image.cols; x++ ) {
            for( int c = 0; c < 3; c++ ) {
                new_image.at<Vec3b>(y,x)[c] = saturate_cast<uchar>( alpha*( image.at<Vec3b>(y,x)[c] ) + beta );
            }
        }
    }
    
    return new_image;
}

//Find Edges of window
void Standard_Hough( int threshold, void*, vector<vector<Point> >& end_points)
{
    vector<Vec2f> s_lines;
    vector<Point> temp_points; //Temporary vector to store points
    bool similarFound = false; //Similar line found indicator for later use
    
    //Convert to gray
    cvtColor( edges, standard_hough, COLOR_GRAY2BGR );
    
    //Use Standard Hough Transform to find edges
    //Make sure threshold is low enough to find all edges
    HoughLines( edges, s_lines, 1, CV_PI/180, threshold, 0, 0 );
    
    // Show the resulting lines
    for( size_t i = 0; i < s_lines.size(); i++ )
    {
        
        float r = s_lines[i][0], t = s_lines[i][1];
      		double cos_t = cos(t), sin_t = sin(t);
      		double x0 = r*cos_t, y0 = r*sin_t;
      		double alpha = 1000;
        
        //Compute end point coordinates of lines
        Point pt1( cvRound(x0 + alpha*(-sin_t)), cvRound(y0 + alpha*cos_t) );
        Point pt2( cvRound(x0 - alpha*(-sin_t)), cvRound(y0 - alpha*cos_t) );
        
        //Find similar lines by comparing starting points.
        //If similar, set similarFlag to true
        //Similar defined by two coordinates within 1/12 of image height/width
        for(size_t j = 0; j < end_points.size(); j++)
        {
            if(
               (pt1.x > end_points[j][0].x - (standard_hough.cols / 12)
                && pt1.x < end_points[j][0].x + (standard_hough.cols / 12)
                && pt1.y > end_points[j][0].y - (standard_hough.rows / 12)
                && pt1.y < end_points[j][0].y + (standard_hough.rows / 12) )
               ||
               (pt1.x > end_points[j][1].x - (standard_hough.cols / 12)
                && pt1.x < end_points[j][1].x + (standard_hough.cols / 12)
                && pt1.y > end_points[j][1].y - (standard_hough.rows / 12)
                && pt1.y < end_points[j][1].y + (standard_hough.rows / 12))	) {
                   similarFound = true;
               }
            
        }
        
        //If line is not similar, push it to the end_points vector for storage.
        if(!similarFound){
            temp_points.push_back(pt1);
            temp_points.push_back(pt2);
            end_points.push_back(temp_points);
            temp_points.clear();
        }
        similarFound = false;	//Reset similarFound flag
   	}
    
    
    //Check if 4 lines were found. If not, end program. Fatal error.
    if(end_points.size() != 4) {
        cout << "Fatal error! Houghlines did not find exactly 4 lines. "
        << end_points.size() << " were found. Consult with Autocrop group." << endl << endl;
        
        for(int i = 0; i < end_points.size(); i++){
            cout << end_points[i] << endl;
        }
        //Draw final lines
        for(size_t i = 0; i < end_points.size(); i++) {
            line( standard_hough, end_points[i][0], end_points[i][1], Scalar(255,0,0), 3);
        }
        //display lines found. Optional.
        //imshow( "Hough Lines", standard_hough );
        
        waitKey(0);
        exit(1);
    }
    
    //Draw final lines
    for(size_t i = 0; i < end_points.size(); i++) {
        line( standard_hough, end_points[i][0], end_points[i][1], Scalar(255,0,0), 3);
    }
    //display lines found. Optional.
   //imshow( "Hough Lines", standard_hough );
    
}


//Finds intersects. Lines are in this format: Line1(o1, p1), Line2(o2, p2)
//where o1, p1, o2, and p2 are coordinates
Point2f computeIntersect(Vec4i a, Vec4i b)
{
    //Convert Vec4i line endpoints to useable points
    Point2f o1, p1, o2, p2;
    
    //Order points to read from left to right. Reduces chances of false intersects
    if(a[0] < a[2]) {
        o1 = Point2f(a[0], a[1]);
        p1 = Point2f(a[2], a[3]);
    }
    else {
        o1 = Point2f(a[2], a[3]);
        p1 = Point2f(a[0], a[1]);
    }
    
    if(b[0] < b[2]) {
        o2 = Point2f(b[0], b[1]);
        p2 = Point2f(b[2], b[3]);
    }
    else {
        Point2f o2 = Point2f(b[2], b[3]);
        Point2f p2 = Point2f(b[0], b[1]);
    }
    
    Point2f r; 	//Intersect
    
    Point2f x = o2 - o1;
    Point2f d1 = p1 - o1;
    Point2f d2 = p2 - o2;
    
    float cross = d1.x*d2.y - d1.y*d2.x;
    if (abs(cross) < /*EPS*/1e-8)
        return Point2f(-1, -1);
    
    double t1 = (x.x * d2.y - x.y * d2.x)/cross;
    r = o1 + d1 * t1;
    return r;
}

//Sorts corners by pushing coordinates into vector in order.
//Order: Top left, top right, bottom right, bottom left
void sortCorners(vector<cv::Point2f>& corners, Point2f center)
{
    vector<Point2f> top, bot;
    
    //Find whether points are top or bottom
    for (int i = 0; i < corners.size(); i++)
    {
        if (corners[i].y < center.y)
            top.push_back(corners[i]);
        else
            bot.push_back(corners[i]);
    }
    corners.clear();	//clear corners temporarily
    
    //order points and push back to corners vector in order
    if (top.size() == 2 && bot.size() == 2){
        Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
        Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
        Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
        Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];
        
        
        corners.push_back(tl);
        corners.push_back(tr);
        corners.push_back(br);
        corners.push_back(bl);
    }
}

//Finds the rectangle that is closest to average
static void chooseBestRectangle(vector<vector<Point> >& rectangles) {
    //cout << "Finding the best of " << rectangles.size() << " detections..." << endl;
    double average = 0.0;
    double max = 0.0;
    int maxIndex = 0;
    double min = contourArea(rectangles[0]);
    double minIndex = 0;
    
    vector<Point2f> centerPoint;
    Point2f avgPoint;
    float tempX;
    float tempY;
    float bestX;
    float bestY;
    int bestCenterIndex;
    
    double areaDiff = 0.0;
    int bestIndex = 0;
    
    vector<Point> temp;
    
    //finds average area
    for (int i = 0; i < rectangles.size(); i++) {
        average += contourArea(rectangles[i]);
        
        if(contourArea(rectangles[i]) > max){
            max = contourArea(rectangles[i]);
            maxIndex = i;
        }
        if(contourArea(rectangles[i]) < min){
            min = contourArea(rectangles[i]);
            minIndex = i;
        }
    }
    average = average / rectangles.size();
    //cout << "Average area: " << average << endl;
    
    
    //find the average center for all rectangles smaller than the average area
    centerPoint = findCenter(rectangles);
    for(int i = 0; i < centerPoint.size(); i++){
        avgPoint += centerPoint[i];
        
    }
    avgPoint.x = avgPoint.x / centerPoint.size();
    avgPoint.y = avgPoint.y / centerPoint.size();
    bestX = avgPoint.x;
    bestY = avgPoint.y;
    //cout << "average center point: " << avgPoint << endl;
    
    for(int i = 0; i < centerPoint.size(); i++){
        if(contourArea(rectangles[i]) <= average) {
            tempX = abs( centerPoint[i].x - avgPoint.x);
            tempY = abs( centerPoint[i].y - avgPoint.y);
            if( tempX <= bestX && tempY <= bestY){
                bestX = tempX;
                bestY = tempY;
                bestCenterIndex = i;
            }
        }
        
    }
    
    
    temp = rectangles[bestCenterIndex];
    rectangles.clear();
    rectangles.push_back(temp);
    
    
}

Mat imageScan(Mat src, vector<vector<Point> >& rectangles, double alpha, int beta) {
    Mat tempImage;
    vector<vector<Point> > tempRectangles;
    int loopCount = 8;
    double startAlpha = 1.1;
    double endAlpha = startAlpha + 0.3;    //Change this variable to adjust outer loop count. Max value: 3
    int countdown = (endAlpha - startAlpha)/0.1;
    //cout << "Countdown time: " << countdown << endl;
    for (alpha = startAlpha; alpha < endAlpha; alpha += 0.1) {
        //Countdown displayed to show progress of scan to user
        //cout << countdown << endl;
	countdown -= 1;
        
        beta = ((alpha - 1) * -130);
        tempImage = brightnessContrast(src, alpha, beta);
        
        for(int thresh = 1; thresh < loopCount; thresh++) {
            tempImage = src;
            findRectangles(tempImage, tempRectangles, thresh);    //Finds all rectangles
            tempRectangles = filterRectangles(tempImage, tempRectangles);    //Filters out large and small false detections
            
            for(int i = 0; i < tempRectangles.size(); i++){
                rectangles.push_back(tempRectangles[i]);
            }
            
        }
        if(countdown <= 0) break;   
    }
    
    return tempImage;
}

//Convert end points to useable format
vector<Vec4i> convertLines(vector<vector< Point> > end_points) {
    vector<Vec4i> lines;	
    for(int i = 0; i < end_points.size(); i++)
   	{
        lines.push_back(Vec4i(end_points[i][0].x, end_points[i][0].y, end_points[i][1].x, end_points[i][1].y));
   	}
    
    return lines;
}

//Checks if detected shape is a quadrilateral. End program if not.
void checkQuadrilateral(vector<Point2f> approx, Mat src, vector<Point2f> corners, vector<Vec4i> lines) {
    if (approx.size() != 4)
    {
        cout << "The object is not quadrilateral! Consult with the autocrop team." << endl;
        cout << "Sides found: " << approx.size() << endl;
        
        cout << "Resolution of image: " << src.cols << " x " << src.rows << endl;
        
        cout << "corners: " << endl << corners << endl;
        
        cout << "extended lines:" << endl;
        for(int i = 0; i < lines.size(); i++) {
            cout << lines[i] << endl;
        }
        
        Mat temp = src;
        for(int i = 0; i < corners.size(); i++) {
            // Draw mass center
            circle(temp, corners[i], 6, CV_RGB(255,255,0), 2);
        }
        //imshow("Found corners", temp);
        
        waitKey(0);
        
        exit(1);
    }
}


// Finds center point of rectangle
vector<Point2f> findCenter(vector<vector<Point> > rectangles){
    /// Get the moments
    vector<Moments> mu(rectangles.size() );
    for( int i = 0; i < rectangles.size(); i++ ){ 
        mu[i] = moments( rectangles[i], false ); 
    }
    
    ///  Get the mass centers:
    vector<Point2f> mc( rectangles.size() );
    for( int i = 0; i < rectangles.size(); i++ ){ 
        mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ); 
        //cout << "Center: " << mc[i] << endl;
    }
    return mc;
}

