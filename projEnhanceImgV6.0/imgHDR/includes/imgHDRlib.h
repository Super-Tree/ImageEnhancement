

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "math.h"
 

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif
#define PI2  6.283185307179586  /* 2*pi*/

#define LOG_LUT_LENGTH 65536

#include <iostream>
#include<io.h> 
#include "cv.h"
#include "highgui.h"
#include <time.h>
#include <conio.h>
using namespace std;
using namespace cv;
 

int myMax3(uchar n1, uchar n2, uchar n3);
int HSV_MSRSCB(Mat &input, Mat&output1, Mat&output2, int Nscales, int* scales, float* weight, float *Sigma, float Lcutpercent, float Rcutpercent, float *logTbale);
int singleChn_SCB(Mat&  data_in, Mat& data_out, float cutPercentS, float cutPercentL);
void singleChn_MSR(Mat& inputArry, Mat& outAarry, int *scale, int nscales, float *sigma, float *w, float *logLUT);

/******************************************************/
int floatComparison(const void *x, const void *y);
void convolution(Mat& inputArry, int scale, Mat& outputArry);
void intensityCalculation(Mat &gray, vector<Mat>& rgbChannel);
void MSRetinex(vector<Mat>& inputArry, vector<Mat>& outAarry, int *scale, int nscales, float w);
void processMSR(vector<Mat>& inputArry, vector<Mat>& outAarry, int *scale, int nscales, float *sigma, float *w, float* logTable);
void Mat2Arry(Mat &mata, uchar *data);
void Arry2Mat(uchar *data, Mat &mata);
void chromaPreserve(vector<Mat>inMSRSCB, vector<Mat>inMSRCPSCB, vector<Mat> &origin, Mat& inten, Mat& inten1);
void simplest_color_balance(vector<Mat>&  data, vector<Mat>& data_out, float cutPercentS, float cutPercentL);
void Color_Restoration(vector<Mat>& inputMSR, vector<Mat>& outMSRCR, vector<Mat>& inputORI, Mat &intensity, float* logTable);
int RGB_MSRSCB(Mat &input, Mat&output1, Mat&output2, int Nscales, int* scales, float* weight, float *Sigma, float Lcutpercent, float Rcutpercent, float* logTable);