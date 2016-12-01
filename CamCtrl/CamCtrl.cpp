// CamCtrl.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include "CamCtrl.h"

int main(int argc, char** argv)
{
	CamCtrl camctrl;
	CommandLineParser parser(argc, argv, "{@input|E:\\Cloud\\Research\\Vision\\CamCtrl\\CamCtrl\\gzt01.jpg|}");
	string filename = parser.get<string>("@input");
	if (filename.empty())
	{
		cout << "\nEmpty filename" << endl;
		return 1;
	}
	Mat image = imread(filename, 1);
	if (image.empty())
	{
		cout << "Couldn't read image filename" << filename << endl;
		return 1;
	}
	const string winName = "image";
	namedWindow(winName, WINDOW_NORMAL);

	camctrl.setImageAndWinName(image, winName);
	camctrl.showImage();
	for (;;)
	{
		int c = waitKey(0);
		int flags;
		switch ((char)c)
		{
		case 'd':
			cout << "Move Right ..." << endl;
			flags = MOVERIGHT;
			break;
		case 'a':
			cout << "Move Left ..." << endl;
			flags = MOVELEFT;
			break;
		case 'w':
			cout << "Move Up ..." << endl;
			flags = MOVEUP;
			break;
		case 's':
			cout << "Move Down ..." << endl;
			flags = MOVEDOWN;
			break;
		case 'u':
			cout << "Zoom in ..." << endl;
			flags = ZOOMIN;
			break;
		case 'i':
			cout << "Zoom out ..." << endl;
			flags = ZOOMOUT;
			break;
		}
		camctrl.moveWin(flags);
		camctrl.showImage();
	}
}




