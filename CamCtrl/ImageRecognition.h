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

class ImageRecognition
{
public:
	static int getContour(vector<vector<Point>> &_vecContour, Mat const &_image);							//��ȡ������HSV��ɫ���
	static int getScanPoint(const Mat& _image, ImageFeature &_template, vector<Point> _pointpos);			//����ģ��ͼ����ȡ��_image�����еķ���Ҫ���������λ��
	static bool compareArea(vector<Point> _first, vector<Point> _second);
	static void help();
};

#endif // !IMAGERECOGNITION_H