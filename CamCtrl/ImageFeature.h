#pragma once
#include "CamCtrl.h"
#include "vector"
#include "algorithm"


//该类用于存储相应轮廓的点序列、面积、中心等
const int msfsp = 20;		//作为meanshift filter的像素位置差值
const int msfsr = 10;		//作为Meanshift filter的像素大小差值
const float thratio = 1;	//图像平均阈值比例系数，thration*thmean作为阈值提取边界

enum ImageState
{
	TEMPLATE = 0,
	TARGET = 1
};

bool compareArea(vector<Point> _first, vector<Point> _second)
{
	float area1, area2;
	area1 = contourArea(_first, false);
	area2 = contourArea(_second, false);
	if (area1 > area2)
		return true;
	else return false;
}

class ImageFeature
{
public:
	ImageFeature() {}
	int initialize(const CamCtrl& _template);			//对模板图像的特征初始化
	int initialize(const CamCtrl& _target, const ImageFeature& _template);
	int getContour();									//提取工件轮廓（阈值法）
	void getFetchPoint();								//提取模板图像fetchpoint
	void getFetchAngle();								//提取模板图像fetch相对椭圆坐标系的angle及绝对angle
	void getFetchPoint(const ImageFeature& _template);		//根据模板图像提取目标图像fetchpoint
	void getFetchAngle(const ImageFeature& _template);		//根据模板图像提取目标图像fetch相对椭圆坐标系的angle及绝对angle
	void showImage(const string& _winName) const;			//显示并存储特征图像
	void clear();


private:
	bool isInitialized = false;	
	float FetchAngle;			//抓取用的机械臂相对绝对坐标系的角度
	Mat SrcImage;				//加载进的原图像
	Point FetchPoint;			//抓取点
	Point ComPoint;				//抓取点在绝对坐标系中相对椭圆中心的位置
	float ComAngle;				//抓取用的机械臂相对椭圆坐标轴的角度
	vector<Point> Contour;		//存储图像中工件的外形轮廓
	RotatedRect Box;			//图像工件轮廓的最小包围椭圆的外切矩形
	
};

void ImageFeature::clear()
{
	SrcImage.release();
	FetchPoint = Point(0, 0);
	ComPoint = Point(0, 0);
	FetchAngle = 0;
	ComAngle = 0;
	Contour.clear();
	Box.size = Size(0, 0);
	Box.center = Point(0, 0);
}

int ImageFeature::initialize(const CamCtrl& _template)			
{
	SrcImage = _template.WinImage.clone();
	FetchAngle = _template.FetchAngle;
	if (getContour() == 0)
		return 0;		//初始化失败
	else
	{
		getFetchPoint();
		getFetchAngle();
		isInitialized = true;
		return 1;
	}
}
int ImageFeature::getContour()
{
	if (!SrcImage.data)
		return 0;
	Mat Dst;
	Mat Dst_th;		//用于存储单通道图像
	pyrMeanShiftFiltering(SrcImage, Dst, msfsp, msfsr);		//进行meanshift滤波

	cvtColor(Dst, Dst_th, CV_BGR2GRAY);
	float th;
	th = thratio*mean(Dst_th).val[0];						//根据平均灰度值乘自定义比例，获得阈值提取分解点
	threshold(Dst_th, Dst_th, th, 255, CV_THRESH_BINARY);

	vector<vector<Point>> vecContour;
	vector<Vec4i> Hierachy;
	findContours(Dst_th, vecContour, Hierachy,
		CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	if (vecContour.size() <= 0)
	{
		cout << "failed to search the workpiece" << endl;
		return 0;
	}
	sort(vecContour.begin(), vecContour.end(), compareArea);
	Contour = *(vecContour.begin() + 1);		//选择其中面积第二大的轮廓作为工件轮廓
	Box = fitEllipse(Contour);
	return 1;
}

void ImageFeature::getFetchPoint()
{
	if (!SrcImage.data)
		return;
	FetchPoint.x = SrcImage.cols / 2;
	FetchPoint.y = SrcImage.rows / 2;
	ComPoint.x = FetchPoint.x - Box.center.x;
	ComPoint.y = FetchPoint.x - Box.center.y;
}

void ImageFeature::getFetchAngle()
{
	if (!SrcImage.data)
		return;
	ComAngle = FetchAngle - Box.angle/180*PI;
}

int ImageFeature::initialize(const CamCtrl& _target, const ImageFeature& _template)
{
	SrcImage = _target.WinImage.clone();
	FetchAngle = _target.FetchAngle;
	getContour();
	if (contourArea(Contour, false) < 0.7*contourArea(_template.Contour, false)
		|| contourArea(Contour, false) >1.3*contourArea(_template.Contour, false))		//利用面积大小对轮廓是否为工件轮廓进行判断
		return 0;
	else
	{
		getFetchPoint(_template);
		getFetchAngle(_template);
		isInitialized = true;
		return 1;
	}
	
}

void ImageFeature::getFetchPoint(const ImageFeature& _template)
{
	//由2到1的变换矩阵：
	//12R = [cos(theta) -sin(theta)]
	//	  [sin(theta) cos(theta)]
	//由1到2的变换矩阵是上述矩阵的逆
	Point templatepos_t, targetpos_t;							//分别表示template与target相对于Box.center在椭圆坐标系中的位置
	float templateAngle = _template.Box.angle / PI * 180;		//椭圆坐标系相对绝对坐标转动的角度
	templatepos_t.x = cos(templateAngle)*_template.ComPoint.x + sin(templateAngle)*_template.ComPoint.y;
	templatepos_t.y = -sin(templateAngle)*_template.ComPoint.x + cos(templateAngle)*_template.ComPoint.y;
	float rate_x, rate_y;										//模板、目标轮廓的缩放比例, dst/src
	rate_x = this->Box.size.width / _template.Box.size.width;
	rate_y = this->Box.size.height / _template.Box.size.height;
	targetpos_t.x = templatepos_t.x*rate_x;						//对目标轮廓在椭圆坐标系中相对尺寸膨胀率
	targetpos_t.y = templatepos_t.y*rate_y;
	float targetAngle = this->Box.angle / PI * 180;
	this->ComPoint.x = cos(targetAngle)*targetpos_t.x - sin(targetAngle)*targetpos_t.y;
	this->ComPoint.y = sin(targetAngle)*targetpos_t.x + cos(targetAngle)*targetpos_t.y;
	this->FetchPoint.x = this->ComPoint.x + this->Box.center.x;
	this->FetchPoint.y = this->ComPoint.y + this->Box.center.y;
}

void ImageFeature::getFetchAngle(const ImageFeature& _template)
{
	this->ComAngle = _template.ComAngle;
	this->FetchAngle = this->ComAngle + this->Box.angle / 180 * PI;
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

		
		circle(res, FetchPoint, 5, RED, -1);
		line(res, Point(FetchPoint.x + res.cols, FetchPoint.y - res.cols * tan(FetchAngle)),
			Point(FetchPoint.x - res.cols, FetchPoint.y + res.cols * tan(FetchAngle)), GREEN, 1);
		line(res, Point(FetchPoint.x + res.rows * tan(FetchAngle), FetchPoint.y + res.rows),
			Point(FetchPoint.x - res.rows * tan(FetchAngle), FetchPoint.y - res.rows), BLUE, 1);

		//namedWindow(_winName, WINDOW_NORMAL);
		//imshow(_winName, res);
		string jpgname = _winName + ".jpg";
		imwrite(jpgname, res);
	}
	else
	{
		cout << "The imagefeature hasn't be initialized" << endl;
		return;
	}
}

