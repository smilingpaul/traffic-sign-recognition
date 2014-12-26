#pragma once
#include "opencv2\core\core.hpp"

class ColorTransformer
{
public:
	/*
	Hàm nhận vào một ảnh, thay đổi độ sáng của ảnh này và lưu kết quả vào ảnh mới
	Tham so:
		sourceImage	: ảnh ban đầu
		destinationImage: ảnh kết quả
		b	: giá trị số nguyên dùng để thay đổi độ sáng của ảnh
	Hàm trả về:
		1: Nếu thành công thì trả về ảnh kết quả (ảnh gốc vẫn giữ nguyên giá trị)
		0: Nếu không tạo được ảnh kết quả hoặc ảnh input không tồn tại
	*/
	int ChangeBrighness(IplImage* sourceImage, IplImage* destinationImage, short b);
	int ChangeBrighness(cv::Mat& sourceImage, cv::Mat& destinationImage, short b);

	/*
	Hàm nhận vào một ảnh, thay đổi độ tương phản của ảnh này và lưu kết quả vào ảnh mới
	Tham so :
		sourceImage : ảnh ban đầu
		destinationImage : ảnh kết quả
		c	: giá trị số thực dùng để thay đổi độ tương phản của ảnh
	Hàm trả về:
		1: Nếu thành công thì trả về ảnh kết quả (ảnh gốc vẫn giữ nguyên giá trị)
		0: Nếu không tạo được ảnh kết quả hoặc ảnh input không tồn tại
	*/
	int ChangeContrast(IplImage* sourceImage, IplImage* destinationImage, float c);
	int ChangeContrast(cv::Mat& sourceImage, cv::Mat& destinationImage, float c);
	
	/*
	Hàm nhận vào một ảnh, thay đổi độ tương phản của ảnh này và lưu kết quả vào ảnh mới
	Tham so :
		sourceImage : ảnh ban đầu
		destinationImage : ảnh kết quả
		c,d	: cực trị mức xám của ảnh mới ([min max] -> [c d])
	Hàm trả về:
		1: Nếu thành công thì trả về ảnh kết quả (ảnh gốc vẫn giữ nguyên giá trị)
		0: Nếu không tạo được ảnh kết quả hoặc ảnh input không tồn tại
	*/
	int StretchContrast(IplImage* sourceImage, IplImage* destinationImage, uchar c, uchar d);
	int StretchContrast(cv::Mat& sourceImage, cv::Mat& destinationImage, uchar c, uchar d);

	/*
	Hàm cân bằng lược đồ màu tổng quát cho ảnh bất kỳ
	Tham so :
		sourceImage : ảnh ban đầu
		destinationImage : ảnh kết quả
	Hàm trả về:
		1: Nếu thành công thì trả về ảnh kết quả (ảnh gốc vẫn giữ nguyên giá trị)
		0: Nếu không tạo được ảnh kết quả hoặc ảnh input không tồn tại
	*/
	int HistogramEqualization(IplImage* sourceImage, IplImage* destinationImage);
	int HistogramEqualization(cv::Mat& sourceImage, cv::Mat& destinationImage);

	/*
	//Hàm khớp lược đồ ảnh xám (Histogram specification)
	Tham so :
		sourceImage : ảnh ban đầu (kiểm tra ảnh xám)
		referenceImage: ảnh tham chiếu (kiểm tra ảnh xám)
		destinationImage : ảnh kết quả
	Hàm trả về:
		1: Nếu thành công thì trả về ảnh kết quả (ảnh gốc vẫn giữ nguyên giá trị)
		0: Nếu không tạo được ảnh kết quả hoặc ảnh input không tồn tại
	*/
	int HistogramSpecification(IplImage* sourceImage, IplImage* referenceImage, IplImage* destinationImage);
	int HistogramSpecification(cv::Mat& sourceImage, cv::Mat& referenceImage, cv::Mat& destinationImage);


	int ChangeLogarithm(cv::Mat& sourceImage, cv::Mat& destinationImage, float c);
	int ChangeExponential(cv::Mat& sourceImage, cv::Mat& destinationImage, float gama);
	

	ColorTransformer();
	~ColorTransformer();
};

