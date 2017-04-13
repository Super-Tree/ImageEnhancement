#include "cv.h"
#include "highgui.h"
#include <iostream>
#include<io.h>

using namespace std;
using namespace cv;
 
float brightness_value(const MatND& hist, int div, int * mbl, int*mbll, int* mblh);
void getJustCurrentFile(string path, vector<string>& files);
Mat getHistImg(string name, const MatND& hist);
int MBL = 0;
int MBLh = 0;
int MBLl = 0;
int DMP = 0;

int main()
{
	vector<string> files;
	getJustCurrentFile("./data/", files);
	for (int i = 0; i < files.size(); i++)
	{
		    //string fileName ="./data/pic" + format("%d", i) + ".png";   // "./data/pic01.png";   // 
			//Mat origin = imread(fileName);
		    Mat origin = imread(files[i]);
			if (origin.empty())
			{
				cout << "Read picture failed!" << endl;
				waitKey();
				return -1;
			}
			Mat gray_img;
			cvtColor(origin, gray_img, CV_RGB2GRAY);
			/***********************************************/
			const int channels[1] = { 0 };
			const int histSize[1] = { 256 };
			float hranges[2] = { 0, 255 };
			const float* ranges[1] = { hranges };
			MatND hist,hist2;
			Mat histMask = imread("./data/ZhistMask.jpg");
			float a = 0.85;
			float b = (1 -a); 
			Mat dst;
			addWeighted(origin, a, histMask, b, 0.0, dst);		 
			cvtColor(histMask, histMask, CV_RGB2GRAY);
			threshold(histMask, histMask, 170, 255, CV_THRESH_BINARY);
			calcHist(&gray_img, 1, channels, histMask, hist, 1, histSize, ranges);
			calcHist(&gray_img, 1, channels, Mat(), hist2, 1, histSize, ranges);
			getHistImg("One",hist);
			getHistImg("Two",hist2);
			//waitKey();
			float perc = brightness_value(hist, 1, &MBL, &MBLl, &MBLh);
			DMP = MBL - (MBLl + MBLh) / 2;
			string result ="Delta:"+format("%.2f", perc) + "%";
			string num = "DMP: " + format("%d", DMP) + " MBL: " + format("%d", MBL);
			putText(dst, result, Point(20,20), FONT_HERSHEY_SIMPLEX,0.5, Scalar(255, 0, 0),1, 4);
			putText(dst, num, Point(120,20), FONT_HERSHEY_SIMPLEX,0.5, Scalar(255, 0, 0), 1, 4);
			imshow("result",dst);
			/***********************************************/
			waitKey();
	}
	return 0;
}



Mat getHistImg(string name,const MatND& hist_)
{
	double maxVal = 0;
	double minVal = 0;

	//找到直方图中的最大值和最小值
	minMaxLoc(hist_, &minVal, &maxVal, 0, 0);
	int histSize = hist_.rows;
	Mat histImg(histSize, histSize, CV_8U, Scalar(255));
	// 设置最大峰值为图像高度的90%
	int hpt = static_cast<int>(0.9*histSize);

	for (int h = 0; h<histSize; h++)
	{
		float binVal = hist_.at<float>(h);
		int intensity = static_cast<int>(binVal*hpt / maxVal);
		line(histImg, Point(h, histSize), Point(h, histSize - intensity), Scalar::all(0));
		imshow(name, histImg);
	}

	return histImg;
}

/***********************************
*const Mat & image
*
/***********************************/
float brightness_value(const MatND& hist, int div, int * mbl, int*mbll, int* mblh)
{
	int numPoint = 0;
	for (size_t i = 0; i<hist.rows; i++)
	{
		*mbl += hist.at<float>(i) * i;
		numPoint += hist.at<float>(i);
	}
	*mbl /= numPoint;

	if (*mbl > 255)
	{
		cout << "MBL is out of range!" << "NUM: " << *mbl << endl;
		return -1;
	}

	int temp = 0;
	for (size_t i = *mbl; i <hist.rows; i++)
	{
		*mblh += hist.at<float>(i)* i;
		temp += hist.at<float>(i);
	}

	if (temp == 0)
	{
		*mblh = 0;
	}
	else
	{
		*mblh /= temp;
	}

	int temp1 = 0;
	for (size_t i = 0; i < *mbl; i++)
	{
		*mbll += hist.at<float>(i) * i;
		temp1 += hist.at<float>(i);
	}
	if (temp1 == 0)
	{
		*mbll = 0;
	}
	else
	{
		*mbll /= temp1;
	}

	float percent = (float)(*mbl - (*mbll + *mblh) / 2) * 100 / *mbl;


	return percent;

}

void getJustCurrentFile(string path, vector<string>& files)
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