#include "SignTrafficDetect.h"

CSignTrafficDetect::~CSignTrafficDetect(void)
{
}

void CSignTrafficDetect::preProcess()
{
	output = src.clone();

	prepare(src);

	applyFilter(src, src_gray);

}

void CSignTrafficDetect::edgeDetect()
{
	/// Reduce noise with a kernel 9x9

	GaussianBlur( src_gray, detected_edges, Size(9,9), 2, 2);
	/// Canny detector
	Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size, true );
}

void CSignTrafficDetect::findContour(vector<vector<Point> > &contours, vector<Vec4i> &hierarchy)
{
	/// Find Contours
	findContours( detected_edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_TC89_KCOS, Point(0, 0) );
}

void CSignTrafficDetect::detect()
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	preProcess();
	edgeDetect();
	findContour(contours, hierarchy);

	/// Using Canny's output as a mask, we display our result
	//dst = Scalar::all(0);
	//src.copyTo( dst, detected_edges);

	/// Get the moments
	vector<Moments> mu( contours.size() );
	for( int i = 0; i < contours.size(); i++ )
	{ 
		mu[i] = moments( contours[i], false ); 
	}

	///  Get the mass centers:
	vector<Point2f> mc( contours.size() );
	for( int i = 0; i < contours.size(); i++ )
	{ 
		mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ); 
	}

	/// Draw contours
	Mat drawing = Mat::zeros( src.size(), CV_8UC3 );

	for( int i = 0; i< contours.size(); i++ )
	{
		vector<Point> approx;

		Rect boundRect = boundingRect( Mat(contours[i]) );

		approxPolyDP( contours[i], approx, arcLength(Mat(contours[i]), true)*0.15, true );

		//cout << "apsize: " << approx.size() << ", R =  " << isCircle(mu[i].m00, mc[i], contours[i]) << ", S = " << mu[i].m00 << endl;

		if ( approx.size() >= 3 && approx.size() <= 3 && mu[i].m00 > 200 && !isCircle(mu[i].m00, mc[i], contours[i]) )
		{
			drawContours( drawing, contours, i, Scalar(255, 255, 255), 2, 8, hierarchy, 0, Point() );			
			//cout << " tam giac: S = " << mu[i].m00 << ", size = " << approx.size() << ", tam: " << mc[i] << endl;

			bool flag = false;

			for (int k = 0; k < triangles.size(); k++)
			{
				if (pointDistance(triangles[k].tam, mc[i]) < 2)
				{				
					flag = true;
					if (mu[i].m00 >= triangles[k].S)
					{
						triangles[k].S = mu[i].m00;
						triangles[k].tam = mc[i];
						triangles[k].bound = boundRect;
					}
					break;
				}
			}

			if (flag == false)
			{
				myTriangles tmp;
				tmp.bound = boundRect;
				tmp.S = mu[i].m00;
				tmp.tam = mc[i];
				triangles.push_back(tmp);			
			}


		}
		else
		{
			int radius;
			if ( approx.size() >= 4 && (radius = isCircle(mu[i].m00, mc[i], contours[i])) && radius > 15 )
			{
				drawContours( drawing, contours, i, Scalar(100, 100, 0), 2, 8, hierarchy, 0, Point() );				
				//cout << "hinh tron: S = " << mu[i].m00 << ", size = " << approx.size() << ", R = " << radius << ", tam: " << mc[i] << endl;

				int flag = false;

				for (int k = 0; k < circles.size(); k++)
				{
					if (pointDistance(circles[k].tam, mc[i]) < 1.3)
					{
						flag = true;
						if (mu[i].m00 > circles[k].S)
						{
							circles[k].S = mu[i].m00;
							circles[k].tam = mc[i];
							circles[k].R = radius;
						}
						break;
					}
				}

				if (flag == false)
				{
					myCircles tmp;					
					tmp.S = mu[i].m00;
					tmp.tam = mc[i];
					tmp.R = radius;
					circles.push_back(tmp);
				}

			}
			else
			{
				drawContours( drawing, contours, i, Scalar(110, 50, 0), 2, 8, hierarchy, 0, Point() );
			}
		}
	}

	imshow("Contours", drawing);
}

void CSignTrafficDetect::drawOutput()
{
	for (int i = 0; i < triangles.size(); i++)
	{
		rectangle( output, triangles[i].bound.tl(), triangles[i].bound.br(), Scalar(255, 255, 0), 2, 8, 0 );
		cout << triangles[i].S << "," << triangles[i].tam << endl;
	}

	for (int i = 0; i < circles.size(); i++)
	{
		circle( output, circles[i].tam, circles[i].R, Scalar(0, 255, 0), 2);
		cout << circles[i].S << "," << circles[i].tam << endl;
	}

	imshow( "OUT", output );
}