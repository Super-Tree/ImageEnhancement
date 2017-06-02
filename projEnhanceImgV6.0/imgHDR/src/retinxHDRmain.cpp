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
#include "imgHDRlib.h"
#include "omp.h"
float logLUT[LOG_LUT_LENGTH] = { 0.0 };
void logLUTinit();
void getJustCurrentFile(string path, vector<string>& files);
void controller(int *j);
/******************************************************/

int HSV_MSRSCB(Mat &input, Mat&output1, Mat&output2, int Nscales, int* scales, float* weight, float *Sigma, float Lcutpercent, float Rcutpercent, float *logTbale)
{
	if (input.type() != CV_8UC3)
	{
		cout << "input.type()!=CV_8UC3" << endl;
		return 4;
	}
	if (input.channels() != 3)
	{
		cout << "input.channels()!=3" << endl;
		return 1;
	}
	if (input.size() != Size(640, 480))
	{
		cout << "input.size() != Size(640, 480" << endl;
		return 2;
	}
	if (input.data == NULL)
	{
		cout << "input.data() = NULL" << endl;
		return 4;
	}

	time_t Node_0 = clock();
	cout << "HSV	Node0 START   " << endl;
	//********************HSV操作*********************************//
	Mat hsvImg(input.size(), CV_8UC1, Scalar(0));
	cvtColor(input, hsvImg, CV_BGR2HSV);
	vector<Mat> hsv{ Mat(input.size(), CV_8U, Scalar(0)), Mat(input.size(), CV_8U, Scalar(0)), Mat(input.size(), CV_8U, Scalar(0)) };
	split(hsvImg, hsv);
	time_t Node_1 = clock();
	cout << "HSV	Node1  cvtColor&SPLIT   " << (float)(Node_1 - Node_0) / CLOCKS_PER_SEC << "secs" << endl;
	Mat V_hsvMSR(input.size(), CV_32FC1, Scalar(0.0));
	singleChn_MSR(hsv[2], V_hsvMSR, scales, Nscales, Sigma, NULL, logTbale);

	time_t Node_2 = clock();
	cout << "HSV	Node1  singleChn_MSR" << (float)(Node_2 - Node_1) / CLOCKS_PER_SEC << "secs" << endl;
	Mat V_hsvMSRSCB(input.size(), CV_32F, Scalar(0.0));
	singleChn_SCB(V_hsvMSR, V_hsvMSRSCB, Lcutpercent, Rcutpercent);
	time_t Node_3 = clock();
	cout << "QNode2 singleChn_SCB   " << (float)(Node_3 - Node_2) / CLOCKS_PER_SEC << "secs" << endl;
	//imshow("scbV", V_hsvMSRSCB);
	V_hsvMSRSCB.convertTo(hsv[2], CV_8UC1);
	//imshow("scbVUCHAR", hsv[2]);
	//imshow("BB", hsv[2]);
	//Mat hsvResult(input.size(), CV_8UC3, Scalar(0.0));
	merge(hsv, output1);
	cvtColor(output1, output1, CV_HSV2BGR);
	//imshow("HSV_RESULT", output1);
	time_t Node_4 = clock();
	cout << "HSV	Node4    Finish   " << "  " << (float)(Node_4 - Node_1) / CLOCKS_PER_SEC << "secs   %%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;

	return 0;
}
int RGB_MSRSCB(Mat &input, Mat&output1, Mat&output2, int Nscales, int* scales, float* weight, float *Sigma, float Lcutpercent, float Rcutpercent, float *logTbale)
{

	if (input.type() != CV_8UC3)
	{
		cout << "input.type()!=CV_8UC3" << endl;
		return 4;
	}
	if (input.channels() != 3)
	{
		cout << "input.channels()!=3" << endl;
		return 1;
	}
	if (input.size() != Size(640, 480))
	{
		cout << "input.size() != Size(640, 480" << endl;
		return 2;
	}
	if (input.data == NULL)
	{
		cout << "input.data() = NULL" << endl;
		return 4;
	}

	cout << "START multi-scale-retinex !!" << endl;
	time_t Node_0 = clock();
	vector<Mat> rgb{ Mat(input.size(), CV_32F, Scalar(0.0)), Mat(input.size(), CV_32F, Scalar(0.0)), Mat(input.size(), CV_32F, Scalar(0.0)) };
	split(input, rgb);
	Mat internsity(input.size(), CV_32F);
	intensityCalculation(internsity, rgb);
	time_t Node_1 = clock();
	cout << "RGB_MSR  Node_1 split&intensity  " << "  cost:  " << (float)(Node_1 - Node_0) / CLOCKS_PER_SEC << "secs  " << endl;

	vector<Mat> rgbMSR{ Mat(input.size(), CV_32F, Scalar(0.0)), Mat(input.size(), CV_32F, Scalar(0.0)), Mat(input.size(), CV_32F, Scalar(0.0)) };
	processMSR(rgb, rgbMSR, scales, Nscales, Sigma, weight, logTbale);
	time_t Node_2= clock();
	cout << "RGB_MSR  Node_2 processMSR  " << "  cost:  " << (float)(Node_2- Node_1) / CLOCKS_PER_SEC << "secs  " << endl;

	//vector<Mat> rgbMSRCR{ Mat(input.size(), CV_32F, Scalar(0.0)), Mat(input.size(), CV_32F, Scalar(0.0)), Mat(input.size(), CV_32F, Scalar(0.0)) };
	//Color_Restoration(rgbMSR, rgbMSRCR, rgb, internsity, logTbale);
	//time_t Node_3 = clock();
	//cout << "RGB_MSR  Node_3 Color_Restoration  " << "  cost:  " << (float)(Node_3 - Node_2) / CLOCKS_PER_SEC << "secs  " << endl;

	//vector<Mat> rgbMSRCRSCB{ Mat(input.size(), CV_32F, Scalar(0.0)), Mat(input.size(), CV_32F, Scalar(0.0)), Mat(input.size(), CV_32F, Scalar(0.0)) };
	//simplest_color_balance(rgbMSRCR, rgbMSRCRSCB, Lcutpercent, Rcutpercent);
	//time_t Node_4= clock();
	//cout << "RGB_MSR  Node_4 simplest_color_balance  " << "  cost:  " << (float)(Node_4 - Node_3) / CLOCKS_PER_SEC << "secs  " << endl;

	//Mat ResultImg8(input.size(), CV_32FC3, Scalar(0, 0, 0));
	//merge(rgbMSRCRSCB, ResultImg8);
	//ResultImg8.convertTo(output2, CV_8UC3);
	//cout << "MSR CR SCB DONE" << endl;

	time_t Node_5 = clock();
 
	vector<Mat> rgbMSRSCB{ Mat(input.size(), CV_32F, Scalar(0.0)), Mat(input.size(), CV_32F, Scalar(0.0)), Mat(input.size(), CV_32F, Scalar(0.0)) };
	simplest_color_balance(rgbMSR, rgbMSRSCB, Lcutpercent, Rcutpercent);
	time_t Node_6 = clock();
	cout << "RGB_MSR  Node_5 simplest_color_balance without CR " << "  cost:  " << (float)(Node_6 - Node_5) / CLOCKS_PER_SEC << "secs  " << endl;

	Mat resultMSRSCB(input.size(), CV_32FC3, Scalar(0, 0, 0));
	merge(rgbMSRSCB, resultMSRSCB);
	resultMSRSCB.convertTo(output1, CV_8UC3);
	time_t Node_7= clock();
	cout << "RGB_MSR  Node_5 MSR SCB " << "  cost:  " << (float)(Node_7 - Node_0) / CLOCKS_PER_SEC << "secs %%%%%%%%%%%%%%%%%%% " << endl << endl << endl << endl;
 
	return 0;
}

int main3()
{

	logLUTinit();
	vector<string> files;
	getJustCurrentFile("./data/", files);
	int nscales = 1;
	int scales[3] = { 251, 251, 311 };
	float Sigma[3] = { 0 };
	float cutPercL = 1.5, cutPercR = 1.5;
	float weight[3] = { 1.0, 1.0, 0.0 };

	for (int j = 0; j < files.size(); ++j)
	{

		cout << "START retinex" << "PicNum:" << j << "   Name:" << files[j] << endl;
		time_t start_t = clock();
		Mat origin = imread(files[j].c_str(), IMREAD_COLOR);
		Mat out1(origin.size(), CV_8UC3, Scalar(0));		Mat out2(origin.size(), CV_8UC3, Scalar(0));
		RGB_MSRSCB(origin, out1, out2, nscales, scales, weight, Sigma, cutPercL, cutPercR, logLUT);
		imshow("out1", out1);
		imshow("out2", out2);
		waitKey();
		////////////////////////////////////////////////////////////////
		imshow("origin", origin);
		vector<Mat> rgb{ Mat(origin.size(), CV_32F, Scalar(0.0)), Mat(origin.size(), CV_32F, Scalar(0.0)), Mat(origin.size(), CV_32F, Scalar(0.0)) };
		split(origin, rgb);
		Mat internsity(origin.size(), CV_32F);
		intensityCalculation(internsity, rgb);
		vector<Mat> rgbMSR{ Mat(origin.size(), CV_32F, Scalar(0.0)), Mat(origin.size(), CV_32F, Scalar(0.0)), Mat(origin.size(), CV_32F, Scalar(0.0)) };
		time_t Node_1 = clock();
		cout << "Node1 Load&Split counts " << (float)(Node_1 - start_t) / CLOCKS_PER_SEC << "secs" << endl;

		processMSR(rgb, rgbMSR, scales, nscales, Sigma, weight, logLUT);
		time_t Node_2 = clock();
		cout << "Node2 GaussianBlur count " << (float)(Node_2 - Node_1) / CLOCKS_PER_SEC << "secs" << endl;
		//***********************MSR CR SCB方法**********************//
		vector<Mat> rgbMSRCR{ Mat(origin.size(), CV_32F), Mat(origin.size(), CV_32F), Mat(origin.size(), CV_32F) };
		Color_Restoration(rgbMSR, rgbMSRCR, rgb, internsity, logLUT);

		vector<Mat> rgbMSRCRSCB{ Mat(origin.size(), CV_32F), Mat(origin.size(), CV_32F), Mat(origin.size(), CV_32F) };
		simplest_color_balance(rgbMSRCR, rgbMSRCRSCB, cutPercL + 0.5, cutPercR + 0.5);
		Mat ResultImg8(origin.size(), CV_32FC3, Scalar(0, 0, 0));
		merge(rgbMSRCRSCB, ResultImg8);
		ResultImg8.convertTo(ResultImg8, CV_8UC3);
		imshow("enhanceByMSRCRSCB", ResultImg8);
		time_t Node_3 = clock();
		cout << "Node3 MSR CR SCB count " << (float)(Node_3 - Node_2) / CLOCKS_PER_SEC << "secs" << endl;
		//***********************MSR SCB CP **********************//
		vector<Mat> rgbMSRSCB{ Mat(origin.size(), CV_32F), Mat(origin.size(), CV_32F), Mat(origin.size(), CV_32F) };
		simplest_color_balance(rgbMSR, rgbMSRSCB, cutPercL, cutPercR);
		Mat resultMSRSCB(origin.size(), CV_32FC3, Scalar(0, 0, 0));
		merge(rgbMSRSCB, resultMSRSCB);
		resultMSRSCB.convertTo(resultMSRSCB, CV_8UC3);
		imshow("rgbMSRSCB", resultMSRSCB);
		time_t Node_4 = clock();
		cout << "Node4 SCB count " << (float)(Node_4 - Node_3) / CLOCKS_PER_SEC << "secs" << endl;


		//Mat internsity1(origin.size(), CV_32F);
		//intensityCalculation(internsity1, rgbMSRSCB);
		//vector<Mat> rgbMSRCPSCB{ Mat(origin.size(), CV_32F), Mat(origin.size(), CV_32F), Mat(origin.size(), CV_32F) };
		//chromaPreserve(rgbMSRSCB, rgbMSRCPSCB, rgbMSRSCB, internsity, internsity1);
		//Mat ResultImg2(origin.size(), CV_32FC3, Scalar(0, 0, 0));
		//merge(rgbMSRCPSCB, ResultImg2);
		//ResultImg2.convertTo(ResultImg2, CV_8UC3);
		//imshow("enhanceByMSRCPSCB", ResultImg2);
		time_t Node_5 = clock();
		cout << "Node5 CP count " << (float)(Node_5 - Node_4) / CLOCKS_PER_SEC << "secs" << endl;


		cout << "FINISH Retinex done ,count:" << (float)(clock() - start_t) / CLOCKS_PER_SEC << "secs %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
		controller(&j);

	}
	return 0;
}

int singleChn_SCB(Mat&  data_in, Mat& data_out, float cutPercentS, float cutPercentL)
{
	int image_size = data_in.rows*data_in.cols;
	float *sortdata = (float *)malloc(image_size*sizeof(float));

	if (data_in.isContinuous())
	{
		memcpy(sortdata, data_in.data, image_size*sizeof(float));
		cout << "   memcpy success!" << endl;
	}
	else
	{
		for (int n1 = 0; n1 < data_in.rows; n1++)
		{
			for (int n2 = 0; n2 < data_in.cols; n2++)
			{
				*(sortdata + n1*data_in.cols + n2) = data_in.at<float>(n1, n2);
			}
		}
	}
	time_t iNode1 = clock();
	qsort(sortdata, image_size, sizeof(sortdata[0]), &floatComparison);
	time_t iNode2 = clock();
	cout << "   iNode a :" << " qsort count " << (float)(iNode2 - iNode1) / CLOCKS_PER_SEC << "secs" << endl;


	int per1 = (int)(cutPercentS*image_size / 100);
	float min = sortdata[per1];

	int per2 = (int)(cutPercentL*image_size / 100);
	float max = sortdata[image_size - 1 - per2];
	//for (int n1 = 0; n1 < data_in.rows; n1++)
	//{
	//	for (int n2 = 0; n2 < data_in.cols; n2++)
	//	{
	//		if (data_in.at<float>(n1, n2)>max)
	//		{
	//			origin_copy.at<Vec3b>(n1, n2) = Vec3b(0, 255, 0);
	//		}
	//		else if ((data_in.at<float>(n1, n2) < min))
	//		{
	//			origin_copy.at<Vec3b>(n1, n2) = Vec3b(255, 0, 0);
	//		}
	//	}
	//}
	//imshow("cult", origin_copy);

	if (max <= min)
	{
		cout << "ERROR :simplest_color_balance function: max<min" << endl;
		return  1;
	}
	else
	{
		float scale = 255. / (max - min);
		for (int i = 0; i < data_in.rows; i++)
		{
			for (int j = 0; j < data_in.cols; j++)
			{
				if (data_in.at<float>(i, j) < min)
					data_out.at<float>(i, j) = 0.;
				else if (data_in.at<float>(i, j) > max)
					data_out.at<float>(i, j) = 255.;
				else
					data_out.at<float>(i, j) = scale*(data_in.at<float>(i, j) - min);
			}
		}
	}
	time_t iNode3 = clock();
	cout << "   iNode b :" << "reflect count " << (float)(iNode3 - iNode2) / CLOCKS_PER_SEC << "secs" << endl;
	free(sortdata);
	return 0;
}
void singleChn_MSR(Mat& inputArry, Mat& outAarry, int *scale, int nscales, float *sigma, float *w, float *logLUT)
{
	// float w = (float)1/nscales;
	Mat pasArry(inputArry.size(), CV_32FC1, Scalar(0));
	Mat temp(inputArry.size(), CV_32FC1, Scalar(0));
	Mat temp1(inputArry.size(), CV_32FC1, Scalar(0));
	for (size_t k = 0; k < nscales; ++k)
	{
		inputArry.convertTo(temp1, CV_32FC1);
		GaussianBlur(temp1, pasArry, Size(scale[k], scale[k]), sigma[k], sigma[k]);
		//imshow("gaussian blur", pasArry);
		//imshow("origin blur", inputArry);
		//float weight = *(w + k);
		for (size_t i = 0; i < inputArry.rows; ++i)
		{
			for (size_t j = 0; j < inputArry.cols; ++j)
			{
				outAarry.at<float>(i, j) += (logLUT[inputArry.at<uchar>(i, j)] - logLUT[(int)(pasArry.at<float>(i, j) + 0.5)]); //weight*(logLUT[inputArry[0].at<uchar>(i, j)] - logLUT[pasArry[0].at<uchar>(i, j)]);//公式（6）
				//temp.at<float>(i, j) = (logLUT[inputArry.at<uchar>(i, j)] - logLUT[(int)(pasArry.at<float>(i, j) + 0.5)]); //weight*(logLUT[inputArry[0].at<uchar>(i, j)] - logLUT[pasArry[0].at<uchar>(i, j)]);//公式（6）
			}
		}
		//imshow("temp", temp);
		//	imshow("sub", outAarry);
		cout << "   GaussianING : " << scale[k] << endl;
		//waitKey();
	}
}

void chromaPreserve(vector<Mat>inMSRSCB, vector<Mat>inMSRCPSCB, vector<Mat> &origin, Mat& inten, Mat& inten1)
{
	float temp1, temp2;
	float A, B = 0;
	for (int i = 0; i < origin[0].rows; i++)
		for (int j = 0; j < origin[0].cols; j++)
		{
			//B = myMax3(origin[0].at<float>(i, j), origin[1].at<float>(i, j), origin[2].at<float>(i, j));
			//temp1 = 255.0/(B+1);
			temp2 = inten1.at<float>(i, j) / (inten.at<float>(i, j) + 1);
			//if (temp1<temp2)
			//	A = temp1;
			//else
			A = temp2;
			inMSRCPSCB[0].at<float>(i, j) = A*origin[0].at<float>(i, j);
			inMSRCPSCB[1].at<float>(i, j) = A*origin[1].at<float>(i, j);
			inMSRCPSCB[2].at<float>(i, j) = A*origin[2].at<float>(i, j);
		}
}

void simplest_color_balance(vector<Mat>&  data, vector<Mat>& data_out, float cutPercentS, float cutPercentL)
{

	int image_size = data[0].rows*data[0].cols;
	float *sortdata = (float *)malloc(image_size*sizeof(float));
#pragma omp parallel for
	for (int k = 0; k < 3; k++)
	{
		cout <<"%%"<<k<<"%%"<< endl;
		for (int n1 = 0; n1 < data[0].rows; n1++)
		{
			for (int n2 = 0; n2 < data[0].cols; n2++)
			{
				*(sortdata + n1*data[0].cols + n2) = data[k].at<float>(n1, n2);
			}
		}
		time_t iNode = clock();
		qsort(sortdata, image_size, sizeof(sortdata[0]), &floatComparison);
		cout << "   iNode:" << k << " qsort count " << (float)(clock() - iNode) / CLOCKS_PER_SEC << "secs" << endl;


		int per1 = (int)(cutPercentS*image_size / 100);
		float min = sortdata[per1];

		int per2 = (int)(cutPercentL*image_size / 100);
		float max = sortdata[image_size - 1 - per2];

		if (max <= min)
		{
			cout << "ERROR :simplest_color_balance function: max<min" << endl;
			//return  1;
		}
		else
		{
			float scale = 255. / (max - min);
			for (int i = 0; i < data[k].rows; i++)
			{
				for (int j = 0; j < data[k].cols; j++)
				{
					if (data[k].at<float>(i, j) < min)
						data_out[k].at<float>(i, j) = 0.;
					else if (data[k].at<float>(i, j) > max)
						data_out[k].at<float>(i, j) = 255.;
					else
						data_out[k].at<float>(i, j) = scale*(data[k].at<float>(i, j) - min);
				}
			}
		}

	}
	free(sortdata);
	//return 0;



}

int floatComparison(const void *x, const void *y)
{
	float dx, dy;

	dx = *(float *)x;
	dy = *(float *)y;

	if (dx < dy)
	{
		return -1;
	}
	else if (dx > dy)
	{
		return +1;
	}
	return 0;
}

void intensityCalculation(Mat &gray, vector<Mat>& rgbChannel)
{
	int nCols = rgbChannel[0].cols;
	int nRows = rgbChannel[0].rows;
	if (rgbChannel[0].isContinuous() && rgbChannel[1].isContinuous() && rgbChannel[2].isContinuous())
	{
		//cout << "intensityCalculation function's calculation is continuous" << endl;
		nCols *= nRows;
		nRows = 1;
	}
	int i, j;
	float* p;
	uchar* r;
	uchar* g;
	uchar* b;
	for (i = 0; i < nRows; ++i)
	{
		p = gray.ptr<float>(i);
		r = rgbChannel[0].ptr<uchar>(i);
		g = rgbChannel[1].ptr<uchar>(i);
		b = rgbChannel[2].ptr<uchar>(i);
		for (j = 0; j < nCols; ++j)
		{
			p[j] = (float)(r[j] + g[j] + b[j]) / 3;
		}
	}
}

void Color_Restoration(vector<Mat>& inputMSR, vector<Mat>& outMSRCR, vector<Mat>& inputORI, Mat &intensity, float* logTable)
{
	float alpha = 125.0;	int belta = 46;
	float A;
	float Gain = 192;
	float b = -30;
	for (int m = 0; m < 3; m++)
	{
		for (int j = 0; j < inputMSR[m].rows; j++)
		{
			for (int i = 0; i < inputMSR[m].cols; i++)
			{ //以下实现公式（7）
				int index = (intensity.at<float>(j, i)) * 3;
				A = logTable[index];
				int indexi = alpha* inputORI[m].at<uchar>(j, i);
				outMSRCR[m].at<float>(j, i) = inputMSR[m].at<float>(j, i) * (logTable[indexi] - A);
				outMSRCR[m].at<float>(j, i) = Gain*outMSRCR[m].at<float>(j, i) + b;
			}
		}
	}
}

void processMSR(vector<Mat>& inputArry, vector<Mat>& outAarry, int *scale, int nscales, float *sigma, float *w, float*logTable)
{
	// float w = (float)1/nscales;

	vector<Mat> pasArryR{ Mat(inputArry[0].size(), CV_8UC1, Scalar(0)), Mat(inputArry[0].size(), CV_8UC1, Scalar(0)), Mat(inputArry[0].size(), CV_8UC1, Scalar(0)) };/* Compute Retinex output*/
	vector<Mat> pasArryG{ Mat(inputArry[0].size(), CV_8UC1, Scalar(0)), Mat(inputArry[0].size(), CV_8UC1, Scalar(0)), Mat(inputArry[0].size(), CV_8UC1, Scalar(0)) };/* Compute Retinex output*/
	vector<Mat> pasArryB{ Mat(inputArry[0].size(), CV_8UC1, Scalar(0)), Mat(inputArry[0].size(), CV_8UC1, Scalar(0)), Mat(inputArry[0].size(), CV_8UC1, Scalar(0)) };/* Compute Retinex output*/
	time_t Node_0 = clock();
   #pragma omp parallel  //四线程  10*3 30+3 30+3 30+3 
	{ 
		#pragma omp sections nowait //第个sections  
		{
				#pragma omp section
			   { 
				   time_t t1 = clock();

				   GaussianBlur(inputArry[0], pasArryB[1], Size(scale[1], scale[1]), sigma[1], sigma[1]); // 0通道 scale[0]尺度变换
				   GaussianBlur(inputArry[1], pasArryG[1], Size(scale[1], scale[1]), sigma[1], sigma[1]); // 0通道 scale[0]尺度变换
				   GaussianBlur(inputArry[2], pasArryR[1], Size(scale[1], scale[1]), sigma[1], sigma[1]); // 0通道 scale[0]尺度变换
				   time_t t2 = clock();
				   float a = (float)(t2 - t1) / CLOCKS_PER_SEC;
				   cout << "section 1 线程ID：" << omp_get_thread_num() << "\n";
				   cout << " " << "  cost:  " << a<< "secs  " << endl;

			   }
				#pragma omp section
			   {  
				   time_t t3= clock();

			       GaussianBlur(inputArry[0], pasArryB[0], Size(scale[0], scale[0]), sigma[0], sigma[0]); // 0通道 scale[0]尺度变换
				   GaussianBlur(inputArry[0], pasArryB[2], Size(scale[2], scale[2]), sigma[2], sigma[2]); // 0通道 scale[2]尺度变换
				   time_t t4 = clock();
				   float b = (float)(t4 - t3) / CLOCKS_PER_SEC;
				   cout << "section 2 线程ID：" << omp_get_thread_num() << "\n";
				   cout << " " << "  cost:  " <<b<< "secs  " << endl;
			   }
		}
		#pragma omp sections nowait  //第个sections  
		{
				#pragma omp section
			   {				   
				   time_t t5 = clock();

				   GaussianBlur(inputArry[1], pasArryG[0], Size(scale[0], scale[0]), sigma[0], sigma[0]); // 0通道 scale[0]尺度变换
				   GaussianBlur(inputArry[1], pasArryG[2], Size(scale[2], scale[2]), sigma[2], sigma[2]); // 0通道 scale[2]尺度变换
				   time_t t6 = clock();
				   cout << "section 3 线程ID：" << omp_get_thread_num() << "\n";
				   float c = (float)(t6 - t5) / CLOCKS_PER_SEC;
				   cout << " " << "  cost:  " << c<< "secs  " << endl;
			   }
				#pragma omp section
			   {
				   time_t t7 = clock();
				  
				   GaussianBlur(inputArry[2], pasArryR[0], Size(scale[0], scale[0]), sigma[0], sigma[0]); // 0通道 scale[0]尺度变换
				   GaussianBlur(inputArry[2], pasArryR[2], Size(scale[2], scale[2]), sigma[2], sigma[2]); // 0通道 scale[2]尺度变换
				   time_t t8= clock();
				   cout << "section 4 线程ID：" << omp_get_thread_num() << "\n";
				   float d = (float)(t8 - t7) / CLOCKS_PER_SEC;
				   cout << " " << "  cost:  " <<d << "secs  " << endl;
			   }
		}
	}
	//imshow("ada",pasArryR[1]);
	//waitKey();
	time_t Node_1 = clock();
	cout << "Parallel Gaussian smooth "<< "  cost:  " << (float)(Node_1 - Node_0) / CLOCKS_PER_SEC << "secs  " << endl;
 #pragma omp parallel for
	for (int k = 0; k < nscales; ++k)
	{
		for (int i = 0; i < inputArry[0].rows; ++i)
		{
			for (int j = 0; j < inputArry[0].cols; ++j)
			{
				//float 	a1 = logTable[inputArry[0].at<uchar>(i, j)];
				//float     a2 = logTable[pasArryB[k].at<uchar>(i, j)];
				//if (a1 > a2)
				//{
				//	outAarry[0].at<float>(i, j) += (a1-a2);
				//}				
				//float 	b1 = logTable[inputArry[1].at<uchar>(i, j)];
				//float     b2 = logTable[pasArryG[k].at<uchar>(i, j)];
				//if (b1 > b2)
				//{
				//	outAarry[1].at<float>(i, j) += (b1-b2);
				//}
				//float 	c1 = logTable[inputArry[2].at<uchar>(i, j)];
				//float     c2 = logTable[pasArryR[k].at<uchar>(i, j)];
				//if (c1 > c2)
				//{
				//	outAarry[2].at<float>(i, j) += (c1 - c2);
				//}
				outAarry[0].at<float>(i, j) += (logTable[inputArry[0].at<uchar>(i, j)] - logTable[pasArryB[k].at<uchar>(i, j)]);//公式（6）
				outAarry[1].at<float>(i, j) += (logTable[inputArry[1].at<uchar>(i, j)] - logTable[pasArryG[k].at<uchar>(i, j)]);
				outAarry[2].at<float>(i, j) += (logTable[inputArry[2].at<uchar>(i, j)] - logTable[pasArryR[k].at<uchar>(i, j)]);
			}
		}
	}
		time_t Node_4 = clock();
		cout << "   R,G,B,Three Gaussian addition  " << "  cost:  " << (float)(Node_4 - Node_1) / CLOCKS_PER_SEC << "secs  " << endl;

	//// float w = (float)1/nscales;
	//time_t Node_a1 = clock();
	//vector<Mat> pasArry1{ Mat(inputArry[0].size(), CV_8UC1, Scalar(0)), Mat(inputArry[0].size(), CV_8UC1, Scalar(0)), Mat(inputArry[0].size(), CV_8UC1, Scalar(0)) };/* Compute Retinex output*/
	//
	//for (size_t k = 0; k < nscales; ++k)
	//{
	//	time_t Node_0 = clock();
	//	GaussianBlur(inputArry[0], pasArry1[0], Size(scale[k], scale[k]), sigma[k], sigma[k]);
	//	//time_t Node_1 = clock();
	//	//cout <<"   GaussianING :  " << scale[k] <<"  cost:  "<<  (float)(Node_1 - Node_0) / CLOCKS_PER_SEC << "secs  " << endl;

	//	GaussianBlur(inputArry[1], pasArry1[1], Size(scale[k], scale[k]), sigma[k], sigma[k]);
	//	//time_t Node_2= clock();
	//	//cout << "   GaussianING :  " << scale[k] << "  cost:  " << (float)(Node_2 - Node_1) / CLOCKS_PER_SEC << "secs  " << endl;

	//	GaussianBlur(inputArry[2], pasArry1[2], Size(scale[k], scale[k]), sigma[k], sigma[k]);
	//	time_t Node_3 = clock();
	//	cout << "   R,G,B,Three GaussianSmooth  " << "Scale:  " << scale[k] << "  cost:  " << (float)(Node_3 - Node_0) / CLOCKS_PER_SEC << "secs  " << endl;

	//	// imshow("inputArry", inputArry[0]);	
	//	// imshow("pasArry1", pasArry1[0]);
	//	//  float weight = *(w+k);
	//	for (size_t i = 0; i < inputArry[0].rows; ++i)
	//	{
	//		for (size_t j = 0; j < inputArry[0].cols; ++j)
	//		{
	//			outAarry[0].at<float>(i, j) += (logTable[inputArry[0].at<uchar>(i, j)] - logTable[pasArry1[0].at<uchar>(i, j)]);//公式（6）
	//			outAarry[1].at<float>(i, j) += (logTable[inputArry[1].at<uchar>(i, j)] - logTable[pasArry1[1].at<uchar>(i, j)]);
	//			outAarry[2].at<float>(i, j) += (logTable[inputArry[2].at<uchar>(i, j)] - logTable[pasArry1[2].at<uchar>(i, j)]);
	//		}
	//	}

	//	time_t Node_4 = clock();
	//	cout << "   R,G,B,Three Gaussian addition  " << "  cost:  " << (float)(Node_4 - Node_3) / CLOCKS_PER_SEC << "secs  " << endl;
	//}

	//time_t Node_a4 = clock();
	//cout << "   Inside R,G,B,Three Gaussian addition  " << "  cost:  " << (float)(Node_a4 - Node_a1) / CLOCKS_PER_SEC << "secs  " << endl;
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

void Mat2Arry(Mat &mata, uchar*data)
{
	int nCols = mata.cols;
	int nRows = mata.rows;
	if (mata.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}
	uchar* p;
	for (size_t i = 0; i < nRows; ++i)
	{
		p = mata.ptr<uchar>(i);
		for (size_t j = 0; j < nCols; ++j)
		{
			data[i*nRows + j] = p[j];
		}
	}
}

void Arry2Mat(uchar *data, Mat &mata)
{
	int nCols = mata.cols;
	int nRows = mata.rows;
	if (mata.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}
	uchar* p;
	for (size_t i = 0; i < nRows; ++i)
	{
		p = mata.ptr<uchar>(i);
		for (size_t j = 0; j < nCols; ++j)
		{
			p[j] = data[i*nRows + j];
		}
	}
}

int myMax3(uchar n1, uchar n2, uchar n3)
{
	int b = 0;
	if (n1 > n2)
		b = n1;
	else b = n2;
	if (b < n3)
		b = n3;
	return b;

}

void controller(int *j)
{
	int keyBoard = waitKey();
	if (keyBoard == 2555904)//right
	{
		waitKey(40);

	}
	else if (keyBoard == 2424832)//left
	{
		if (*j > 0)
		{
			*j -= 2;
			waitKey(40);

		}
	}
	else if (keyBoard == 32)
	{
		*j--;
	}
}

void logLUTinit()
{
	for (long int i = 0; i < LOG_LUT_LENGTH; i++)
	{
		logLUT[i] = log((float)i + 1);
	}

}