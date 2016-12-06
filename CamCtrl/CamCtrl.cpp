// CamCtrl.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CamCtrl.h"
#include "ImageFeature.h"

const Point origin = Point(682, 651);

void autoScanning(CamCtrl _template);		//对12*9的隔震台螺纹孔进行扫描，传入的系数是模板图像的CamCtrl类
void manualScanning(CamCtrl _camctrl, const float _WinWidth, const float _WinHeight);		//进行人工遥控扫描，传入的系数是模板图像的CamCtrl类，并将被继续用于扫描其他工件

int main(int argc, char** argv)
{
	
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
	CamCtrl camctrl;				//模板图像的CamCtrl类
	const string winName = "image";
	namedWindow(winName, WINDOW_AUTOSIZE);
	camctrl.setImageAndWinName(image, winName);
	camctrl.showImage();
	//使用ctrlCamera函数获取模板图像
	int camctrlflag;
	camctrlflag = camctrl.ctrlCamera();
	if (camctrlflag == SAVE_TEMPLATE_IMAGE)
		autoScanning(camctrl);
	if (camctrlflag == SAVE_TARGET_IMAGE)
		manualScanning(camctrl, camctrl.WinWidth, camctrl.WinHeight);
}

void autoScanning(CamCtrl _template)
{
	ImageFeature Template;
	Template.initialize(_template);
	string TwinName = "template";
	Template.showImage(TwinName);

	ImageFeature Target;
	TwinName = "target";
	string temp;
	for (int i = 0; i < 12; i++)		//行
	{
		for (int j = 0; j < 9; j++)		//列
		{
			_template.moveWin(origin + i*Point(0, 330) + j*Point(330, 0));
			Target.clear();
			if (Target.initialize(_template, Template) == 0)
				continue;
			else
			{
				if (i < 10)
					temp = TwinName + (char)('1' + i) + (char)('1' + j);
				else
					temp = TwinName + (char)('A' + i - 10) + (char)('1' + j);
				Target.showImage(temp);
			}
		}
	}
	cout << "Auto scanning finished." << endl;
	waitKey(0);
}

void manualScanning(CamCtrl _camctrl, const float _WinWidth, const float _WinHeight)
{
	cout << "\tBegin the manual Scanning\n" << endl;
	ImageFeature Template;
	Template.initialize(_camctrl);
	string TwinName = "template";
	Template.showImage(TwinName);

	ImageFeature Target;
	TwinName = "target";
	int camctrlflag2 = SAVE_TEMPLATE_IMAGE;
	int num = 0;			//匹配次数计数器
	char strnum[25];
	string temp;
	while (camctrlflag2 != EXIT)
	{
		camctrlflag2 = _camctrl.ctrlCamera();
		if (camctrlflag2 == SAVE_TEMPLATE_IMAGE)
		{
			float tempWinWidth = _camctrl.WinWidth;
			float tempWinHeight = _camctrl.WinHeight;
			_camctrl.WinWidth = _WinWidth;		//将camctrl的观测框大小进行调整，以匹配
			_camctrl.WinHeight = _WinHeight;
			_camctrl.refreshWin();				//将camctrl.winimage进行更新
			Target.clear();
			_itoa(num, strnum, 10);				//进行文件名编辑
			temp = TwinName + strnum;
			if (Target.initialize(_camctrl, Template) == 0)
				continue;
			else
			{
				_camctrl.WinWidth = tempWinWidth;
				_camctrl.WinHeight = tempWinHeight;
				Target.showImage(temp);
				num++;
				
			}
				
		}
	}

}