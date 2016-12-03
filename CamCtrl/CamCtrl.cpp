// CamCtrl.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CamCtrl.h"
#include "ImageFeature.h"

int main(int argc, char** argv)
{
	CamCtrl camctrl;
	CommandLineParser parser(argc, argv, "{help h||}{@input|E:\\Cloud\\Research\\Vision\\CamCtrl\\CamCtrl\\gzt01.jpg|}");
	parser.about("Application name v1.1");
	if (parser.has("help"))
	{
		help();
		return 0;
	}

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

	help();

	const string winName = "image";
	namedWindow(winName, WINDOW_AUTOSIZE);
	camctrl.setImageAndWinName(image, winName);
	camctrl.showImage();
	int camctrlflag;
	camctrlflag = camctrl.ctrlCamera();

	if (camctrlflag == SAVEIMAGE)
	{
		ImageFeature Template;
		Template.initialize(camctrl.WinImage);
		string TwinName = "template";
		Template.showImage(TwinName);
		waitKey(0);
	}
		
		
}




