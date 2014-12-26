#pragma once
#include <iostream>
#include <vector>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "ColorTransformer.h"
#include "ColorFilter.h"
#include "function.h"

using namespace std;
using namespace cv;

class CSignTrafficDetect
{
	int edgeThresh;
	int lowThreshold;
	int ratio;
	int kernel_size;
	Mat detected_edges;
	Mat src;
	Mat src_gray;
	Mat output;
	vector<myCircles> circles;

public:
	CSignTrafficDetect(Mat& img, int edgeThres = 1, int lowThres = 50, int thresRatio = 3, int kerSize = 3)
		: src(img), edgeThresh(edgeThres), lowThreshold(lowThres), ratio(thresRatio), kernel_size(kerSize) {}
	~CSignTrafficDetect(void);

	vector<myTriangles> triangles;

	void detect();
	void edgeDetect();
	void findContour(vector<vector<Point> > &contours, vector<Vec4i> &hierarchy);
	void drawOutput();
	void preProcess();

};

