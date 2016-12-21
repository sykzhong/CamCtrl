#include "stdafx.h"
#include "ImageFeature.h"
#include "CamCtrl.h"

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

int ImageFeature::initFeature(const CamCtrl& _template)
{
	SrcImage = _template.WinImage.clone();
	FetchAngle = _template.FetchAngle;
	vector<vector<Point>> vecContour;
	int flag;								//用于判断是否查找到轮廓
	flag = ImageRecognition::getContour(vecContour, this->SrcImage);			//查找获得轮廓组
	if (flag == 0)
	{
		isInitialized = false;				//无轮廓，初始化失败
		return 0;
	}
	else
	{
		sort(vecContour.begin(), vecContour.end(), ImageRecognition::compareArea);
		Contour = *(vecContour.begin() + 1);		//选择其中面积第二大的轮廓作为工件轮廓
		Box = fitEllipse(Contour);
		getFetchPoint();
		getFetchAngle();
		isInitialized = true;
		return 1;
	}
}

int ImageFeature::initFeature(const CamCtrl& _target, const ImageFeature& _template)
{
	SrcImage = _target.WinImage.clone();
	FetchAngle = _target.FetchAngle;
	vector<vector<Point>> vecContour;
	int flag;								//用于判断是否查找到轮廓
	flag = ImageRecognition::getContour(vecContour, SrcImage);			//查找获得轮廓组
	if (flag == 0)
	{
		isInitialized = false;				//无轮廓，初始化失败
		return 0;
	}

	sort(vecContour.begin(), vecContour.end(), ImageRecognition::compareArea);
	Contour = *(vecContour.begin() + 1);		//选择其中面积第二大的轮廓作为工件轮廓
	Box = fitEllipse(Contour);

	if (contourArea(Contour, false) < cmin*contourArea(_template.Contour, false)
		|| contourArea(Contour, false) > cmax*contourArea(_template.Contour, false))		//利用面积大小对轮廓是否为工件轮廓进行判断
		return 0;

	else
	{
		getFetchPoint(_template);
		getFetchAngle(_template);
		isInitialized = true;
		return 1;
	}
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
	ComAngle = FetchAngle - Box.angle / 180 * PI;
}

void ImageFeature::getFetchPoint(const ImageFeature& _template)
{
	//由2到1的变换矩阵：
	//12R = [cos(theta) -sin(theta)]
	//	  [sin(theta) cos(theta)]
	//由1到2的变换矩阵是上述矩阵的逆
	Point2f templatepos_t, targetpos_t;							//分别表示template与target相对于Box.center在椭圆坐标系中的位置
	double templateAngle = _template.Box.angle / 180 * PI;		//椭圆坐标系相对绝对坐标转动的角度
	templatepos_t.x = cos(templateAngle)*_template.ComPoint.x + sin(templateAngle)*_template.ComPoint.y;
	templatepos_t.y = -sin(templateAngle)*_template.ComPoint.x + cos(templateAngle)*_template.ComPoint.y;
	double rate_x, rate_y;										//模板、目标轮廓的缩放比例, dst/src
	rate_x = this->Box.size.width / _template.Box.size.width;
	rate_y = this->Box.size.height / _template.Box.size.height;
	targetpos_t.x = templatepos_t.x*rate_x;						//对目标轮廓在椭圆坐标系中相对尺寸膨胀率
	targetpos_t.y = templatepos_t.y*rate_y;
	double targetAngle = this->Box.angle / 180 * PI;
	this->ComPoint.x = cos(targetAngle)*targetpos_t.x - sin(targetAngle)*targetpos_t.y;
	this->ComPoint.y = sin(targetAngle)*targetpos_t.x + cos(targetAngle)*targetpos_t.y;
	this->FetchPoint.x = this->ComPoint.x + this->Box.center.x;
	this->FetchPoint.y = this->ComPoint.y + this->Box.center.y;
}

void ImageFeature::getFetchAngle(const ImageFeature& _template)
{
	this->ComAngle = _template.ComAngle;
	this->FetchAngle = this->ComAngle + this->Box.angle/ 180 * PI;
}

void ImageFeature::writeImage(const string& _winName) const
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
		line(res, Point(FetchPoint.x + res.cols, FetchPoint.y + res.cols * tan(FetchAngle)),
			Point(FetchPoint.x - res.cols, FetchPoint.y - res.cols * tan(FetchAngle)), GREEN, 1);
		line(res, Point(FetchPoint.x - res.rows * tan(FetchAngle), FetchPoint.y + res.rows),
			Point(FetchPoint.x + res.rows * tan(FetchAngle), FetchPoint.y - res.rows), BLUE, 1);

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

int ImageFeature::getContourArea()
{
	return contourArea(this->Contour, false);
}


