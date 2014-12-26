#include "function.h"

int isCircle(double area, Point2f pMass, Point2f p)
{
	Point2f diff = pMass - p;

	double squaredR = diff.x*diff.x + diff.y*diff.y;
	double pi = area/squaredR;
	if ( abs(pi - CV_PI) < 1 )
	{
		return ceil(sqrt(squaredR)); // That's circle
	}

	return 0;
}

int isCircle(double area, Point2f pMass, vector<Point> p)
{
	double rmean = 0;

	for ( int i = 0; i < p.size(); i++)
	{
		double dx = pMass.x - p[i].x;
		double dy = pMass.y - p[i].y;

		double squaredR = dx*dx + dy*dy;
		rmean += ceil(sqrt(squaredR));
	}

	rmean /= p.size();
	double pi = area/(rmean*rmean);
	if ( abs(pi - CV_PI) < 0.3 )
	{
		return ceil(rmean); // That's circle
	}

	return 0;
}

void prepare(Mat& image)
{
	Mat dstImg;
	// Chuyen anh goc RGB ve HSV
	vector<Mat> hsv_chanel;
	cvtColor(image, dstImg, CV_RGB2HSV);
	// Tach HSV ra 3 kenh mau
	split(dstImg, hsv_chanel);
	// Can bang tren kenh S
	equalizeHist(hsv_chanel[1], hsv_chanel[1]);
	// Tron kenh mau
	merge(hsv_chanel, dstImg);
	cvtColor(dstImg, image, CV_HSV2RGB);
}

float pointDistance(Point2f a, Point2f b)
{
	float dx = (a.x - b.x)*(a.x - b.x);
	float dy = (a.y - b.y)*(a.y - b.y);

	float d = sqrt(dx+dy);
	return d;
}

void applyFilter(Mat src, Mat& bw)
{
	ColorFilter filter;
	Mat myMasked;
	//GaussianBlur( src, src, Size(9, 9), 4, 4 );
	filter.doMask(src, myMasked);
	vector<Mat> ch;

	filter.redBoost(src, bw);
	cvtColor(myMasked, myMasked, CV_BGR2GRAY);
	myMasked = myMasked >= 10;
	bitwise_and(bw, myMasked, bw);

	GaussianBlur( bw, bw, Size(9, 9), 4, 4 );
	
	imshow("Maskedbw", bw);
}