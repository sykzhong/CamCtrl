#pragma once
#include "ImageRecognition.h"

class CamCtrl;
//�������ڴ洢��Ӧ�����ĵ����С���������ĵ�
const int msfsp = 20;		//��Ϊmeanshift filter������λ�ò�ֵ
const int msfsr = 10;		//��ΪMeanshift filter�����ش�С��ֵ
const double thratio = 0.5;	//ͼ��ƽ����ֵ����ϵ����thration*thmean��Ϊ��ֵ��ȡ�߽�
const double cmin = 0.7;		//������ȡ�жϵ������С����
const double cmax = 1.3;		//������ȡ�жϵ����������

//����HSVȡֵ���䣬���ﱳ����Ϊ��ɫ
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
	int initFeature(const CamCtrl& _template);								//��ģ��ͼ���������ʼ��
	int initFeature(const CamCtrl& _target, const ImageFeature& _template);	//����ģ��ͼ�񣬶�camtrl��õ�target���г�ʼ�����ж�
	
	int getContourArea();
	void getFetchPoint();								//��ȡģ��ͼ��fetchpoint
	void getFetchAngle();								//��ȡģ��ͼ��fetch�����Բ����ϵ��angle������angle
	void getFetchPoint(const ImageFeature& _template);		//����ģ��ͼ����ȡĿ��ͼ��fetchpoint
	void getFetchAngle(const ImageFeature& _template);		//����ģ��ͼ����ȡĿ��ͼ��fetch�����Բ����ϵ��angle������angle
	void writeImage(const string& _winName) const;			//��ʾ���洢����ͼ��
	void clear();
private:
	double FetchAngle;			//ץȡ�õĻ�е����Ծ�������ϵ�ĽǶ�
	Mat SrcImage;				//���ؽ���ԭͼ��

	bool isInitialized = false;	
	Point FetchPoint;			//ץȡ��
	Point2f ComPoint;				//ץȡ���ھ�������ϵ�������Բ���ĵ�λ��
	double ComAngle;				//ץȡ�õĻ�е�������Բ������ĽǶ�
	vector<Point> Contour;		//�洢ͼ���й�������������
	RotatedRect Box;			//ͼ�񹤼���������С��Χ��Բ�����о���
};
