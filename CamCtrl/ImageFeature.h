#pragma once
#include "ImageRecognition.h"

class CamCtrl;
//该类用于存储相应轮廓的点序列、面积、中心等
const int msfsp = 20;		//作为meanshift filter的像素位置差值
const int msfsr = 10;		//作为Meanshift filter的像素大小差值
const double thratio = 0.5;	//图像平均阈值比例系数，thration*thmean作为阈值提取边界
const double cmin = 0.7;		//轮廓提取判断的面积最小比例
const double cmax = 1.3;		//轮廓提取判断的面积最大比例

//背景HSV取值区间，这里背景设为绿色
const int iLowH = 34;
const int iHighH = 130;

const int iLowS = 90;
const int iHighS = 255;

const int iLowV = 0;
const int iHighV = 255;

enum ImageState
{
	TEMPLATE = 0,
	TARGET = 1
};




class ImageFeature
{
public:
	int initFeature(const CamCtrl& _template);								//对模板图像的特征初始化
	int initFeature(const CamCtrl& _target, const ImageFeature& _template);	//根据模板图像，对camtrl获得的target进行初始化与判断
	
	int getContourArea();
	void getFetchPoint();								//提取模板图像fetchpoint
	void getFetchAngle();								//提取模板图像fetch相对椭圆坐标系的angle及绝对angle
	void getFetchPoint(const ImageFeature& _template);		//根据模板图像提取目标图像fetchpoint
	void getFetchAngle(const ImageFeature& _template);		//根据模板图像提取目标图像fetch相对椭圆坐标系的angle及绝对angle
	void writeImage(const string& _winName) const;			//显示并存储特征图像
	void clear();
private:
	double FetchAngle;			//抓取用的机械臂相对绝对坐标系的角度
	Mat SrcImage;				//加载进的原图像

	bool isInitialized = false;	
	Point FetchPoint;			//抓取点
	Point2f ComPoint;				//抓取点在绝对坐标系中相对椭圆中心的位置
	double ComAngle;				//抓取用的机械臂相对椭圆坐标轴的角度
	vector<Point> Contour;		//存储图像中工件的外形轮廓
	RotatedRect Box;			//图像工件轮廓的最小包围椭圆的外切矩形
};
