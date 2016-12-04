#pragma once
#include "CamCtrl.h"
#include "vector"
#include "algorithm"


//�������ڴ洢��Ӧ�����ĵ����С���������ĵ�
const int msfsp = 20;		//��Ϊmeanshift filter������λ�ò�ֵ
const int msfsr = 10;		//��ΪMeanshift filter�����ش�С��ֵ
const float thratio = 1;	//ͼ��ƽ����ֵ����ϵ����thration*thmean��Ϊ��ֵ��ȡ�߽�

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
	int initialize(const CamCtrl& _template);			//��ģ��ͼ���������ʼ��
	int initialize(const CamCtrl& _target, const ImageFeature& _template);
	int getContour();									//��ȡ������������ֵ����
	void getFetchPoint();								//��ȡģ��ͼ��fetchpoint
	void getFetchAngle();								//��ȡģ��ͼ��fetch�����Բ����ϵ��angle������angle
	void getFetchPoint(const ImageFeature& _template);		//����ģ��ͼ����ȡĿ��ͼ��fetchpoint
	void getFetchAngle(const ImageFeature& _template);		//����ģ��ͼ����ȡĿ��ͼ��fetch�����Բ����ϵ��angle������angle
	void showImage(const string& _winName) const;			//��ʾ���洢����ͼ��
	void clear();


private:
	bool isInitialized = false;	
	float FetchAngle;			//ץȡ�õĻ�е����Ծ�������ϵ�ĽǶ�
	Mat SrcImage;				//���ؽ���ԭͼ��
	Point FetchPoint;			//ץȡ��
	Point ComPoint;				//ץȡ���ھ�������ϵ�������Բ���ĵ�λ��
	float ComAngle;				//ץȡ�õĻ�е�������Բ������ĽǶ�
	vector<Point> Contour;		//�洢ͼ���й�������������
	RotatedRect Box;			//ͼ�񹤼���������С��Χ��Բ�����о���
	
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
		return 0;		//��ʼ��ʧ��
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
	Mat Dst_th;		//���ڴ洢��ͨ��ͼ��
	pyrMeanShiftFiltering(SrcImage, Dst, msfsp, msfsr);		//����meanshift�˲�

	cvtColor(Dst, Dst_th, CV_BGR2GRAY);
	float th;
	th = thratio*mean(Dst_th).val[0];						//����ƽ���Ҷ�ֵ���Զ�������������ֵ��ȡ�ֽ��
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
	Contour = *(vecContour.begin() + 1);		//ѡ����������ڶ����������Ϊ��������
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
		|| contourArea(Contour, false) >1.3*contourArea(_template.Contour, false))		//���������С�������Ƿ�Ϊ�������������ж�
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
	//��2��1�ı任����
	//12R = [cos(theta) -sin(theta)]
	//	  [sin(theta) cos(theta)]
	//��1��2�ı任�����������������
	Point templatepos_t, targetpos_t;							//�ֱ��ʾtemplate��target�����Box.center����Բ����ϵ�е�λ��
	float templateAngle = _template.Box.angle / PI * 180;		//��Բ����ϵ��Ծ�������ת���ĽǶ�
	templatepos_t.x = cos(templateAngle)*_template.ComPoint.x + sin(templateAngle)*_template.ComPoint.y;
	templatepos_t.y = -sin(templateAngle)*_template.ComPoint.x + cos(templateAngle)*_template.ComPoint.y;
	float rate_x, rate_y;										//ģ�塢Ŀ�����������ű���, dst/src
	rate_x = this->Box.size.width / _template.Box.size.width;
	rate_y = this->Box.size.height / _template.Box.size.height;
	targetpos_t.x = templatepos_t.x*rate_x;						//��Ŀ����������Բ����ϵ����Գߴ�������
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

