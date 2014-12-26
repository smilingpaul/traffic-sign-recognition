#include "ColorTransformer.h"
#include "opencv\cv.h"
#include <vector>

using namespace std;
using namespace cv;

int ColorTransformer::ChangeBrighness(IplImage* sourceImage, IplImage* destinationImage, short b)
{
	if (sourceImage == NULL)
		return 0;

	destinationImage = cvCreateImage(cvGetSize(sourceImage), sourceImage->depth, sourceImage->nChannels);
	cvAddS(sourceImage, cvScalar(b, b, b), destinationImage);

	return 1;
}

int ColorTransformer::ChangeBrighness(cv::Mat& sourceImage, cv::Mat& destinationImage, short b)
{
	if (sourceImage.empty())
		return 0;

	sourceImage.convertTo(destinationImage, -1, 1, b);

	return 1;
}

int ColorTransformer::ChangeContrast(IplImage* sourceImage, IplImage* destinationImage, float c)
{
	if (sourceImage == NULL)
		return 0;

	destinationImage = cvCreateImage(cvGetSize(sourceImage), sourceImage->depth, sourceImage->nChannels);
	cvConvertScale(sourceImage, destinationImage, c, 0);

	return 1;
}

int ColorTransformer::ChangeContrast(cv::Mat& sourceImage, cv::Mat& destinationImage, float c)
{
	if (sourceImage.empty())
		return 0;

	sourceImage.convertTo(destinationImage, -1, c, 0);

	return 1;
}


int ColorTransformer::StretchContrast(IplImage* sourceImage, IplImage* destinationImage, uchar c, uchar d)
{
	if (sourceImage == NULL)
		return 0;

	destinationImage = cvCreateImage(cvGetSize(sourceImage), sourceImage->depth, sourceImage->nChannels);
	cvAddS(sourceImage, cvScalar(-c, -c, -c), destinationImage);
	cvConvertScale(destinationImage, destinationImage, 255 / (d - c), 0);

	return 1;
}

int ColorTransformer::StretchContrast(cv::Mat& sourceImage, cv::Mat& destinationImage, uchar c, uchar d)
{
	if (sourceImage.empty())
		return 0;

	// https://www.scribd.com/doc/106715203/OPENCV-Topic-03-Contrast-Stretching
	// http://ict.udlap.mx/people/oleg/docencia/IMAGENES/chapter3/image_311_IS548.html
	// http://www.programming-techniques.com/2013/01/contrast-stretching-using-c-and-opencv.html
	// *http://azimcemimrag.blogspot.com/2013/03/contrast-stretching.html
	// http://www.google.com/patents/EP1323132A2?cl=en
	// having Normalization
	destinationImage = (sourceImage - c) * (255 / (d - c));

	return 1;
}

void EqualizeHist(Mat& sourceImage, Mat& destinationImage)
{
	int nRow = sourceImage.rows;
	int nCol = sourceImage.cols;
	float factors = nRow*nCol;
	MatIterator_<uchar> it;
	int nG = 0;
	//b1
	for (it = sourceImage.begin<uchar>(); it != sourceImage.end<uchar>(); ++it)
		if (*it > nG)
			nG = *it;
	vector<float> H(nG + 1, 0);

	//b2
	for (it = sourceImage.begin<uchar>(); it != sourceImage.end<uchar>(); ++it)
		H[*it]++;
	//b3
	vector<float> T(nG + 1, 0);
	T[0] = H[0];
	for (int i = 1; i < nG; i++)
		T[i] = T[i - 1] + H[i];
	//b4
	for (int i = 0; i < nG; i++)
		T[i] = (((nG)*1.0 / (factors))*T[i]);
	//b5

	for (it = destinationImage.begin<uchar>(); it != destinationImage.end<uchar>(); ++it)
	{
		*it = (int)T[*it];
	}
}

int ColorTransformer::HistogramEqualization(Mat& sourceImage, Mat& destinationImage)
{
	if (sourceImage.empty())
		return 0;
	if (sourceImage.channels() == 1)
		equalizeHist(sourceImage, destinationImage);
	else
	{
		Mat ycrcb;

		cvtColor(sourceImage, ycrcb, CV_BGR2YCrCb);

		vector<Mat> channels;
		split(ycrcb, channels);

		EqualizeHist(channels[0], channels[0]);
		
		Mat result;
		merge(channels, ycrcb);

		cvtColor(ycrcb, result, CV_YCrCb2BGR);

		destinationImage = result;
	}
	return 1;
}



int ColorTransformer::HistogramSpecification(cv::Mat& sourceImage, cv::Mat& referenceImage, cv::Mat& destinationImage)
{
	if (sourceImage.channels() != 1 || referenceImage.channels() != 1)
		return 0;

	int nSRow = sourceImage.rows;
	int nSCol = sourceImage.cols;
	int nRRow = referenceImage.rows;
	int nRCol = referenceImage.cols;

	uchar* p = sourceImage.data;
	int nMax = 255;

	float sFactor = sourceImage.rows*sourceImage.cols;
	float rFactor = referenceImage.rows*referenceImage.cols;

	//Step 1
	vector<float> Hf(nMax + 1, 0);  //histogram f
	vector<float> Hg(nMax + 1, 0); //histogram g
	//Step 2

	uchar* pSrc = sourceImage.data;
	uchar* pRef = referenceImage.data;

	for (int i = 0; i < nSRow; i++)
	{
		for (int j = 0; j < nSCol; j++)
		{
			Hf[pSrc[j]] += 1;
		}

		pSrc += sourceImage.step;
	}

	for (int i = 0; i < nRRow; i++)
	{
		for (int j = 0; j < nRCol; j++)
		{
			Hg[pRef[j]] += 1;
		}
		pRef += referenceImage.step;
	}

	//Step 3, 4, 5, 6
	vector<float> T(nMax + 1);
	vector<float> G(nMax + 1);
	T[0] = Hf[0] / sFactor;
	G[0] = Hg[0] / rFactor;

	for (int i = 1; i <= nMax - 1; i++)
	{
		T[i] = T[i - 1] + Hf[i] / sFactor;
		G[i] = G[i - 1] + Hg[i] / rFactor;
	}

	T[nMax] = 1;
	G[nMax] = 1;

	//Step 7
	destinationImage = sourceImage.clone();

	vector<int> tables(nMax + 1, 0);
	int currentID = 0;
	for (int i = 1; i < nMax + 1; i++)
	{
		bool mapped = false;
		while (mapped == false && currentID < nMax + 1)
		{
			if (T[i] <= G[currentID])
			{
				tables[i] = currentID;
				mapped = true;
			}
			else
			{
				currentID++;
			}
		}
	}

	p = sourceImage.data;
	uchar* p2 = destinationImage.data;
	for (int k = 0; k < nSRow; k++)
	{
		for (int j = 0; j < nSCol; j++)
		{
			p2[j] = tables[(int)p[j]];
		}
		p += sourceImage.step;
		p2 += destinationImage.step;
	}

	return 1; //SUCCESS
}

int ColorTransformer::HistogramSpecification(IplImage* sourceImage, IplImage* referenceImage, IplImage* destinationImage)
{
	Mat srcImg(sourceImage, true);
	Mat refImg(referenceImage, true);
	Mat dstImg;
	int result = this->HistogramSpecification(srcImg, refImg, dstImg);

	IplImage temp = dstImg;
	destinationImage = new IplImage(temp);

	return result;
}

int ColorTransformer::ChangeLogarithm(cv::Mat& sourceImage, cv::Mat& destinationImage, float c)
{
	if (sourceImage.empty() || destinationImage.empty())
		return 0;
	destinationImage.convertTo(destinationImage, CV_32F);//chuyển các giá trị điểm ảnh về kiểu số thực (float)
	log(destinationImage, destinationImage);//f = log(f)
	destinationImage = c*destinationImage;//f=f*c
	destinationImage.convertTo(destinationImage, CV_8U);
	normalize(destinationImage, destinationImage, 0, 255, NORM_MINMAX);//đặt giá trị ảnh về đoạn [0,255]
	return 1;
}

int ColorTransformer::ChangeExponential(cv::Mat& sourceImage, cv::Mat& destinationImage, float gama)
{
	if (sourceImage.empty() || destinationImage.empty())
		return 0;
	destinationImage.convertTo(destinationImage, CV_32F);
	pow(destinationImage / 255, gama, destinationImage);
	destinationImage = destinationImage * 255;
	convertScaleAbs(destinationImage, destinationImage);
	normalize(destinationImage, destinationImage, 0, 255, NORM_MINMAX);
	return 1;
}

ColorTransformer::ColorTransformer()
{
}

ColorTransformer::~ColorTransformer()
{
}

int findIndex(vector<int> a, int val)
{
	int sum = 0;
	int count = 0;
	for (int i = 0; i < a.size(); i++)
	{
		if (a[i] == val)
		{
			sum += i;
			count++;
		}
	}

	if (sum == 0)
		return 0;

	return sum / count;
}
