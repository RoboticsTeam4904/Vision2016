#include "preprocess.h"

using namespace cv;
using namespace std;

struct threshold_t{
	Mat * src;
	bool debug;
};

void thresholdImage(int threshold, void * data){

	threshold_t thresholdData = *((struct threshold_t *) data);
	Mat src = *thresholdData.src;
	bool debug = thresholdData.debug;
	
	cvtColor(src, src_gray, CV_BGR2GRAY);
	blur(src_gray, src_gray, Size(3, 3));

	
	Mat threshold_output, convex;
	vector<Vec4i> hierarchy;

	threshold(src_gray, threshold_output, thresh, max_thresh, THRESH_BINARY);
	if (debug) imshow("threshold", threshold_output);
	findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<vector<Point> > hull(contours.size());
	for(int i = 0; i < contours.size(); ++i) {
		convexHull(Mat(contours[i]), hull[i], false);
	}

	convex = Mat::zeros( threshold_output.size(), CV_8UC1 );
	for (int i = 0; i<contours.size(); ++i) {
		drawContours(convex, hull, i, Scalar(255,255,255), CV_FILLED, 8, vector<Vec4i>(), 0, Point() );
	}

	subtracted = Mat::zeros(convex.size(), CV_8UC1);

	if (convex.isContinuous() && threshold_output.isContinuous()) {
		uchar *p1, *p2, *p3;
		p1 = convex.ptr<uchar>(0);
		p2 = threshold_output.ptr<uchar>(0);
		p3 = subtracted.ptr<uchar>(0);
		for (int i = 0; i < convex.rows * convex.cols; ++i) {
			if (*p2 != 0){
				*p3 = 0;
			} else if (*p1 != 0){
				*p3 = 255;
			}
			p1++;
			p2++;
			p3++;
		}

	}
	// subtract(convex, threshold_output, subtracted);

	thresholdData.src = &threshold_output;

	if (debug) {
		imshow("convex", convex);
		imshow("subtracted", subtracted);
	}
}