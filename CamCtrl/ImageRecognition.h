//#pragma once
#ifndef IMAGERECOGNITION_H_
#define IMAGERECOGNITION_H_

#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <algorithm>

using namespace std;
using namespace cv;

class ImageFeature;

const double PI = 3.141592654;
const Scalar RED = Scalar(0, 0, 255);
const Scalar PINK = Scalar(230, 130, 255);
const Scalar BLUE = Scalar(255, 0, 0);
const Scalar LIGHTBLUE = Scalar(255, 255, 160);
const Scalar GREEN = Scalar(0, 255, 0);
const Scalar BLACK = Scalar(0, 0, 0);


class ImageRecognition
{
public:

	static int getContour(vector<vector<Point>> &_vecContour, Mat const &_image);							//提取轮廓（HSV颜色辨别）
	static int getScanPoint(const Mat& _image, ImageFeature &_template, vector<Point> &_pointpos);			//根据模板图像，提取出_image函数中的符合要求的轮廓的位置
	static bool compareArea(vector<Point> _first, vector<Point> _second);
	static void help();
};

#endif // !IMAGERECOGNITION_H