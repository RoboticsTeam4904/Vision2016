#include "findgoal.h"
#include "config.h"
#include <utility>

using namespace std;
using namespace cv;

struct rect_points {
		Point side_one;
		Point side_two;
		Point side_three;
		Point side_four;
};

pair<float,float> off_angle(int size_x, int size_y, rect_points goal) {
		float degPerPxlX = nativeAngleX / size_x;
		float degPerPxlY = nativeAngleY / size_y;
		float goalPixelY = size_y - (goal.side_two.y + goal.side_one.y + goal.side_three.y + goal.side_four.y) / 4;
		float goalAngleY = mountAngleY + degPerPxlY * (goalPixelY - size_y / 2);
		float goalPixelX = (goal.side_two.x + goal.side_one.x + goal.side_three.x + goal.side_four.x) / 4;
		float goalAngleX = mountAngleX + degPerPxlX * (goalPixelX - size_x / 2);
		float cameraDistance = (goalHeight - cameraHeight) / tan(goalAngleY);
		float shift = sqrt(shiftX * shiftX + shiftY * shiftY);
		float cameraAngle = M_PI - goalAngleX - atan(shiftX / shiftY);
		float distance = sqrt(cameraDistance * cameraDistance + shift * shift - 2 * cameraDistance * shift * cos(cameraAngle));
		float offAngle = asin(sin(cameraAngle) * cameraDistance / distance);
		offAngle += atan(shiftY / shiftX) - M_PI / 2;
		distance /= millimetersPerInch;
		return make_pair(offAngle, distance);
}

goalPosition findGoal(Mat src, int blobSize, bool debug) {

		int size_x = src.cols;
		int size_y = src.rows;

		vector<Point> poly, largest_contour;
		vector<Vec4i> hierarchy;
		Mat blobbed;
		Mat element = getStructuringElement(MORPH_ELLIPSE, Size(2 * blobSize + 1, 2 * blobSize + 1), Point(blobSize, blobSize));

		rect_points goal;
		vector<vector<Point> > contours;
		bool existingGoal;

		erode(src, blobbed, element);
		dilate(blobbed, blobbed, element);
		if (debug) imshow("blobbed", blobbed);
		findContours(blobbed, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		Mat result = src.clone();
		// Mat::zeros(blobbed.size(), CV_8UC3);

		if (contours.size()!=0) {
				existingGoal = true;
				double largest_area = 0.0;
				for (int i = 0; i < contours.size(); i++) {
						// Find the area of contour
						double a = contourArea(contours[i], false);
						if (a > largest_area) {
								largest_contour = contours[i];
								largest_area = a;
						}
				}
		}
		approxPolyDP(Mat(largest_contour), poly, 3, true);
		goal.side_one = poly[0];
		goal.side_two = poly[1];
		goal.side_three = poly[2];
		goal.side_four = poly[3];

		if (debug) {
				line(result, goal.side_one, goal.side_two, Scalar(255, 0, 0), 5);
				line(result, goal.side_two, goal.side_three, Scalar(255, 0, 0), 5);
				line(result, goal.side_three, goal.side_four, Scalar(255, 0, 0), 5);
				line(result, goal.side_four, goal.side_one, Scalar(255, 0, 0), 5);
				imshow("Vision2016", result);
		}

		goalPosition position;
		position.foundGoal = existingGoal;

		if (existingGoal) {
				pair<float,float> tempvar = off_angle(size_x, size_y, goal);
				position.offAngle = tempvar.first;
				position.distance = tempvar.second;
		}

		return position;
}
