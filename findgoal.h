#pragma once

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

struct goalPosition{
		bool foundGoal;
		float offAngle;
		float distance;
};

goalPosition findGoal(cv::Mat src, int blobSize, bool debug);
