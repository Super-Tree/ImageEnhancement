/*
	The main function is an example of video dehazing 
	The core algorithm is in "dehazing.cpp," "guidedfilter.cpp," and "transmission.cpp". 
	You may modify the code to improve the results.

	The detailed description of the algorithm is presented
	in "http://mcl.korea.ac.kr/projects/dehazing". See also 
	J.-H. Kim, W.-D. Jang, Y. Park, D.-H. Lee, J.-Y. Sim, C.-S. Kim, "Temporally
	coherent real-time video dehazing," in Proc. IEEE ICIP, 2012.

	Last updated: 2013-02-14
	Author: Jin-Hwan, Kim.
 */

#include <iostream>
#include<io.h> 
#include "cv.h"
#include "highgui.h"
#include "dehazing.h"
#include <time.h>
#include <conio.h>
//静态库头文件
#include "imgHDRlib.h"
#include "estimationMain.h"

void logTableInit(float w);
//////////////
using namespace std;
using namespace cv;
void darkChannel(Mat& src, Mat& darkImage, int Block_size);
void getCurrentFile(string path, vector<string>& files);
int Controller(int *j);
Mat Origin_show; //调试用
#define LOG_LUT_LENGTH 65536
float logTABLE[LOG_LUT_LENGTH] = { 0.0 };
/****************************************************** 
*  时间：2017-5-4  
*  环境：win10 64位 VS2013 openCV版本 2.4.13
*  在transmission.cpp line120  有待改进的地方
*  天空光 选取目前是150帧选一次，以后可以滑动搜索。
*  原方法存在问题： 透射图参数t计算过小，参数难以调节，且透射图计算有大问题。
*  目前方法存在问题：部分素材开始部分会由暗变亮闪烁一次，怀疑是天空光出错。
								去雾后亮度偏低，gamma校正
/******************************************************/
int main()
{
	int nscales = 2;
	int scales[3] = { 251, 161, 251 };
	float weight[3] = { 0.33, 0.33, 0.33 };
	float L = 1.5, R = 1.5;
	float sigma[3] = { 0 };
	logTableInit((float)1 / nscales);
	vector<string> files;
	getCurrentFile("./data/", files);
	int  filesCnt = files.size();
	int j = 0;
	for (j = 0; j < filesCnt; ++j)
	{
		Mat origin;// = imread(files[j].c_str());
		Mat OUT1(origin.size(), CV_8UC3);
		Mat OUT2(origin.size(), CV_8UC3);
		VideoCapture capture(files[j].c_str());
		int frame_number = capture.get(CV_CAP_PROP_FRAME_COUNT);
		int wid = capture.get(CV_CAP_PROP_FRAME_WIDTH);
		int hei = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
		/////
		IplImage *imInput;
		IplImage *imOutput = cvCreateImage(cvSize(wid, hei), IPL_DEPTH_8U, 3);
		dehazing dehazingImg(wid, hei, 16, false, false, 5.0f, 113.0f, 60);
		////
		for (int nFrame = 0; nFrame < (frame_number); nFrame++)
		{
			capture.read(origin);
			Origin_show = origin.clone();
			int flag=estimation(Origin_show, "Origin_show");
			if (flag)
			{
				time_t Node_0 = clock();
				cout << "MSR Node_0 start !!  " << endl;
				RGB_MSRSCB(origin, OUT1, OUT2, nscales, scales, weight, sigma, L, R, logTABLE);
				//HSV_MSRSCB(origin, OUT1, OUT2, nscales, scales, weight, sigma, L, R, logTABLE);
				//imshow("OUT2", OUT2);
				//imshow("Origin_show", origin);
				imshow("OUT1", OUT1);
					waitKey(20);
				if (Controller(&j))
				{
					break;
				}
			}
			else
			{
				//dehazingImg.GammaLUTMaker(0.7f);//用来提高亮度
 				imInput = &IplImage(origin);
				dehazingImg.AirlightSerachRange(cvPoint(120, 0), cvPoint(520, 240));
				dehazingImg.HazeRemoval(imInput, imOutput, nFrame);
				cvShowImage("OUT1", imOutput);
				if (Controller(&j))
				{
					break;	
				}
			}
		}
			cvReleaseImage(&imOutput);
		}
	}

void getCurrentFile(string path, vector<string>& files)
{
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				;
			}
			else
			{
				string temp = path + fileinfo.name;
				files.push_back(temp);
				//files.push_back(p.assign(path).append("\\").append(fileinfo.name) );  
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
void logTableInit(float w)
{
	for (long int i = 0; i < LOG_LUT_LENGTH; i++)
	{
		logTABLE[i] =w*log((float)i + 1);
	}

}
void darkChannel(Mat& src,   Mat& darkImage, int Block_size)
{

 
	clock_t start, finish;
	double totletime;
	start = clock();

	Mat Blue(Block_size, Block_size, CV_8U);
	Mat Green(Block_size, Block_size, CV_8U);
	Mat Red(Block_size, Block_size, CV_8U);

	int t = 0;
	double Blue_min = 0;
	double Green_min = 0;
	double Red_min = 0;
	double min_value = 0;

	t = (Block_size - 1) / 2;

	vector<Mat>channels;
	split(src, channels);
	for (int i = t; i < darkImage.rows - t; ++i)
	{
		for (int j = t; j < darkImage.cols - t; ++j)
		{
			Blue = channels.at(0)(Range(i - t, i + t + 1), Range(j - t, j + t + 1));
			Green = channels.at(1)(Range(i - t, i + t + 1), Range(j - t, j + t + 1));
			Red = channels.at(2)(Range(i - t, i + t + 1), Range(j - t, j + t + 1));

			minMaxLoc(Blue, &Blue_min, NULL, NULL, NULL);
			minMaxLoc(Green, &Green_min, NULL, NULL, NULL);
			minMaxLoc(Red, &Red_min, NULL, NULL, NULL);

			min_value = min(Blue_min, Green_min);
			min_value = min(min_value, Red_min);

			darkImage.at<uchar>(i, j) = (uchar)min_value;
		}
	}

	finish = clock();
	totletime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "暗通道函数运行时间为" << totletime << "秒" << endl;
}
int Controller(int *j)
{
	int keyBoard = waitKey(1);
	if (keyBoard == 2555904)//right
	{
		waitKey(40);
		return 1;
	}
	else if (keyBoard == 2424832)//left
	{
		if (*j > 0)
		{
			*j -= 2;
			waitKey(40);
			return 1;
		}
	}
	else if (keyBoard == 32)
	{
		waitKey();
	   return 0;
	}
	return 0;
}

