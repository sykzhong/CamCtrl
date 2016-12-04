#pragma once
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include "math.h"

using namespace std;
using namespace cv;


const float PI = 3.141592654;
const Scalar RED = Scalar(0, 0, 255);
const Scalar PINK = Scalar(230, 130, 255);
const Scalar BLUE = Scalar(255, 0, 0);
const Scalar LIGHTBLUE = Scalar(255, 255, 160);
const Scalar GREEN = Scalar(0, 255, 0);
const Scalar BLACK = Scalar(0, 0, 0);

const float moveratio = 0.01;

const float WinWidthMax = 1000;		//�۲ⴰ�ڵ�����
const float WinHeightMax = 1000;		//�۲ⴰ�ڵ����ߣ�����ֱ��ȷ���˴��ڵĳ����
const float WinWidthMin = 100;
const float WinHeightMin = WinWidthMin*WinHeightMax / WinWidthMax;

static void help()
{
	cout << "\nThis program aims to control the move of the camera, \n"
		"and then transform the target image to other program.\n"
		"Call:\n"
		"./CamCtrl <image_name>\n"
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
		"\tJ - Save image\n"
		"\tEsc - Exit"
		"\n"<< endl;
}

enum CamCtrlFlags
{
	MOVERIGHT = 0,
	MOVELEFT = 1,
	MOVEDOWN = 2,
	MOVEUP = 3,
	ZOOMIN = 4,						//�Ŵ���Ұ
	ZOOMOUT = 5,					//��С��Ұ
	ROTATECLOCKWISE = 6,			//˳ʱ����ת
	ROTATEANTICLOCKWISE = 7,		//��ʱ����ת
	EXIT = 8,
	SAVEIMAGE = 9
};



class CamCtrl
{
public:
	void setImageAndWinName(const Mat& _image, const string& _winName);
	void showImage() const;
	void reset();
	void moveWin(int flags);
	void moveWin(Point _pos);
	int ctrlCamera();
	Mat WinImage;				//��������ͼƬ
	float FetchAngle = 0;		//ץȡ�Ƕȣ���ʼ��x��ƽ�У����ȱ�ʾ��ȡֵ��Χ��-pi/2~pi/2֮��
	Point WinCenter;			//�۲�����

private:
	Mat SrcImg;					//���ڷ����ȫ��ͼ��
	const string *WinName;		//������ʾ�Ĵ�������
	bool isInitialized;			//�Ƿ��ʼ���ı�ʶ��һ�㴦�ڷǳ�ʼ��״̬��
	Rect ObserveWin;			//���ڹ۲�Ĵ���
	float WinWidth = 1000;		//�۲ⴰ�ڵĿ�
	float WinHeight = 1000;		//�۲ⴰ�ڵĸ�
	Point TopLeft, TopRight, BottomLeft, BottomRight;		//�ɹ۲������ĸ��ǵ�
	void refreshWin();			//����WinCenter����ObserveWin
};

void CamCtrl::refreshWin()
{
	ObserveWin.width = WinWidth;
	ObserveWin.height = WinHeight;
	ObserveWin.x = WinCenter.x - 0.5*WinWidth;
	ObserveWin.y = WinCenter.y - 0.5*WinHeight;
	WinImage = SrcImg(ObserveWin).clone();
}

void CamCtrl::reset()
{
	isInitialized = false;
	WinWidth = WinWidthMax;			//�۲ⴰ�ڵĿ�
	WinHeight = WinHeightMax;		//�۲ⴰ�ڵĸ�
	WinCenter = TopLeft;
	FetchAngle = 0;
	refreshWin();
}

void CamCtrl::setImageAndWinName(const Mat& _image, const string& _winName)
{
	CV_Assert(!_image.empty() || !_winName.empty());
	int top, bottom, left, right;
	top = (int)WinHeightMax / 2;
	bottom = (int)WinHeightMax / 2;
	left = (int)WinWidthMax / 2;
	right = (int)WinWidthMax / 2;
	SrcImg = _image.clone();
	copyMakeBorder(_image, SrcImg, top, bottom, left, right, BORDER_CONSTANT);

	TopLeft = Point(left, top);
	TopRight = TopLeft + Point(_image.cols, 0);
	BottomLeft = TopLeft + Point(0, _image.rows);
	BottomRight = TopLeft + Point(_image.cols, _image.rows);
	WinName = &_winName;
	reset();
}

void CamCtrl::showImage() const
{
	CV_Assert(!SrcImg.empty() || !WinName->empty());
	Mat res;
	if (!isInitialized)
	{
		Mat Roi = WinImage.clone();
				
		resize(Roi, Roi, Size(WinWidthMax, WinHeightMax));
		Point center = Point(Roi.cols / 2, Roi.rows / 2);
		circle(Roi, center, 5, RED, -1);
		line(Roi, Point(center.x + Roi.cols / 2, center.y - Roi.cols /2 *tan(FetchAngle)),
			Point(center.x - Roi.cols / 2, center.y + Roi.cols / 2 *tan(FetchAngle)), GREEN, 1);
		line(Roi, Point(center.x + Roi.rows / 2 * tan(FetchAngle), center.y + Roi.rows / 2),
			Point(center.x - Roi.rows / 2 * tan(FetchAngle), center.y - Roi.rows / 2), BLUE, 1);
		imshow(*WinName, Roi);
		cout << WinCenter << endl;
	}
	else
	{
		cout << "Initialized" << endl;
		return;
	}

}

void CamCtrl::moveWin(Point _pos)
{
	WinCenter = _pos;
	refreshWin();
}

void CamCtrl::moveWin(int flags)
{
	switch (flags)
	{
	case MOVERIGHT:
		WinCenter.x += WinWidth*moveratio;
		if (WinCenter.x >= TopRight.x)
			WinCenter.x = TopRight.x;
		break;
	case MOVELEFT:
		WinCenter.x -= WinWidth*moveratio;
		if (WinCenter.x <= TopLeft.x)
			WinCenter.x = TopLeft.x;
		break;
	case MOVEDOWN:
		WinCenter.y += WinHeight*moveratio;
		if (WinCenter.y >= BottomLeft.y)
			WinCenter.y = BottomLeft.y;
		break;
	case MOVEUP:
		WinCenter.y -= WinHeight*moveratio;
		if (WinCenter.y <= TopLeft.y)
			WinCenter.y = TopLeft.y;
		break;
	case ZOOMIN:			
		WinWidth += WinWidth*moveratio;
		WinHeight += WinHeight*moveratio;
		if (WinWidth >= WinWidthMax || WinHeight >= WinHeightMax)
		{
			WinWidth = WinWidthMax;
			WinHeight = WinHeightMax;
		}
		break;
	case ZOOMOUT:
		WinWidth -= WinWidth*moveratio;
		WinHeight -= WinHeight*moveratio;
		if (WinWidth <= WinWidthMin || WinHeight <= WinHeightMin)
		{
			WinWidth = (WinWidthMax <= WinHeightMax) ? WinWidthMin : WinWidthMax / WinHeightMax;
			WinHeight = (WinHeightMax >= WinWidthMax) ? WinHeightMin : WinHeightMax / WinWidthMax;
		}
		break;
	case ROTATECLOCKWISE:
		FetchAngle -= PI / 180;
		break;
	case ROTATEANTICLOCKWISE:
		FetchAngle += PI / 180;
		break;
	default:
		break;
	}
	refreshWin();
}

int CamCtrl::ctrlCamera()
{
	for (;;)
	{
		int c = waitKey(0);
		int flags = 100;
		switch ((char)c)
		{
		case 'd':
			flags = MOVERIGHT;
			break;
		case 'a':
			flags = MOVELEFT;
			break;
		case 'w':
			flags = MOVEUP;
			break;
		case 's':
			flags = MOVEDOWN;
			break;
		case 'i':
			flags = ZOOMIN;
			break;
		case 'u':
			flags = ZOOMOUT;
			break;
		case 'o':
			flags = ROTATEANTICLOCKWISE;
			break;
		case 'p':
			flags = ROTATECLOCKWISE;
			break;
		case 'j':
			return SAVEIMAGE;
		case 27:
			return EXIT;
		default:
			cout << "Wrong button ..." << endl;
			break;
		}
		moveWin(flags);
		showImage();
	}
}