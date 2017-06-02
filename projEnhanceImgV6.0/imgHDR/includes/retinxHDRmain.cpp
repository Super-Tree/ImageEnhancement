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
float logLUT[LOG_LUT_LENGTH] = { 0.0 };
void logLUTinit();
void getJustCurrentFile(string path, vector<string>& files);
void controller(int *j);
/******************************************************/
int addd(int x, int y)
{
	cout << "lib success" << (x + y) << endl;
	return (x+y);
}
int MSRSCB(Mat &input, Mat&output1, Mat&output2,int Nscales, int* scales, float* weight, float *Sigma,float Lcutpercent, float Rcutpercent,float *logTbale)
{
	 
	if (input.type()!=CV_8UC3)
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
	if (input.data= NULL)
	{
		cout << "input.data() = NULL" << endl;
		return 4;
	}

	 //cout << "START multi-scale-retinex"<< endl;
	 vector<Mat> rgb{ Mat(input.size(), CV_32F, Scalar(0.0)), Mat(input.size(), CV_32F, Scalar(0.0)), Mat(input.size(), CV_32F, Scalar(0.0)) };
	 split(input, rgb);
	 Mat internsity(input.size(), CV_32F);
	 intensityCalculation(internsity, rgb);
	 vector<Mat> rgbMSR{ Mat(input.size(), CV_32F, Scalar(0.0)), Mat(input.size(), CV_32F, Scalar(0.0)), Mat(input.size(), CV_32F, Scalar(0.0)) };
	 processMSR(rgb, rgbMSR, scales, Nscales, Sigma, weight, logTbale);
	 vector<Mat> rgbMSRCR{ Mat(input.size(), CV_32F, Scalar(0.0)), Mat(input.size(), CV_32F, Scalar(0.0)), Mat(input.size(), CV_32F, Scalar(0.0)) };
	 Color_Restoration(rgbMSR, rgbMSRCR, rgb, internsity, logTbale);
	 vector<Mat> rgbMSRCRSCB{ Mat(input.size(), CV_32F, Scalar(0.0)), Mat(input.size(), CV_32F, Scalar(0.0)), Mat(input.size(), CV_32F, Scalar(0.0)) };
	 simplest_color_balance(rgbMSRCR, rgbMSRCRSCB, Lcutpercent,Rcutpercent);
	 Mat ResultImg8(input.size(), CV_32FC3, Scalar(0, 0, 0));
	 merge(rgbMSRCRSCB, ResultImg8);
	 ResultImg8.convertTo(output1, CV_8UC3);
	 cout << "MSRCRSCB DONE"<< endl;

	 vector<Mat> rgbMSRSCB{ Mat(input.size(), CV_32F, Scalar(0.0)), Mat(input.size(), CV_32F, Scalar(0.0)), Mat(input.size(), CV_32F, Scalar(0.0)) };
	 simplest_color_balance(rgbMSR, rgbMSRSCB, Lcutpercent, Rcutpercent);
	 Mat resultMSRSCB(input.size(), CV_32FC3, Scalar(0, 0, 0));
	 merge(rgbMSRSCB, resultMSRSCB);
	 resultMSRSCB.convertTo(output2, CV_8UC3);
	 cout << "MSR SCB DONE" << endl;
 

	return 0;
}
int main0()
{

	logLUTinit();
	vector<string> files;
	getJustCurrentFile("./data/", files);
	int nscales=1;
	int scales[3] = {251,251,311};
	float Sigma[3] = {0};
	float cutPercL =1.5,cutPercR = 1.5;
	float weight[3] = { 1.0, 1.0,0.0 }; 

	for (int j = 0; j <files.size(); ++j)
	{

		cout << "START retinex" <<  "PicNum:" << j << "   Name:" << files[j] << endl;
		time_t start_t = clock();
		Mat origin = imread(files[j].c_str(), IMREAD_COLOR);
		Mat out1(origin.size(), CV_8UC3, Scalar(0));		Mat out2(origin.size(), CV_8UC3, Scalar(0));
		MSRSCB(origin, out1, out2, nscales, scales, weight, Sigma, cutPercL, cutPercR, logLUT);
		imshow("out1",out1);
		imshow("out2",out2);
		waitKey();
		////////////////////////////////////////////////////////////////
		imshow("origin", origin);
		vector<Mat> rgb{ Mat(origin.size(), CV_32F, Scalar(0.0)), Mat(origin.size(), CV_32F, Scalar(0.0)), Mat(origin.size(), CV_32F, Scalar(0.0)) };
		split(origin, rgb);
		Mat internsity(origin.size(),CV_32F);
		intensityCalculation(internsity, rgb);	
		vector<Mat> rgbMSR{ Mat(origin.size(), CV_32F, Scalar(0.0)), Mat(origin.size(), CV_32F, Scalar(0.0)), Mat(origin.size(), CV_32F, Scalar(0.0)) };
		time_t Node_1 = clock();
		cout << "Node1 Load&Split counts " <<(float)(Node_1 - start_t) / CLOCKS_PER_SEC << "secs" << endl;
		 
		processMSR(rgb, rgbMSR, scales, nscales, Sigma, weight, logLUT);
		time_t Node_2 = clock();
		cout << "Node2 GaussianBlur count " << (float)(Node_2 - Node_1) / CLOCKS_PER_SEC << "secs" << endl;
		//***********************MSR CR SCB方法**********************//
		vector<Mat> rgbMSRCR{ Mat(origin.size(), CV_32F), Mat(origin.size(), CV_32F), Mat(origin.size(), CV_32F) };
		Color_Restoration(rgbMSR, rgbMSRCR, rgb, internsity, logLUT);

		vector<Mat> rgbMSRCRSCB{ Mat(origin.size(), CV_32F), Mat(origin.size(), CV_32F), Mat(origin.size(), CV_32F) };
		simplest_color_balance(rgbMSRCR,rgbMSRCRSCB,  cutPercL+0.5, cutPercR+0.5);
		Mat ResultImg8(origin.size(), CV_32FC3, Scalar(0,0,0));
		merge(rgbMSRCRSCB, ResultImg8);
		ResultImg8.convertTo(ResultImg8, CV_8UC3);
		imshow("enhanceByMSRCRSCB", ResultImg8);
		time_t Node_3 = clock();
		cout << "Node3 MSR CR SCB count " <<(float)(Node_3 - Node_2) / CLOCKS_PER_SEC << "secs" << endl;
		//***********************MSR SCB CP **********************//
		vector<Mat> rgbMSRSCB{ Mat(origin.size(), CV_32F), Mat(origin.size(), CV_32F), Mat(origin.size(), CV_32F) };
		simplest_color_balance( rgbMSR, rgbMSRSCB,cutPercL, cutPercR);
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


		 cout<< "FINISH Retinex done ,count:" << (float)(clock() - start_t) / CLOCKS_PER_SEC << "secs %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
		controller(&j);

	}
	return 0;
}

void chromaPreserve(vector<Mat>inMSRSCB, vector<Mat>inMSRCPSCB, vector<Mat> &origin, Mat& inten, Mat& inten1)
{
	float temp1,temp2;
	float A,B = 0 ;
	for (int i = 0; i < origin[0].rows; i++)
		for (int j = 0; j < origin[0].cols; j++)
		{
			//B = myMax3(origin[0].at<float>(i, j), origin[1].at<float>(i, j), origin[2].at<float>(i, j));
			//temp1 = 255.0/(B+1);
			temp2 = inten1.at<float>(i, j) /( inten.at<float>(i, j)+1);
		    //if (temp1<temp2)
			//	A = temp1;
			//else
				A = temp2;
			inMSRCPSCB[0].at<float>(i, j) = A*origin[0].at<float>(i, j);
			inMSRCPSCB[1].at<float>(i, j) = A*origin[1].at<float>(i, j);
			inMSRCPSCB[2].at<float>(i, j) = A*origin[2].at<float>(i, j);
		}
}

int simplest_color_balance(vector<Mat>&  data,vector<Mat>& data_out,  float cutPercentS, float cutPercentL)
{

	int image_size = data[0].rows*data[0].cols;
	float *sortdata = (float *)malloc(image_size*sizeof(float));
	
	for (int k = 0; k < 3; k++)
	{	
	
		for (int n1 = 0; n1 < data[0].rows; n1++)
		{
			for (int n2 = 0; n2 < data[0].cols; n2++)
			{
				*(sortdata + n1*data[0].cols + n2) = data[k].at<float>(n1, n2);
			}
		}
	    time_t iNode = clock();
		qsort(sortdata, image_size, sizeof(sortdata[0]), &floatComparison);
		cout << "   iNode:"<<k<<" qsort count " << (float)(clock() - iNode) / CLOCKS_PER_SEC << "secs" << endl;
		
		
		int per1 = (int)(cutPercentS*image_size / 100);
		float min = sortdata[per1];

		int per2 = (int)(cutPercentL*image_size / 100);
		float max = sortdata[image_size - 1 - per2];

		if (max <= min)
		{
			cout << "ERROR :simplest_color_balance function: max<min" << endl;
			return  1;
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
	return 0;



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
		r  = rgbChannel[0].ptr<uchar>(i);
		g = rgbChannel[1].ptr<uchar>(i);
		b = rgbChannel[2].ptr<uchar>(i);
		for (j = 0; j < nCols; ++j)
		{
			p[j] = (float)(r[j] + g[j] + b[j]) / 3;
		}
	}
}

void Color_Restoration(vector<Mat>& inputMSR, vector<Mat>& outMSRCR, vector<Mat>& inputORI, Mat &intensity,float* logTable)
{
	float alpha =125.0;	int belta =46;
	float A;
	float Gain = 192;
	float b = -30;
	for (int m = 0; m < 3; m++)
	{
		for (int j = 0; j < inputMSR[m].rows; j++)
		{
			for (int i = 0; i < inputMSR[m].cols; i++)
			{ //以下实现公式（7）
				int index = (intensity.at<float>(j, i))*3;
				A = logTable[index];
				int indexi = alpha* inputORI[m].at<uchar>(j, i);
				outMSRCR[m].at<float>(j, i) = inputMSR[m].at<float>(j, i) * (logTable[indexi] - A);
				outMSRCR[m].at<float>(j, i)=Gain*outMSRCR[m].at<float>(j, i) + b;
			} 
		}
	}
}


void processMSR(vector<Mat>& inputArry, vector<Mat>& outAarry, int *scale, int nscales, float *sigma,float *w,float*logTable)
{
	  // float w = (float)1/nscales;
	   vector<Mat> pasArry{ Mat(inputArry[0].size(), CV_8UC1, Scalar(0)), Mat(inputArry[0].size(), CV_8UC1, Scalar(0)), Mat(inputArry[0].size(), CV_8UC1, Scalar(0))};/* Compute Retinex output*/
	   for (size_t k = 0; k < nscales; ++k)
	   {
		   GaussianBlur(inputArry[0], pasArry[0], Size(scale[k], scale[k]), sigma[k], sigma[k]);
		   GaussianBlur(inputArry[1], pasArry[1], Size(scale[k], scale[k]), sigma[k], sigma[k]);
		   GaussianBlur(inputArry[2], pasArry[2], Size(scale[k], scale[k]), sigma[k], sigma[k]);
		   imshow("inputArry", inputArry[0]);	
		   imshow("pasArry", pasArry[0]);
		   float weight = *(w+k);
		   for (size_t i = 0; i < inputArry[0].rows; ++i)
		   {
			   for (size_t j = 0; j < inputArry[0].cols; ++j)
			   { 
				   outAarry[0].at<float>(i, j) += weight*(logTable[inputArry[0].at<uchar>(i, j)] - logTable[pasArry[0].at<uchar>(i, j)]);//公式（6）
				   outAarry[1].at<float>(i, j) += weight*(logTable[inputArry[1].at<uchar>(i, j)] - logTable[pasArry[1].at<uchar>(i, j)]);
				   outAarry[2].at<float>(i, j) += weight*(logTable[inputArry[2].at<uchar>(i, j)] - logTable[pasArry[2].at<uchar>(i, j)]);
			   }
		   }
		   imshow("rgbMSR", outAarry[0]);
		   cout << "   GaussianING : " << scale[k] << endl;
		   // waitKey();
	   }
}
//void convolution(Mat& inputArry, int scale, Mat& outputArry)
//{
//
//	Mat Out(inputArry.size(),CV_8UC1,Scalar(0));
//	int nx = inputArry.cols;
//	int ny = inputArry.rows;
// 
// 
////*************************//
//	double* input = (double *)malloc(nx*ny*sizeof(double));
//	double* output = (double *)malloc(nx*ny*sizeof(double));
//	int nCols = inputArry.cols;
//	int nRows = inputArry.rows;
//
//	if (inputArry.isContinuous())
//	{
//		cout << "convolution Input function's calculation is continuous" << endl;
//		nCols *= nRows;
//		nRows = 1;
//	} 
//	uchar* data;
//	for (size_t i = 0; i < nRows; ++i)
//	{
//		data = inputArry.ptr<uchar>(i);
//		for (size_t j = 0; j < nCols; ++j)
//		{
//			input[i*nRows + j] = (double)data[j];
//		}
//	}
//
//	double *out;
//	fftw_plan p;
//	int image_size, image_size4;
//	int i, j, index;
//	double sigma, normx, normy;
//
//	out = (double*)fftw_malloc(sizeof(double) * (nx*ny));
//
//	/*compute the Fourier transform of the input data*/
//
//	p = fftw_plan_r2r_2d((int)ny, (int)nx, input, out, FFTW_REDFT10,FFTW_REDFT10, FFTW_ESTIMATE);
//	fftw_execute(p);
//	fftw_destroy_plan(p);
//
//	/*define the gaussian constants for the convolution*/
//
//	sigma = scale*scale / 2.;
//	normx = M_PI / (float)nx;
//	normy = M_PI / (float)ny;
//	normx *= normx;
//	normy *= normy;
//
//	image_size = (int)nx * (int)ny;
//	image_size4 = 4 * image_size;
//
//	for (j = 0; j < (int)ny; j++)
//	{
//		index = j*(int)nx;
//		for (i = 0; i < (int)nx; i++)
//			out[i + index] *= exp((double)(-sigma)*(normx*i*i + normy*j*j));
//	}
//
//	/*compute the Inverse Fourier transform */
//
//	p = fftw_plan_r2r_2d((int)ny, (int)nx, out, output, FFTW_REDFT01,FFTW_REDFT01, FFTW_ESTIMATE);
//	fftw_execute(p);
//
//	for (index = 0; index < image_size; index++)
//		output[index] /= image_size4;
//
//	fftw_destroy_plan(p);
//	fftw_free(out);
//
//	int ncols = outputArry.cols;
//	int nrows = outputArry.rows;
//	if (outputArry.isContinuous())
//	{
//		cout << "convolution output function's calculation is continuous" << endl;
//		ncols *= nrows;
//		nrows = 1;
//	}
// 
//	uchar* Data;
//	for (size_t  m = 0; m < nRows; ++m)
//	{
//		Data = outputArry.ptr<uchar>(m);
//		for (size_t t = 0; t< nCols; ++t)
//		{
//			Data[t] = (uchar)output[i*nRows + j];
//		}
//	}
//	free(input);
//	free(output);
//}

void getJustCurrentFile(string path, vector<string>& files)
{
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p;
	if((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
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

void Mat2Arry(Mat &mata,uchar*data)
{
	int nCols = mata.cols;
	int nRows = mata.rows;
	if ( mata.isContinuous() )
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

void Arry2Mat(uchar *data,Mat &mata)
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
			p[j]=data[i*nRows + j];
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
	for ( long int  i = 0; i < LOG_LUT_LENGTH; i++)
	{
		logLUT[i] = log((float)i+1);
	}
  
}