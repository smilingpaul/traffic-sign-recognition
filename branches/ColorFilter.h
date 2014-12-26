#pragma once
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>

using namespace cv;

class ColorFilter
{
public:
	void redBoost(Mat& src, Mat& dst);
	void doMask(Mat sourceImage, Mat &destinationImage);
	ColorFilter(void);
	~ColorFilter(void);
};

