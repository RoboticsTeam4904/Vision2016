#include <thread>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <cstdlib>
#include <dirent.h>
// For availability of M_PI
#define _USE_MATH_DEFINES
#include <math.h>

#include "ctserver.h"

#include "config.h"
#include "preprocess.h"
#include "findgoal.h"

using namespace cv;
using namespace std;

Mat loadImage(){
	return imread("latest.jpg", CV_LOAD_IMAGE_COLOR);
}

void serveResults(int port, float * offAngle, float * distance, bool * foundGoal){

		ctserver server;
		server.create(port);

		while(true){
				server.getconn();

				stringstream data;

				data << *foundGoal << ":" << *offAngle << ":" << *distance;
				server.c_write(data.str());

				server.c_close();
		}
}

void debugUpdateThreshold(int thresh, void * thresholdPtr){
		*((int *) thresholdPtr) = thresh;
}

void debugUpdateBlobsize(int blobSize, void * blobSizePtr){
		*((int *) blobSizePtr) = blobSize;
}

void processImages(float * offAngle, float * distance, bool * foundGoal, bool debug){
		int thresh;
		int blobSize;

		if(debug){
				namedWindow("Vision2016", CV_WINDOW_AUTOSIZE);
				createTrackbar("Threshold:", "Vision2016", &thresh, maxThresh, NULL, NULL);	
				createTrackbar("Blobsize:", "Vision2016", &blobSize, maxBlobSize, NULL, NULL);
		}
		
		while(true){

				Mat inputImage = loadImage();

				Mat thresholdedImage = thresholdImage(inputImage, thresh, debug);

				if(debug){
						imshow("preprocessed", thresholdedImage);
				}

				goalPosition goal = findGoal(thresholdedImage, blobSize, debug);

				if(goal.foundGoal){
						*offAngle = goal.offAngle;
						*distance = goal.distance;
				}
				*foundGoal = goal.foundGoal;
		}
}

int main(int argc, char** argv) {
		bool debug = false;
		int port = 9999;

		float offAngle;
		float distance;
		bool foundGoal;

		thread server = thread(serveResults, port, &offAngle, &distance, &foundGoal);
		thread processor = thread(processImages, &offAngle, &distance, &foundGoal, debug);

		server.join();
		processor.join();
	
		return 0;
}
