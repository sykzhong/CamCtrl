#pragma once
#include "CamCtrl.h"
#include "vector"
#include "algorithm"


//�������ڴ洢��Ӧ�����ĵ����С���������ĵ�
const int msfsp = 20;		//��Ϊmeanshift filter������λ�ò�ֵ
const int msfsr = 10;		//��ΪMeanshift filter�����ش�С��ֵ
const int th = 70;			//��Ϊ��ֵ��ȡ�ı߽磨����ʹ�ú�������ȷ����

enum ImageState
{
	TEMPLATE = 0,
	TARGET = 1
};

class ImageFeature
{
public:
	ImageFeature() {}
	int initialize(const CamCtrl& _template);			//��ģ��ͼ���������ʼ��
	int initialize(const CamCtrl& _target, const ImageFeature& _template);
	void getContour();									//��ȡ������������ֵ����
	void getFetchPoint();								//��ȡģ��ͼ��fetchpoint
	void getFetchAngle();								//��ȡģ��ͼ��fetch�����Բ����ϵ��angle������angle
	void getFetchPoint(const ImageFeature& _template);		//����ģ��ͼ����ȡĿ��ͼ��fetchpoint
	void getFetchAngle(const ImageFeature& _template);		//����ģ��ͼ����ȡĿ��ͼ��fetch�����Բ����ϵ��angle������angle
	void showImage(const string& _winName) const;
	


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

bool compareArea(vector<Point> _first, vector<Point> _second)
{
	float area1, area2;
	area1 = contourArea(_first, false);
	area2 = contourArea(_second, false);
	if (area1 > area2)
		return true;
	else return false;
}

int ImageFeature::initialize(const CamCtrl& _template)			
{
	SrcImage = _template.WinImage.clone();
	FetchAngle = _template.FetchAngle;
	getContour();
	getFetchPoint();
	getFetchAngle();
	isInitialized = true;
	return 1;
}
void ImageFeature::getContour()
{
	if (!SrcImage.data)
		return;
	Mat Dst;
	Mat Dst_th;		//���ڴ洢��ͨ��ͼ��
	pyrMeanShiftFiltering(SrcImage, Dst, msfsp, msfsr);		//����meanshift�˲�

	cvtColor(Dst, Dst_th, CV_BGR2GRAY);
	threshold(Dst_th, Dst_th, th, 255, CV_THRESH_BINARY);

	vector<vector<Point>> vecContour;
	vector<Vec4i> Hierachy;
	findContours(Dst_th, vecContour, Hierachy,
		CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	sort(vecContour.begin(), vecContour.end(), compareArea);
	Contour = *(vecContour.begin() + 1);		//ѡ����������ڶ����������Ϊ��������
	Box = fitEllipse(Contour);
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
	getFetchPoint(_template);
	getFetchAngle(_template);
	isInitialized = true;
	return 1;
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
		namedWindow(_winName, WINDOW_NORMAL);
		imshow(_winName, res);
	}
	else
	{
		cout << "The imagefeature hasn't be initialized" << endl;
		return;
	}
}