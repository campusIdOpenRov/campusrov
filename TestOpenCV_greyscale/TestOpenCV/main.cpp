#include <iostream>
#include <vector>
#include<stdio.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/video/tracking.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace cv::ogl;
using namespace std;

char mouseController[] = "Raw Video";
char objWindow[] = "Object Window";
char scribbleWindow[] = "Scribble Window";
char resultWindow[] = "Mouse Controller";
int main() {
	static int lposX, lposY, llastX, llastY, rposX, rposY, rlastX, rlastY; //To hold the X and Y position of tracking color object
	int i, j;
	Mat rgbCameraFrames; //Matrix to hold Raw frames from Webcam in RGB color space
	Mat colorTrackingFrames; //Matrix to hold color filtered Frames in GRAY color space
	Mat leftImage;
	Mat rightImage;
	//Texture2D* test;
	Rect* leftROI;
	Rect* rightROI;
	int default_x_distance = 300; // default distance separating de red dots when objects reflecting is at 50cm from them
	float default_distance = 31; // distance in CM when default_x_distance = 550px;
	VideoCapture capture; //Used to access the default camera in your machine
	capture.open(0); //open the camera reading
	//namedWindow(mouseController, CV_WINDOW_AUTOSIZE); //Can be used to change the window properties
	//VideoCapture capture("F:/Workspace/C#/TestOpenCV/Debug/test.avi");//only for test purpose
	CvMoments colorMomentLeft; //Structure to hold moments information and their order
	CvMoments colorMomentRight; //Structure to hold moments information and their order
	capture >> rgbCameraFrames; //Simply by overloading the operator we copy the Camera frames to our rgbCameraFrames object
	assert(rgbCameraFrames.type() == CV_8UC3);
	//Checking whether the color space is in RGB space

	cout<<"Hello"<<std::endl;
	while (1) {
		capture >> rgbCameraFrames; //Each frames is copied to our rgbCameraFrames object
		//resize(rgbCameraFrames, rgbCameraFrames, Size(rgbCameraFrames.cols/2, rgbCameraFrames.rows/2));
		//GaussianBlur(rgbCameraFrames, hsv, Size(3, 3), 0, 0); //Just a filter to reduce the noise
		//           B  G  R     B   G    R
		//cvtColor(rgbCameraFrames, colorTrackingFrames, CV_BGR2YUV); //CV_BGR2HSV CV_BGR2HLS CV_BGR2GRAY		
		cvtColor(rgbCameraFrames, colorTrackingFrames, CV_BGR2GRAY);
		//cvtColor(colorTrackingFrames, colorTrackingFrames, CV_BGR2HSV);
		//cvtColor(colorTrackingFrames, colorTrackingFrames, CV_BGR2HLS);
		//cvtColor(colorTrackingFrames, colorTrackingFrames, CV_BGR2GRAY);
		//pull two region of interest out of the image
		colorTrackingFrames(Rect(0,colorTrackingFrames.rows/3,colorTrackingFrames.cols/2,colorTrackingFrames.rows/3)).copyTo(leftImage);
		colorTrackingFrames(Rect(colorTrackingFrames.cols/2,colorTrackingFrames.rows/3,colorTrackingFrames.cols/2,colorTrackingFrames.rows/3)).copyTo(rightImage);

		inRange(rightImage, (240, 240, 240), (255, 255, 255),// Adjust the scalar values for different color
			rightImage); //We make RED color pixels to white and other colors to black

		inRange(leftImage, (240, 240, 240), (255, 255, 255),// Adjust the scalar values for different color
			leftImage); //We make RED color pixels to white and other colors to black

		colorMomentLeft = moments(leftImage); //We give the binary converted frames for calculating the moments
		double momentLeft10 = cvGetSpatialMoment(&colorMomentLeft, 1, 0); //Sum of X coordinates of all white pixels
		double momentLeft01 = cvGetSpatialMoment(&colorMomentLeft, 0, 1); //Sum of Y coordinates of all white pixels
		double areaLeft = cvGetCentralMoment(&colorMomentLeft, 0, 0); //Sum of all white color pixels

		colorMomentRight = moments(rightImage); //We give the binary converted frames for calculating the moments
		double momentRight10 = cvGetSpatialMoment(&colorMomentRight, 1, 0); //Sum of X coordinates of all white pixels
		double momentRight01 = cvGetSpatialMoment(&colorMomentRight, 0, 1); //Sum of Y coordinates of all white pixels
		double areaRight = cvGetCentralMoment(&colorMomentRight, 0, 0); //Sum of all white color pixels

		llastX = lposX;
		llastY = lposY;
		lposX = (momentLeft10 / areaLeft);
		lposY = (((momentLeft01) / (areaLeft)) + colorTrackingFrames.rows/3); //Simple logic which you can understand

		if (lposX > 0 && lposY > 0 && llastX > 0 && llastY > 0) {
			line(rgbCameraFrames, cvPoint(lposX, lposY), cvPoint(llastX, llastY),
				cvScalar(0, 255, 255), 5); //To draw a yellow point on the center of the colored object
			//	//cvPoint is used to create a Point data type which holds the pixel location
		}

		rlastX = rposX;
		rlastY = rposY;

		rposX = (momentRight10/ areaRight)+colorTrackingFrames.cols/2;
		rposY = (((momentRight01) / (areaRight))  +colorTrackingFrames.rows/3) ;
		int middleSpaceLeft = (rgbCameraFrames.cols/2)-lposX;
		int middleSpaceRight = rposX-(rgbCameraFrames.cols/2);
		int extrapolatedLeftPosX = (rgbCameraFrames.cols/2) + middleSpaceLeft;
		int extrapolatedRightPosX = (rgbCameraFrames.cols/2) - middleSpaceRight;
		int extrapolatedDistanceLeft = extrapolatedLeftPosX - lposX;
		int extrapolatedDistanceRight  = rposX - extrapolatedRightPosX;
		int extrpolatedDistanceDifference = extrapolatedDistanceRight - extrapolatedDistanceLeft;
		if (rposX > 0 && rposY > 0 && rlastX > 0 && rposY > 0) {
			if( extrpolatedDistanceDifference > 15 || extrpolatedDistanceDifference < -15){
				float ratioL = (float)default_x_distance / (float)extrapolatedDistanceLeft;
				float ratioR = (float)default_x_distance / (float)extrapolatedDistanceRight;
				float distance_in_mL = ratioL * (float)default_distance;
				float distance_in_mR = ratioR * (float)default_distance;
				cout<<"left  x									  :"<<lposX<<std::endl;
				cout<<"right x									  :"<<rposX<<std::endl;
				cout<<"distance between extrapolated points left  :"<<extrapolatedDistanceLeft<<std::endl;
				cout<<"distance between extrapolated points right :"<<extrapolatedDistanceRight<<std::endl;
				cout<<"distance extrapolated left point           :"<<distance_in_mL<<std::endl;
				cout<<"distance extrapolated right point          :"<<distance_in_mR<<std::endl;
			}
			else{
				int distance_x = rposX-lposX;
				float ratio = (float)default_x_distance / (float)distance_x;
				float distance_in_m = ratio * (float)default_distance;
				cout<<"left  x                 :"<<lposX<<std::endl;
				cout<<"right x                 :"<<rposX<<std::endl;
				cout<<"distance between points :"<<distance_x<<std::endl;
				cout<<"distance                :"<<distance_in_m<<std::endl;
			}
			line(rgbCameraFrames, cvPoint(rposX, rposY), cvPoint(rlastX, rlastY),
				cvScalar(255, 0, 255), 5); //To draw a yellow point on the center of the colored object
			//	//cvPoint is used to create a Point data type which holds the pixel location
		}
		imshow(mouseController, rgbCameraFrames);
		imshow("",colorTrackingFrames);
		imshow("left", leftImage);
		imshow("right", rightImage);
		waitKey(1); // OpenCV way of adding a delay, generally used to get a Key info.
		leftImage.release();
		rightImage.release();
		colorTrackingFrames.release();
	}
	return 0;
}
///*int main(){
//Mat image;
//Mat gray;
//Mat hsv;
//VideoCapture cap;
//cap.open(0);
//namedWindow("window", CV_WINDOW_AUTOSIZE);
//namedWindow("gray", CV_WINDOW_AUTOSIZE);
//namedWindow("hsv", CV_WINDOW_AUTOSIZE);
//
//while (1){
//cap >> image;
//cvtColor(image, gray, CV_BGR2GRAY);
//cvtColor(image, hsv, CV_BGR2HSV);
//imshow("window", image);
//imshow("gray", gray);
//imshow("hsv", hsv);
//waitKey(33);
//}
//return 0;
//}*/


/*
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <ctype.h>

using namespace cv;
using namespace std;

Point2f pt;
bool addRemovePt = false;

void onMouse( int event, int x, int y, int flags, void* param )
{
if( event == CV_EVENT_LBUTTONDOWN )
{
pt = Point2f((float)x,(float)y);
addRemovePt = true;
}
}

int main( int argc, char** argv )
{
VideoCapture cap;
TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03);
Size winSize(10,10);

const int MAX_COUNT = 500;
bool needToInit = false;
bool nightMode = false;

if( argc == 1 || (argc == 2 && strlen(argv[1]) == 1 && isdigit(argv[1][0])))
cap.open(argc == 2 ? argv[1][0] - '0' : 0);
else if( argc == 2 )
cap.open(argv[1]);

if( !cap.isOpened() )
{
cout << "Could not initialize capturing...\n";
return 0;
}

// print a welcome message, and the OpenCV version
cout << "Welcome to lkdemo, using OpenCV version %s\n" << CV_VERSION;

cout << "\nHot keys: \n"
"\tESC - quit the program\n"
"\tr - auto-initialize tracking\n"
"\tc - delete all the points\n"
"\tn - switch the \"night\" mode on/off\n"
"To add/remove a feature point click it\n";

namedWindow( "LK Demo", 1 );
setMouseCallback( "LK Demo", onMouse, 0 );

Mat gray, prevGray, image;
vector<Point2f> points[2];

for(;;)
{
Mat frame;
cap >> frame;
if( frame.empty() )
break;

frame.copyTo(image);
cvtColor(image, gray, CV_BGR2GRAY); 

if( nightMode )
image = Scalar(0,0,0);

if( needToInit )
{
goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
cornerSubPix(gray, points[1], winSize, Size(-1,-1), termcrit);
addRemovePt = false;
}
else if( !points[0].empty() )
{
vector<uchar> status;
vector<float> err;
if(prevGray.empty())
gray.copyTo(prevGray);
calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize,
3, termcrit, 0);
size_t i, k;
for( i = k = 0; i < points[1].size(); i++ )
{
if( addRemovePt )
{
if( norm(pt - points[1][i]) <= 5 )
{
addRemovePt = false;
continue;
}
}

if( !status[i] )
continue;

points[1][k++] = points[1][i];
circle( image, points[1][i], 3, Scalar(0,255,0), -1, 8);
}
points[1].resize(k);
}

if( addRemovePt && points[1].size() < (size_t)MAX_COUNT )
{
vector<Point2f> tmp;
tmp.push_back(pt);
cornerSubPix( gray, tmp, winSize, cvSize(-1,-1), termcrit);
points[1].push_back(tmp[0]);
addRemovePt = false;
}

needToInit = false;
imshow("LK Demo", gray);

char c = (char)waitKey(10);
if( c == 27 )
break;
switch( c )
{
case 'r':
needToInit = true;
break;
case 'c':
points[1].clear();
break;
case 'n':
nightMode = !nightMode;
break;
default:
;
}

std::swap(points[1], points[0]);
swap(prevGray, gray);
}

return 0;
}
*/