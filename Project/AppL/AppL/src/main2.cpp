#include "cv.h"
#include "highgui.h"
#include <iostream>

using namespace std;
using namespace cv;
const int LENGTH = 256;
int brightness_value(const Mat & image, int div, int * MBL, int* MBLl, int* MBLh);
int MBL = 0;
int MBLh = 0;
int MBLl = 0;

int main2()
{
	for (int i = 1; i < 18; i++)
	{
		string fileName = "./data/pic" + format("%d", i) + ".jpg";
		Mat origin = imread(fileName);
		Mat gray_img;
		cvtColor(origin, gray_img, CV_RGB2GRAY);
		int perc = brightness_value(gray_img, 1, &MBL, &MBLl, &MBLh);
		string result = format("%d", perc) + "%";
		putText(origin, result, Point(40, 40), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 4, 8);
		imshow("Origin", origin);
		waitKey();
	}
	return 0;
}
/***********************************
*const Mat & image
*
/***********************************/

int brightness_value(const Mat & image, int div, int * mbl, int*mbll, int* mblh)
{
	int nr = image.rows;
	int nc = image.cols;
	vector<int> gray_distribution(LENGTH, 0);
	if (image.isContinuous())
	{
		nr = 1;
		nc = nc*image.rows*image.channels();
	}

	for (int i = 0; i < nr; i++)
	{
		const  uchar* inData = image.ptr<uchar>(i);
		for (int j = 0; j < nc; j++)
		{
			if (*inData >= LENGTH)
			{
				cout << "Out of range" << endl;
				return -1;
			}
			gray_distribution[*inData++]++;
		}

	}

	for (size_t i = 0; i<LENGTH; i++)
	{
		*mbl += gray_distribution[i] * i;
	}
	*mbl /= image.rows*image.cols;
	if (*mbl > 255)
	{
		cout << "MBL is out of range!" << "NUM: " << *mbl << endl;
		return -1;
	}

	int temp = 0;
	for (size_t i = *mbl; i < LENGTH; i++)
	{
		*mblh += gray_distribution[i] * i;
		temp += gray_distribution[i];
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
		*mbll += gray_distribution[i] * i;
		temp1 += gray_distribution[i];
	}
	if (temp1 == 0)
	{
		*mbll = 0;
	}
	else
	{
		*mbll /= temp1;
	}

	int percent = (*mbl - (*mbll + *mblh) / 2) * 100 / *mbl;


	return percent;

}