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
	ZOOM_IN = 4,						//放大视野
	ZOOM_OUT = 5,					//缩小视野
	ROTATE_CLOCKWISE = 6,			//顺时针旋转
	ROTATE_ANTICLOCKWISE = 7,		//逆时针旋转
	EXIT = 8,
	SAVE_AUTO_MODE = 9,				//按顺序扫描
	SAVE_MANUAL_MODE = 10,			//人工控制扫描
	SAVE_AUTO2_MODE = 11			//乱序情况下的扫描
};



class CamCtrl
{
public:
	Mat WinImage;						//窗口所含图片
	double FetchAngle = 0;				//抓取角度，初始与x轴平行，弧度表示，取值范围在-pi/2~pi/2之间
	Point WinCenter;					//观测中心
	double WinWidth = WinWidthMax;		//观测窗口的宽
	double WinHeight = WinHeightMax;		//观测窗口的高
	static Point origin;

	void setImageAndWinName(const Mat& _image, const string& _winName);
	void showImage() const;								//显示图片
	void reset();										//初始化设置	
	void moveWin(int flags);							//利用键盘进行窗口移动
	void moveWin(Point _pos);							//将窗口直接移动到相应位置
	int ctrlCamera();									//循环检测键盘按键，将参数传递给moveWin(int flags)
	void refreshWin();									//根据WinCenter更新ObserveWin
	

private:
	Mat SrcImg;					//用于仿真的全景图像
	const string *WinName;		//用于显示的窗口名称
	bool isInitialized;			//是否初始化的标识（一般处于非初始化状态）
	Rect ObserveWin;			//用于观测的窗口
	
	const double moveratio = 0.01;

	const double WinWidthMax = 600;		//观测窗口的最大宽
	const double WinHeightMax = 600;		//观测窗口的最大高，这里直接确定了窗口的长宽比
	const double WinWidthMin = 100;
	const double WinHeightMin = WinWidthMin*WinHeightMax / WinWidthMax;

	Point TopLeft, TopRight, BottomLeft, BottomRight;		//可观测区的四个角点

};

