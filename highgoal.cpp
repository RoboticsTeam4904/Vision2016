#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <dirent.h>
// For availability of M_PI
#define _USE_MATH_DEFINES
#include <math.h>

#include "preprocess.h"
#include "findgoal.h"

using namespace cv;
using namespace std;

struct rect_points {
	Point side_one;
	Point side_two;
	Point side_three;
	Point side_four;
};

bool gui = false; // Turn on for debugging
bool detailedGUI = false;
bool test = false;
bool latest = false;
bool done = false;
bool existingGoal = false;

void analyzeImage(Mat src);


int getdir(string dir, vector<string> &files) {
	DIR *dp;
	struct dirent *dirp;
	if ((dp = opendir(dir.c_str())) == NULL) {
		cout << "Error opening directory '" << dir << "'" << endl;
		return -1;
	}

 	while ((dirp = readdir(dp)) != NULL) {
		files.push_back(string(dirp->d_name));
	}
	closedir(dp);
	return 0;
}

int main(int argc, char** argv) {
	string image = "latest.jpg";
	boolean debug = false;
	
	while(true){
		ctclient client;

		if(client.create("10.49.04.2", 9999) == 0){

			Mat input = loadImage();

			int threshold = 210;
			int max_threshold = 255;

			int blob_size = 5;
			int max_blob = 20;

			threshold_t thresholdData;
			threshold.src = &input;
			threshold.debug = debug;

			if(debug){
				namedWindow("Vision2016", CV_WINDOW_AUTOSIZE);
				imshow("preprocessed", preprocessed);
				createTrackbar("Threshold:", "Vision2016", &threshold, max_threshold, thresholdImage, &thresholdData);
			}
			else{
				thresholdImage(threshold, &thresholdData);
				findGoal(blob_size, &analyzeData);
			}

			Mat preprocessed = *thresholdData.src;
			float offAngle;
			float distance;
			boolean foundGoal

			analyze_t analyzeData;
			analyzeData.src = &preprocessed;
			analyzeData.debug = debug;
			analyzeData.offAngle = &offAngle;
			analyzeData.distance = &distance;
			analyzeData.foundGoal = &foundGoal;

			if(debug){
				createTrackbar("Blobsize:", "Vision2016", &blob_size, max_blob, findGoal, &analyzeData);

			}
			
			stringstream data;
			data << foundGoal << ":" << offAngle << ":" << distance << ":";
		
			client.c_write(data.str());

			client.c_close();
		}

	}

	return 0;
}

