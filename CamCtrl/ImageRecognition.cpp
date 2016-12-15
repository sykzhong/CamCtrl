#include "stdafx.h"
#include "ImageRecognition.h"
#include "ImageFeature.h"
#include "CamCtrl.h"

int ImageRecognition::getContour(vector<vector<Point>> &_vecContour, const Mat &_image)
{
	if (!_image.data)
		return 0;
	Mat Dst_HSV, Dst_th;		//用于存储HSV图像，单通道图像
	Mat res = _image.clone();
	pyrMeanShiftFiltering(res, res, msfsp, msfsr);		//进行meanshift滤波
	//Dst = _image.clone();

	cvtColor(res, Dst_HSV, COLOR_BGR2HSV);				//将图像由RGB转为HSV图像，以进行颜色识别
															//用于存储HSV图像各个通道的图像，用于进行直方图均衡化
	vector<Mat> hsvSplit;
	split(Dst_HSV, hsvSplit);
	equalizeHist(hsvSplit[2], hsvSplit[2]);
	merge(hsvSplit, Dst_HSV);
	//利用颜色梯度进行阈值提取，工件为黑，背景（被提取）为白
	inRange(Dst_HSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), Dst_th);

	_vecContour.clear();
	vector<Vec4i> Hierachy;
	findContours(Dst_th, _vecContour, Hierachy,
		CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	if (_vecContour.size() <= 0)
	{
		cout << "failed to search the workpiece" << endl;
		return 0;
	}
	return 1;
}

int ImageRecognition::getScanPoint(const Mat& _image, ImageFeature &_template, vector<Point> _pointpos)
{
	vector<vector<Point>> vecContour;
	getContour(vecContour, _image);
	for (vector<vector<Point>>::iterator iter = vecContour.begin(); iter != vecContour.end(); iter++)
	{
		if (contourArea(*iter, false) < cmin*_template.getContourArea()
			|| contourArea(*(iter), false) > cmax*_template.getContourArea())		//利用面积大小对轮廓是否为工件轮廓进行判断
			continue;
		else
		{
			RotatedRect tempBox;
			tempBox = fitEllipse(*iter);
			_pointpos.push_back(origin + (Point)tempBox.center);
		}
	}
	return 1;
}

bool ImageRecognition::compareArea(vector<Point> _first, vector<Point> _second)
{
	double area1, area2;
	area1 = contourArea(_first, false);
	area2 = contourArea(_second, false);
	if (area1 > area2)
		return true;
	else return false;
}

void ImageRecognition::help()
{
	cout << "\nThis program aims to control the move of the camera, \n"
		"and then transform the target image to other program.\n"
		"Call:\n"
		"vibration isolated table photo\n"
		"\Move the rectangular observing area around the object you want to select as the template\n" <<
		"\nHot keys: \n"
		"\tW - Move up\n"
		"\tS - Move down\n"
		"\tD - Move right\n"
		"\tA - Move left\n"
		"\n"
		"\tU - Zoom out\n"
		"\tI - Zoom in\n"
		"\n"
		"\tO - Rotate clockwise\n"
		"\tP - Rotate anticlockwise\n"
		"\tJ - Save template image\n"
		"\tK - Save target image\n"
		"\tEsc - Exit"
		"\n" << endl;
}