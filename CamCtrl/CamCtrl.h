#pragma once
using namespace std;
using namespace cv;

const Scalar RED = Scalar(0, 0, 255);
const Scalar PINK = Scalar(230, 130, 255);
const Scalar BLUE = Scalar(255, 0, 0);
const Scalar LIGHTBLUE = Scalar(255, 255, 160);
const Scalar GREEN = Scalar(0, 255, 0);
const Scalar BLACK = Scalar(0, 0, 0);

const float moveratio = 0.1;

const float WinWidthMax = 700;
const float WinHeightMax = 700;

enum WindowMoveFlags
{
	MOVERIGHT = 0,
	MOVELEFT = 1,
	MOVEDOWN = 2,
	MOVEUP = 3,
	ZOOMIN = 4,			//放大
	ZOOMOUT = 5			//缩小
};

class CamCtrl
{
public:
	void setImageAndWinName(const Mat& _image, const string& _winName);
	void showImage() const;
	void reset();
	void moveWin(int flags);

private:
	Mat SrcImg;					//用于仿真的全景图像
	const string *WinName;		//用于显示的窗口名称
	Mat WinImage;				//窗口所含图片
	bool isInitialized;

	Rect ObserveWin;			//用于观测的窗口
	Point WinCenter;			//观测中心
	float WinWidth = 1000;		//观测窗口的宽
	float WinHeight = 1000;		//观测窗口的高
	Point TopLeft, TopRight, BottomLeft, BottomRight;		//可观测区的四个角点
	
	void refreshWin();			//根据WinCenter更新ObserveWin
};

void CamCtrl::refreshWin()
{
	ObserveWin.width = WinWidth;
	ObserveWin.height = WinHeight;
	ObserveWin.x = WinCenter.x - 0.5*WinWidth;
	ObserveWin.y = WinCenter.y - 0.5*WinHeight;
}

void CamCtrl::reset()
{
	isInitialized = false;
	WinWidth = WinWidthMax;		//观测窗口的宽
	WinHeight = WinHeightMax;		//观测窗口的高
	WinCenter = TopLeft;
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
		SrcImg.copyTo(res);
	}
	else
	{
		cout << "Initialized" << endl;
		return;
	}
	rectangle(res, Point(ObserveWin.x, ObserveWin.y), Point(ObserveWin.x + WinWidth, ObserveWin.y + WinHeight), RED, 2);
	Mat Roi = res(ObserveWin);
	Mat Roicopy = Roi.clone();
	//res(ObserveWin).convertTo(Roi, Roi.type(), 1, 0);
	//pyrUp(Roi, Roicopy, Size(WinWidthMax*2, WinHeightMax*2));
	resize(Roi, Roicopy, Size(WinWidthMax, WinHeightMax));
	imshow(*WinName, Roicopy);
	//imshow(*WinName, res);
	
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
	case ZOOMIN:			//关于放大缩小的条件约束还需后续加进来，或者在roi的显示中下功夫
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
		if (WinWidth <= 1 || WinHeight <= 1)
		{
			WinWidth = (WinWidthMax <= WinHeightMax) ? 1 : WinWidthMax / WinHeightMax;
			WinHeight = (WinHeightMax >= WinWidthMax) ? 1 : WinHeightMax / WinWidthMax;
		}
	default:
		break;
	}
	refreshWin();
}