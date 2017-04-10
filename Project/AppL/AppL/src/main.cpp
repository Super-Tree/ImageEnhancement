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

int main( )
{
	Mat origin = imread("./data/pic1.jpg");
	imshow("Origin",origin);
	Mat gray_img;
	cvtColor(origin, gray_img, CV_RGB2GRAY);
 
	brightness_value(gray_img, 1, &MBL,&MBLl,&MBLh);

	waitKey();
	return 0;
}
/*********************************** 
*const Mat & image  ª“∂»Õº
*
/***********************************/
vector<int> gray_distribution(LENGTH,0);
int brightness_value(const Mat & image,int div,int * mbl,int*mbll, int* mblh  )
{
	int nr = image.rows;
	int nc = image.cols;
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
						if (*inData >=LENGTH)
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

 
	int temp = 0;
	for (size_t i = *mbl; i < LENGTH; i++)
	{
		*mblh += gray_distribution[i] * i;
		temp += gray_distribution[i];
	}
	*mblh /= temp;
	
 
	int temp1 = 0;
	for (size_t i = 0; i < *mbl; i++)
	{
		*mbll+= gray_distribution[i] * i;
		temp1 += gray_distribution[i];
	}
		*mbll /= temp1;

	return 0;

}


