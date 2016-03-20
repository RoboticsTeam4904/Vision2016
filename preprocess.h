#pragma once

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

cv::Mat thresholdImage(cv::Mat src, int threshold, bool debug);
