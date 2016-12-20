#pragma once
#include "ImageRecognition.h"

using namespace std;
using namespace cv;

namespace img{

}


enum CamCtrlFlags
{
	MOVE_RIGHT = 0,
	MOVE_LEFT = 1,
	MOVE_DOWN = 2,
	MOVE_UP = 3,
	ZOOM_IN = 4,						//�Ŵ���Ұ
	ZOOM_OUT = 5,					//��С��Ұ
	ROTATE_CLOCKWISE = 6,			//˳ʱ����ת
	ROTATE_ANTICLOCKWISE = 7,		//��ʱ����ת
	EXIT = 8,
	SAVE_AUTO_MODE = 9,				//��˳��ɨ��
	SAVE_MANUAL_MODE = 10,			//�˹�����ɨ��
	SAVE_AUTO2_MODE = 11			//��������µ�ɨ��
};



class CamCtrl
{
public:
	Mat WinImage;						//��������ͼƬ
	double FetchAngle = 0;				//ץȡ�Ƕȣ���ʼ��x��ƽ�У����ȱ�ʾ��ȡֵ��Χ��-pi/2~pi/2֮��
	Point WinCenter;					//�۲�����
	double WinWidth = WinWidthMax;		//�۲ⴰ�ڵĿ�
	double WinHeight = WinHeightMax;		//�۲ⴰ�ڵĸ�
	static Point origin;

	void setImageAndWinName(const Mat& _image, const string& _winName);
	void showImage() const;								//��ʾͼƬ
	void reset();										//��ʼ������	
	void moveWin(int flags);							//���ü��̽��д����ƶ�
	void moveWin(Point _pos);							//������ֱ���ƶ�����Ӧλ��
	int ctrlCamera();									//ѭ�������̰��������������ݸ�moveWin(int flags)
	void refreshWin();									//����WinCenter����ObserveWin
	

private:
	Mat SrcImg;					//���ڷ����ȫ��ͼ��
	const string *WinName;		//������ʾ�Ĵ�������
	bool isInitialized;			//�Ƿ��ʼ���ı�ʶ��һ�㴦�ڷǳ�ʼ��״̬��
	Rect ObserveWin;			//���ڹ۲�Ĵ���
	
	const double moveratio = 0.01;

	const double WinWidthMax = 600;		//�۲ⴰ�ڵ�����
	const double WinHeightMax = 600;		//�۲ⴰ�ڵ����ߣ�����ֱ��ȷ���˴��ڵĳ����
	const double WinWidthMin = 100;
	const double WinHeightMin = WinWidthMin*WinHeightMax / WinWidthMax;

	Point TopLeft, TopRight, BottomLeft, BottomRight;		//�ɹ۲������ĸ��ǵ�

};

