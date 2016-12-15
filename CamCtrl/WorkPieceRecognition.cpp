#include "stdafx.h"
#include "CamCtrl.h"
#include "ImageFeature.h"
#include "ImageRecognition.h"

void autoScanning(CamCtrl _template);														//对12*9的隔震台螺纹孔进行扫描，传入的系数是模板图像的CamCtrl类
void manualScanning(CamCtrl _camctrl, const double _WinWidth, const double _WinHeight);		//进行人工遥控扫描，传入的系数是模板图像的CamCtrl类，并将被继续用于扫描其他工件
void autoSacnning2(CamCtrl _camctrl, Mat &_image);		//进行乱序扫描，传入的系数是模板图像的CamCtrl类，并将被继续用于扫描其他工件

int main(int argc, char** argv)
{

	CommandLineParser parser(argc, argv, "{help h||}{@input|E:\\Cloud\\Research\\Vision\\Picture\\workpiece_v6\\lm05.jpg|}");
	parser.about("Application name v1.1");
	if (parser.has("help"))
	{
		ImageRecognition::help();
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
	ImageRecognition::help();
	CamCtrl camctrl;				//模板图像的CamCtrl类
	const string winName = "image";
	namedWindow(winName, WINDOW_AUTOSIZE);
	camctrl.setImageAndWinName(image, winName);
	camctrl.showImage();
	//使用ctrlCamera函数获取模板图像
	int camctrlflag;
	camctrlflag = camctrl.ctrlCamera();
	switch (camctrlflag)
	{
	case SAVE_AUTO2_MODE:
		autoSacnning2(camctrl, image);
		break;
	case SAVE_AUTO_MODE:
		autoScanning(camctrl);
		break;
	case SAVE_MANUAL_MODE:
		manualScanning(camctrl, camctrl.WinWidth, camctrl.WinHeight);
		break;
	default:
		break;
	}
}

void autoScanning(CamCtrl _template)
{
	ImageFeature Template;
	Template.initFeature(_template);
	string TwinName = "template";
	Template.writeImage(TwinName);

	ImageFeature Target;
	TwinName = "target";
	string temp;
	for (int i = 0; i < 12; i++)		//行
	{
		for (int j = 0; j < 9; j++)		//列
		{
			_template.moveWin(origin + i*Point(0, 330) + j*Point(330, 0));
			Target.clear();
			if (Target.initFeature(_template, Template) == 0)
				continue;
			else
			{
				if (i < 10)
					temp = TwinName + (char)('1' + i) + (char)('1' + j);
				else
					temp = TwinName + (char)('A' + i - 10) + (char)('1' + j);
				Target.writeImage(temp);
			}
		}
	}
	cout << "Auto scanning finished." << endl;
	waitKey(0);
}

void manualScanning(CamCtrl _camctrl, const double _WinWidth, const double _WinHeight)
{
	cout << "\tBegin the manual Scanning\n" << endl;
	ImageFeature Template;
	Template.initFeature(_camctrl);
	string TwinName = "template";
	Template.writeImage(TwinName);

	ImageFeature Target;
	TwinName = "target";

	int camctrlflag2 = SAVE_MANUAL_MODE;
	int num = 0;			//匹配次数计数器
	char strnum[25];
	string temp;
	while (camctrlflag2 != EXIT)
	{
		camctrlflag2 = _camctrl.ctrlCamera();
		if (camctrlflag2 == SAVE_MANUAL_MODE)
		{
			double tempWinWidth = _camctrl.WinWidth;
			double tempWinHeight = _camctrl.WinHeight;
			_camctrl.WinWidth = _WinWidth;		//将camctrl的观测框大小进行调整，以匹配
			_camctrl.WinHeight = _WinHeight;
			_camctrl.refreshWin();				//将camctrl.winimage进行更新
			Target.clear();
			_itoa(num, strnum, 10);				//进行文件名编辑
			temp = TwinName + strnum;
			if (Target.initFeature(_camctrl, Template) != 0)
			{
				Target.writeImage(temp);
				num++;
			}
			_camctrl.WinWidth = tempWinWidth;
			_camctrl.WinHeight = tempWinHeight;
		}
	}
}

void autoSacnning2(CamCtrl _camctrl, Mat &_image)
{
	cout << "\tBegin the auto Scanning 2\n" << endl;
	ImageFeature Template;
	Template.initFeature(_camctrl);
	string TwinName = "template";
	Template.writeImage(TwinName);

	ImageFeature Target;
	TwinName = "target";
	vector<Point> pointpos;
	ImageRecognition::getScanPoint(_image, Template, pointpos);
	int num = 0;			//匹配次数计数器
	char strnum[25];
	string temp;
	for (vector<Point>::iterator iter = pointpos.begin(); iter != pointpos.end(); iter++)
	{
		_camctrl.moveWin(*iter);
		Target.clear();
		_itoa(num, strnum, 10);				//进行文件名编辑
		temp = TwinName + strnum;
		if (Target.initFeature(_camctrl, Template) != 0)
		{
			Target.writeImage(temp);
			num++;
		}
	}
}