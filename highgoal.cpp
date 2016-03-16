#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <dirent.h>
// For availability of M_PI
#define _USE_MATH_DEFINES
#include <math.h>

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

pair<float,float> off_angle();

// CONSTANTS
// Distances are in milimeters, angles are in degrees
const float mountAngleX = 0.0;
const float mountAngleY = 45.0 * M_PI / 180;
const int nativeResX = 2592;
const int nativeResY = 1944;
const float nativeAngleX = 53.5 * M_PI / 180;
const float nativeAngleY = 41.41 * M_PI / 180;
const float shiftX = 336.55; // 13.25 inches
const float shiftY = 57.15; // 2.5 inches
const float goalHeight = 2292.35; // 7.5 feet
const float cameraHeight = 296.0; // 296 milimeters
const float millimetersPerInch = 25.4;

rect_points goal;
vector<vector<Point> > contours;

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

			thresholdData_t thresholdData;
			thresholdData.src = &input;
			thresholdData.debug = debug;

			if(debug){
				namedWindow("Vision2016", CV_WINDOW_AUTOSIZE);
				imshow("preprocessed", preprocessed);
				createTrackbar("Threshold:", "Vision2016", &threshold, max_threshold, thresholdImage, &thresholdData);
				// createTrackbar("Blobsize:", "Vision2016",
			}
			else{
				thresholdImage(threshold, thresholdData);
			}

			Mat preprocessed = *thresholdData.src;


			float offAngle;
			float distance;
			boolean existingGoal;

			findGoal(preprocessed, &existingGoal, &offAngle, &distance);

			stringstream data;
			data << existingGoal << ":" << offAngle << ":" << distance << ":";
		
			client.c_write(data.str());

			client.c_close();
		}

	}
		

	if (argc == 1) {
		detailedGUI = true;
	} else if (argc == 2) {
		if (strcmp(argv[1], "test") == 0) {
			gui = false;
			test = true;

			cout << "*********************************" << endl;
			cout << "Testing mode" << endl;
			cout << "*********************************" << endl << endl;
		} else if (strcmp(argv[1], "latest") == 0) {
			latest = true;
			gui = false;
			image = "latest.jpg";
		} else {
			image = argv[1];
		}
	} else if (argc == 3) {
		if (strcmp(argv[1], "folder") == 0) {
			vector<string> files = vector<string>();
			string dir = argv[2];
			if (dir.substr(dir.size() - 1, 1) != "/") {
				dir += "/";
			}

			int status = getdir(dir, files);

			if (status >= 0) {
				cout << "Reading directory '" << dir << "'" << endl;
			} else {
				return status;
			}

			for (unsigned int i = 0; i < files.size(); i++) {
				if (files[i].length() < 4 || files[i].substr(files[i].length() - 4, 4) != ".jpg") continue;
				string path = dir + files[i];

				src = imread(path, CV_LOAD_IMAGE_UNCHANGED);
				if (src.empty()) {
					cout << "Error: Image '" << path << "' cannot be loaded" << endl;
					return -1;
				}
				cout << "Loaded image '" << files[i] << "'" << endl;
				analyzeImage(src);

				waitKey(0);
				done = true;
			}
		}
	}

	if (!done) {
		src = imread(image, CV_LOAD_IMAGE_UNCHANGED);
		if (src.empty()) {
			cout << "Error: Image '" << image << "' cannot be loaded" << endl;
			return -1;
		}
		analyzeImage(src);
	}
	return 0;
}

