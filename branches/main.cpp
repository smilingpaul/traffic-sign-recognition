#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include "SignTrafficDetect.h"
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\nonfree\features2d.hpp>

using namespace std;
using namespace cv;

/// Global variables


/** @function main */
int main( int argc, char** argv )
{
	while (1)
	{
		string srcPath;
		cout << "Source path: ";
		getline(cin, srcPath, '\n');

		string::iterator it = srcPath.begin();
		if (*it == '\"')
			srcPath.erase(srcPath.begin());
		it = srcPath.end() - 1;
		if (*it == '\"')
			srcPath.erase(srcPath.end() - 1);

		Mat src;
		/// Load an image
		src = imread( srcPath );

		CSignTrafficDetect Sangdetector(src);

		Sangdetector.detect();

		Sangdetector.drawOutput();


		//----------------------------------------------------------
		//Mat dst;

		Mat img_object(src, Sangdetector.triangles[0].bound);
		cvtColor(img_object, img_object, CV_BGR2GRAY);
		Mat img_scene = imread( "C:\\Users\\Tan Sang\\Desktop\\Images\\sample-004.jpg", CV_LOAD_IMAGE_GRAYSCALE );

		//-- Step 1: Detect the keypoints using SIFT Detector
		int minHessian = 500;

		SiftFeatureDetector detector( minHessian );

		std::vector<KeyPoint> keypoints_object, keypoints_scene;

		detector.detect( img_object, keypoints_object );
		detector.detect( img_scene, keypoints_scene );

		//-- Step 2: Calculate descriptors (feature vectors)
		SiftFeatureDetector extractor;

		Mat descriptors_object, descriptors_scene;

		extractor.compute( img_object, keypoints_object, descriptors_object );
		extractor.compute( img_scene, keypoints_scene, descriptors_scene );

		//-- Step 3: Matching descriptor vectors using FLANN matcher
		FlannBasedMatcher matcher;
		std::vector<DMatch> matches;
		matcher.match( descriptors_object, descriptors_scene, matches);

		double max_dist = 0; double min_dist = 100;

		//-- Quick calculation of max and min distances between keypoints
		for( int i = 0; i < descriptors_object.rows; i++ )
		{ 
			double dist = matches[i].distance;
			if( dist < min_dist ) min_dist = dist;
			if( dist > max_dist ) max_dist = dist;
		}

		printf("-- Max dist : %f \n", max_dist );
		printf("-- Min dist : %f \n", min_dist );

		//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
		std::vector< DMatch > good_matches;

		for(int i = 0; i < min(descriptors_scene.rows-1,(int) matches.size()); i++) //THIS LOOP IS SENSITIVE TO SEGFAULTS
		{
			if((matches[i].distance < 0.6*(matches[i].distance) ))
			{
				good_matches.push_back(matches[i]);
			}
		}

		Mat img_matches;
		drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
			good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
			vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

		//-- Show detected matches
		imshow( "Good Matches", img_matches );

		for( int i = 0; i < (int)good_matches.size(); i++ )
		{ printf( "-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d -- Distance: %.5f \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx, good_matches[i].distance ); }

		if (good_matches.size() >= 4)
		{
			//-- Localize the object
			std::vector<Point2f> obj;
			std::vector<Point2f> scene;

			for( int i = 0; i < good_matches.size(); i++ )
			{
				//-- Get the keypoints from the good matches
				obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
				scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
			}

			Mat H = findHomography( obj, scene, CV_RANSAC );

			//-- Get the corners from the image_1 ( the object to be "detected" )
			std::vector<Point2f> obj_corners(4);
			obj_corners[0] = cvPoint(0,0); 
			obj_corners[1] = cvPoint( img_object.cols, 0 );
			obj_corners[2] = cvPoint( img_object.cols, img_object.rows ); 
			obj_corners[3] = cvPoint( 0, img_object.rows );
			std::vector<Point2f> scene_corners(4);

			perspectiveTransform( obj_corners, scene_corners, H);

			//-- Draw lines between the corners (the mapped object in the scene - image_2 )
			line( src, scene_corners[0] + Point2f( img_object.cols, 0), scene_corners[1] + Point2f( img_object.cols, 0), Scalar(0, 0, 255), 4 );
			line( src, scene_corners[1] + Point2f( img_object.cols, 0), scene_corners[2] + Point2f( img_object.cols, 0), Scalar(0, 0, 255), 4 );
			line( src, scene_corners[2] + Point2f( img_object.cols, 0), scene_corners[3] + Point2f( img_object.cols, 0), Scalar(0, 0, 255), 4 );
			line( src, scene_corners[3] + Point2f( img_object.cols, 0), scene_corners[0] + Point2f( img_object.cols, 0), Scalar(0, 0, 255), 4 );
			

			//-- Show detected matches
			imshow( "Object detection", img_matches );
		}


		cvWaitKey();
		destroyAllWindows();
	}
	return 0;
}