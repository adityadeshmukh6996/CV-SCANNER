#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <cstdint>
#include <iostream>

using namespace cv;
using namespace std;

///////////////  Project 2 - Document Scanner  //////////////////////

Mat imgOriginal, imgGray, imgCanny, imgBlur, imgDil, imgThre, imgWarp, finalimg, finalimg1;
vector<Point>initialPoints;
vector<Point>finalPoints;
float w = 420, h = 596;

Mat preProcessing(Mat img)
{
	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0);
	Canny(imgBlur, imgCanny, 25, 75);

	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(imgCanny, imgDil, kernel);
	return imgDil;

}

// The above function is to pre-process the image. Pre-processing of the image includes turning the image into Gray and 
// then making it Blur using Gaussian Blur.
// After turning it into Blur imgage, we found out the edges to the Blurred image using the Canny edge filter. 
// Then at last dilated the image that we get after finding out the edges. 



vector<Point> getCountours(Mat image)
{
	vector<vector<Point>>contours;
	vector<Vec4i>hierarchy;
	findContours(image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	vector<vector<Point>>conPoly(contours.size());
	vector<Point>biggest;
	int maxArea = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		cout << area << endl;
		string objectType;
		if (area > 1000)
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
			if (area > maxArea && conPoly[i].size() == 4)
			{
				drawContours(imgOriginal, conPoly, i, Scalar(255, 0, 255), 2);
				biggest = { conPoly[i][0],conPoly[i][1], conPoly[i][2], conPoly[i][3] };
				maxArea = area;
			}
			
			
		}
		//drawContours(imgOriginal, conPoly, i, Scalar(255, 0, 255), 2);
		//imshow("Image Contours", imgOriginal);
	}
	//drawContours(imgOriginal, conPoly, i, Scalar(255, 0, 255), 2);
	return biggest;
}

// After finding out the edges, the next task is to find the counters of the image. To find the countours we used the findcontours syntax with which
// it searches for all the contours. Then we have filtered the contours which are not necessary by defining the area (randomly) that is 1000 and then we extracted 
// max area out of it using the perimeter and approxPolyDP syntaxes. After finding out the contour with max perimeter then we drew the contour using 
// drawContours syntax



void drawPoints(vector<Point> points, Scalar color)
{
	for (int i = 0; i < points.size(); i++)
	{
		circle(imgOriginal, points[i], 5, (0,0,255), FILLED);
		putText(imgOriginal, to_string(i), points[i], FONT_HERSHEY_PLAIN, 1, color, 1);
	}
}

// After getting the countour with maximum perimeter, we marked the vertices of the countour using the above function using cirlce as the syntax. 
// We defined the vertices using putText syntax. 

vector<Point>reorder(vector<Point>points)
{
	vector<Point>newPoints;
	vector<int>sumPoints, subPoints;
	for (int i = 0; i < 4; i++)
	{
		sumPoints.push_back(points[i].x + points[i].y);
		subPoints.push_back(points[i].x - points[i].y);
	}
	newPoints.push_back(points[min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);
	newPoints.push_back(points[max_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]);
	newPoints.push_back(points[min_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]);
	newPoints.push_back(points[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);
	
	
	return newPoints;
}


// As the vertices keep changing with the new image, hence we have fixed the vertices using the above function by taking the maximum and minimum values 
// of the sum and difference of the vertices. The maximum value of the sum of the vertices is considered to be the third vertex, the mininmum value of
// the sum of the vertices is the first vertex which happens to be the origin and the maximum value of the difference of the vertices is the second vertex
// and the remaining vertices is the fourth one. After defining the vertices with the new indexes, these are pushed into the new Vector of newPoints.


Mat getWarp(Mat img, vector<Point>points, float w, float h)
{
	Point2f src[4] = { points[0],points[1], points[2], points[3] };
	Point2f dst[4] = { {0.0f, 0.0f},{w, 0.0f},{0.0f,h},{w,h} };
	Mat matrix = getPerspectiveTransform(src, dst);
	warpPerspective(img, imgWarp, matrix, Point(w, h));
	return imgWarp;
}

// After we get the indexes of the vertices we warp the image using the above function to display it in the top view.


void main()
{
	string path = "C:/Users/deshm/Downloads/PXL_20220426_144158885.jpg";
	Mat dst = Mat();
	
	
	imgOriginal = imread(path);
	resize(imgOriginal, imgOriginal, Size(), 0.2, 0.2);
	imgThre = preProcessing(imgOriginal);
	initialPoints = getCountours(imgThre);
	//drawPoints(initialPoints, Scalar(0, 0, 255));
	finalPoints = reorder(initialPoints);
	drawPoints(finalPoints, Scalar(255, 0, 0));
	imgWarp = getWarp(imgOriginal, finalPoints, w, h);
	//resize(imgOriginal, imgOriginal, Size(), 0.25, 0.25);
	//imgWarp.convertTo(imgWarp, CV_8UC1);
	cvtColor(imgWarp, finalimg, COLOR_BGR2GRAY);
	adaptiveThreshold(finalimg, finalimg1, 200, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 9, 9);
	imshow("Image", imgOriginal);
	imshow("Image Dilation", imgThre);
	imshow("Image Warp", imgWarp);
	imshow("Image threshold", finalimg1);
	waitKey(0);

}
// The above one is the main function where all the functions are defined. the syntax 'imread' reads the path of the image and the syntax 'imshow displays 
// the image specified in the path. 