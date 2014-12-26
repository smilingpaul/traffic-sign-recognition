#pragma once
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include "ColorFilter.h"

using namespace std;
using namespace cv;

typedef struct
{
	Point2f tam;
	double S;
	Rect bound;
} myTriangles;

typedef struct
{
	Point2f tam;
	double S;
	int R;
} myCircles;

int isCircle(double area, Point2f pMass, Point2f p);
int isCircle(double area, Point2f pMass, vector<Point> p);
void prepare(Mat& image);
float pointDistance(Point2f a, Point2f b);
void applyFilter(Mat src, Mat& src_gray);
