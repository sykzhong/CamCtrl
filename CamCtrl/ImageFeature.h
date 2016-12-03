#pragma once
#pragma once
#include "CamCtrl.h"
#include "vector"
#include "algorithm"
#define PI 3.141592654;

//该类用于存储相应轮廓的点序列、面积、中心等
const int msfsp = 20;		//作为meanshift filter的像素位置差值
const int msfsr = 10;		//作为Meanshift filter的像素大小差值
const int th = 70;			//作为阈值提取的边界（考虑使用函数进行确定）

enum ImageState
{
	TEMPLATE = 0,
	TARGET = 1
};

class ImageFeature
{
public:
	ImageFeature() {}
	int initialize(const Mat& _image);
	void getContour();
	void getFetchPoint();				//提取模板图像fetchpoint
	void getFetchPoint(ImageFeature& _feature);		//根据模板图像提取目标图像fetchpoint
	void showImage(const string& _winName) const;

private:
	bool isInitialized = false;
	Mat SrcImage;				//加载进的原图像
	Mat FeatureImage;
	vector<Point> Contour;		//存储图像中工件的外形轮廓
	RotatedRect Box;			//图像工件轮廓的最小包围椭圆的外切矩形
	Point FetchPoint;			//抓取点
};

bool compareArea(vector<Point> _first, vector<Point> _second)
{
	int area1, area2;
	area1 = contourArea(_first, false);
	area2 = contourArea(_second, false);
	if (area1 > area2)
		return true;
	else return false;
}

int ImageFeature::initialize(const Mat& _image)			//对此函数进行重载，已完成不同情况下对fetchpoint的重载
{
	SrcImage = _image.clone();
	getContour();
	getFetchPoint();
	isInitialized = true;
	return 1;
}
void ImageFeature::getContour()
{
	if (!SrcImage.data)
		return;
	Mat Dst;
	Mat Dst_th;		//用于存储单通道图像
	pyrMeanShiftFiltering(SrcImage, Dst, msfsp, msfsr);		//进行meanshift滤波

	cvtColor(Dst, Dst_th, CV_BGR2GRAY);
	threshold(Dst_th, Dst_th, th, 255, CV_THRESH_BINARY);

	vector<vector<Point>> vecContour;
	vector<Vec4i> Hierachy;
	findContours(Dst_th, vecContour, Hierachy,
		CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	sort(vecContour.begin(), vecContour.end(), compareArea);
	Contour = *(vecContour.begin() + 1);		//选择其中面积第二大的轮廓作为工件轮廓
	Box = fitEllipse(Contour);
}

void ImageFeature::getFetchPoint()
{
	if (!SrcImage.data)
		return;
	FetchPoint.x = SrcImage.cols / 2;
	FetchPoint.y = SrcImage.rows / 2;
}

void ImageFeature::getFetchPoint(ImageFeature& feature)
{

}

void ImageFeature::showImage(const string& _winName) const
{
	CV_Assert(!SrcImage.empty());
	Mat res;
	res = SrcImage.clone();
	if (isInitialized == true)
	{
		vector<vector<Point>> vectemp;
		vectemp.clear();
		vectemp.push_back(Contour);
		drawContours(res, vectemp, -1, RED);
		namedWindow(_winName, WINDOW_NORMAL);
		imshow(_winName, res);
	}
	else
	{
		cout << "The imagefeature hasn't be initialized" << endl;
		return;
	}
}