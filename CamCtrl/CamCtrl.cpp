#include "stdafx.h"
#include "CamCtrl.h"
#include "ImageRecognition.h"
Point CamCtrl::origin = Point(0, 0);

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
	WinWidth = WinWidthMax;			//观测窗口的宽
	WinHeight = WinHeightMax;		//观测窗口的高
	WinCenter = TopLeft;
	FetchAngle = 0;
	refreshWin();
}

void CamCtrl::setImageAndWinName(const Mat& _image, const string& _winName)
{
	CV_Assert(!_image.empty() || !_winName.empty());
	SrcImg = _image.clone();
	int top, bottom, left, right;
	top = (int)SrcImg.cols;
	bottom = (int)SrcImg.cols;
	left = (int)SrcImg.rows;
	right = (int)SrcImg.rows;

	copyMakeBorder(_image, SrcImg, top, bottom, left, right, BORDER_CONSTANT);

	TopLeft = Point(left, top);
	TopRight = TopLeft + Point(_image.cols, 0);
	BottomLeft = TopLeft + Point(0, _image.rows);
	BottomRight = TopLeft + Point(_image.cols, _image.rows);
	WinName = &_winName;
	origin = TopLeft;
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

		line(Roi, Point(center.x + Roi.cols / 2, center.y - Roi.cols / 2 * tan(FetchAngle)),
			Point(center.x - Roi.cols / 2, center.y + Roi.cols / 2 * tan(FetchAngle)), GREEN, 2);
		line(Roi, Point(center.x + Roi.rows / 2 * tan(FetchAngle), center.y + Roi.rows / 2),
			Point(center.x - Roi.rows / 2 * tan(FetchAngle), center.y - Roi.rows / 2), BLUE, 2);
		circle(Roi, center, 5, RED, -1);
		imshow(*WinName, Roi);
		//cout << WinCenter << endl;
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
	case MOVE_RIGHT:
		WinCenter.x += WinWidth*moveratio;
		if (WinCenter.x >= TopRight.x)
			WinCenter.x = TopRight.x;
		break;
	case MOVE_LEFT:
		WinCenter.x -= WinWidth*moveratio;
		if (WinCenter.x <= TopLeft.x)
			WinCenter.x = TopLeft.x;
		break;
	case MOVE_DOWN:
		WinCenter.y += WinHeight*moveratio;
		if (WinCenter.y >= BottomLeft.y)
			WinCenter.y = BottomLeft.y;
		break;
	case MOVE_UP:
		WinCenter.y -= WinHeight*moveratio;
		if (WinCenter.y <= TopLeft.y)
			WinCenter.y = TopLeft.y;
		break;
	case ZOOM_IN:
		WinWidth += WinWidth*moveratio;
		WinHeight += WinHeight*moveratio;
		if (WinWidth >= 2 * SrcImg.cols || WinHeight >= 2 * SrcImg.rows)
		{
			WinWidth = 2 * SrcImg.cols;
			WinHeight = 2 * SrcImg.rows;
		}
		break;
	case ZOOM_OUT:
		WinWidth -= WinWidth*moveratio;
		WinHeight -= WinHeight*moveratio;
		if (WinWidth <= WinWidthMin || WinHeight <= WinHeightMin)
		{
			WinWidth = (WinWidthMax <= WinHeightMax) ? WinWidthMin : WinWidthMax / WinHeightMax;
			WinHeight = (WinHeightMax >= WinWidthMax) ? WinHeightMin : WinHeightMax / WinWidthMax;
		}
		break;
	case ROTATE_CLOCKWISE:
		FetchAngle -= PI / 180;
		break;
	case ROTATE_ANTICLOCKWISE:
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
			flags = MOVE_RIGHT;
			break;
		case 'a':
			flags = MOVE_LEFT;
			break;
		case 'w':
			flags = MOVE_UP;
			break;
		case 's':
			flags = MOVE_DOWN;
			break;
		case 'i':
			flags = ZOOM_IN;
			break;
		case 'u':
			flags = ZOOM_OUT;
			break;
		case 'o':
			flags = ROTATE_ANTICLOCKWISE;
			break;
		case 'p':
			flags = ROTATE_CLOCKWISE;
			break;
		case 'j':
			return SAVE_AUTO_MODE;
		case 'k':
			return SAVE_MANUAL_MODE;
		case 'l':
			return SAVE_AUTO2_MODE;
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
