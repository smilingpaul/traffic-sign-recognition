#include "ColorFilter.h"

void ColorFilter::redBoost(Mat& src, Mat& dst)
{

	Mat imR(src.rows, src.cols, CV_8UC1);
	Mat imG(src.rows, src.cols, CV_8UC1);
	Mat imB(src.rows, src.cols, CV_8UC1);
	Mat imRboost(src.rows, src.cols, CV_8UC1);

	Mat out[] = {imR, imG, imB};
	int from_to[] = {2,0  , 1, 1,  0, 2 };
	mixChannels(&src, 1, out, 3, from_to, 3);

	bitwise_not(imG, imG);
	bitwise_not(imB, imB);

	multiply(imR, imG, imRboost, (double)1/255);
	multiply(imRboost, imB, imRboost, (double)1/255);

	dst = imRboost.clone();

	dst = dst >= 10;
	
	blur( dst, dst, cv::Size(3,3) );

}

/**
* @function doMask
* @brief return masked color image with blue and red color.
*/
void ColorFilter::doMask(Mat sourceImage, Mat &destinationImage)
{
	Mat srcHSV, masksrc;
	cvtColor( sourceImage, srcHSV, CV_BGR2HSV );
	Mat maskR1, maskR2, maskR, maskB, maskRB;
	inRange(srcHSV, Scalar(0, 10, 20), Scalar(15, 255, 255), maskR1); 
	inRange(srcHSV, Scalar(150, 10, 20), Scalar(179, 255, 255), maskR2);
	//inRange(srcHSV, Scalar(100, 10, 10), Scalar(135, 255, 255), maskB);
	bitwise_or(maskR1, maskR2, maskR);
	//bitwise_or(maskR, maskB, maskRB);
	bitwise_and(srcHSV, srcHSV, masksrc, maskR);
	cvtColor( masksrc, destinationImage, CV_HSV2BGR );
}

ColorFilter::ColorFilter(void)
{
}


ColorFilter::~ColorFilter(void)
{
}
