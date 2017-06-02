#pragma  once

#include "cv.h"
#include "highgui.h"
#include <iostream>
#include<io.h>
#include <conio.h> 


using namespace std;
using namespace cv;

struct bright
{
	int MBL;
	int MBLh;
	int MBLl;
	int DMP;
};
int estimation(Mat & origin, string name);
int sharpnessValue(Mat & image);
int MaskInit(Point p1, Point p2, int* array);
int zoneBorderInit(Point p1, Point p2, int* array);
float brightnessValue(MatND& histMask, bright& brightness);

Mat plotHist(string name, const MatND& hist);
