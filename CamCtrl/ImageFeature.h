#pragma once
#pragma once
#include "CamCtrl.h"
#include "vector"
#include "algorithm"
#define PI 3.141592654;

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
	int initialize(const Mat& _image);
	void getContour();
	void getFetchPoint();				//��ȡģ��ͼ��fetchpoint
	void getFetchPoint(ImageFeature& _feature);		//����ģ��ͼ����ȡĿ��ͼ��fetchpoint
	void showImage(const string& _winName) const;

private:
	bool isInitialized = false;
	Mat SrcImage;				//���ؽ���ԭͼ��
	Mat FeatureImage;
	vector<Point> Contour;		//�洢ͼ���й�������������
	RotatedRect Box;			//ͼ�񹤼���������С��Χ��Բ�����о���
	Point FetchPoint;			//ץȡ��
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

int ImageFeature::initialize(const Mat& _image)			//�Դ˺����������أ�����ɲ�ͬ����¶�fetchpoint������
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