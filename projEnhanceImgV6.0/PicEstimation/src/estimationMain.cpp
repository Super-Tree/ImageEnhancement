#include "estimationMain.h"
void GetCurrentFile(string path, vector<string>& files);
int main1()
{
	bright brightValue = { 0 };
	vector<string> files;
	GetCurrentFile("./data/", files);
	int  times = files.size();
	for (int i = 0; i < times;)
	{
		Mat origin = imread(files[i]);
		if (origin.empty())
		{
			cout << "Read picture failed!" << endl;
			waitKey();
			return -3;
		}
		Mat gray_img;
	
		cvtColor(origin, gray_img, CV_RGB2GRAY);
		int sharpness = sharpnessValue(gray_img);
		//blur(gray_img, gray_img, Size(17,17), Point(-1, -1));
		medianBlur(gray_img, gray_img, 17);//中值滤波
		Mat img_hsi;
		cvtColor(origin, img_hsi, CV_RGB2HSV);
		vector<Mat> mv;
		split(img_hsi, mv);
		Mat IValue_img = mv[2];
		/***********************************************/
		const int channels[1] = { 0 };
		const int histSize[1] = { 256 };
		float hranges[2] = { 0, 255 };
		const float* ranges[1] = { hranges };
		MatND histResult;
		/*******************制作增益遮罩*************************/
		//Mat histMask  = imread("./data/zHistMask.jpg");
		//Mat gainMask = imread("./data/zGainMask.jpg");
		float a = 1.0;
		float b = (1 - a);
		//addWeighted(origin, a, histMask, b, 0.0, dst1);	
		//addWeighted(dst1, a, gainMask, b, 0.0, dst);
		//cvtColor(histMask, histMask, CV_RGB2GRAY);
		//cvtColor(gainMask, gainMask, CV_RGB2GRAY);
		//threshold(histMask, histMask, 170, 255, CV_THRESH_BINARY);
		//threshold(gainMask, gainMask, 170, 255, CV_THRESH_BINARY);
		/******************统计直方图*************************/
		calcHist(&IValue_img, 1, channels, Mat(), histResult, 1, histSize, ranges);

		float perc = brightnessValue(histResult, brightValue);
		//	plotHist("Mask", histResult);
		string result = "Delta:" + format("%.2f", perc) + "%";
		string num = "DMP: " + format("%d", brightValue.DMP) + " MBL: " + format("%d", brightValue.MBL);
		string sharp = "Sharpness:" + format("%d", sharpness);
		putText(origin, result, Point(20, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1, 4);
		putText(origin, num, Point(20, 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1, 4);
		putText(origin, sharp, Point(20, 60), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 1, 4);

		imshow("result", origin);
		string name = "./result/resultPic" + format("%d", i) + ".jpg";
		imwrite(name, origin);
		int keyBoard = waitKey();
		switch (keyBoard)
		{
		case 2424832: if (i > 1) i--; break;
		case 2555904: if (i < (times - 1)) i++; break;
		default: i++; break;
		}

	}
	return 0;
}

int estimation(Mat & origin,string name)
{
	bright brightValue = { 0 };
	Mat gray_img;
	cvtColor(origin, gray_img, CV_RGB2GRAY);
	medianBlur(gray_img, gray_img, 17);//
	Mat img_hsi;
	cvtColor(origin, img_hsi, CV_RGB2HSV);
	vector<Mat> mv;
	split(img_hsi, mv);
	Mat IValue_img = mv[2];
	const int channels[1] = { 0 };
	const int histSize[1] = { 256 };
	float hranges[2] = { 0, 255 };
	const float* ranges[1] = { hranges };
	MatND histResult;

	calcHist(&IValue_img, 1, channels, Mat(), histResult, 1, histSize, ranges);
	float perc = brightnessValue(histResult, brightValue);
	string result = "Delta:" + format("%.2f", perc) + "%";
	string num = "DMP: " + format("%d", brightValue.DMP) + " MBL: " + format("%d", brightValue.MBL);
	putText(origin, result, Point(20, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1, 4);
	putText(origin, num, Point(20, 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1, 4);
	imshow(name, origin);
	if ( brightValue.MBL < 130)
	{
		if (brightValue.DMP<-4 && brightValue.DMP>-18)
		{
			return 1;
		}
	}	
	return 0;
}

Mat plotHist(string name, const MatND& hist_)
{
	double maxVal = 0;
	double minVal = 0;

	//找到直方图中的最大值和最小值
	//minMaxLoc(hist_, &minVal, &maxVal, 0, 0);
	//if (maxVal < 10000)
	//{
	//	maxVal = 10000;
	//}
	//else if (maxVal < 15000)
	//{
	//	maxVal = 15000;
	//	//}
	//	else {
	//	cout << "Hist out of range" << endl;
	//}
	int flag = 0;
	maxVal = 10000;
	int histSize = hist_.rows;
	Mat histImg(histSize, histSize, CV_8U, Scalar(255));
	// 设置最大峰值为图像高度的90%
	int hpt = static_cast<int>(0.9*histSize);

	for (int h = 0; h < histSize; h++)
	{
		float binVal = hist_.at<float>(h);
		if (binVal > maxVal)
		{
			flag = 1;
			binVal = maxVal;
		}
		int intensity = static_cast<int>(binVal*hpt / maxVal);
		line(histImg, Point(h, histSize), Point(h, histSize - intensity), Scalar::all(0));
		imshow(name, histImg);
	}
	if (flag)
		cout << "Hist out of range" << endl;
	return histImg;
}

float brightnessValue(MatND& histMask, bright& brightness)
{
	int r1 = 0, r2 = 0;

	int numPoint = 0;
	for (size_t i = 0; i < histMask.rows; i++)
	{
		brightness.MBL += histMask.at<float>(i) * i;
		numPoint += histMask.at<float>(i);
	}
	brightness.MBL /= numPoint;

	if (brightness.MBL > histMask.rows)
	{
		cout << "MBL is out of range!" << "NUM: " << brightness.MBL << endl;
		return -1;
	}

	int temp = 0;
	for (size_t i = brightness.MBL; i < histMask.rows; i++)
	{
		brightness.MBLh += histMask.at<float>(i)* i;
		temp += histMask.at<float>(i);
	}
	if (temp == 0)
	{
		brightness.MBLh = 0;
	}
	else
	{
		brightness.MBLh /= temp;
	}

	int temp1 = 0;
	for (size_t i = 0; i < brightness.MBL; i++)
	{
		brightness.MBLl += histMask.at<float>(i) * i;
		temp1 += histMask.at<float>(i);
	}
	if (temp1 == 0)
	{
		brightness.MBLl = 0;
	}
	else
	{
		brightness.MBLl /= temp1;
	}

	brightness.DMP = brightness.MBL - (brightness.MBLl + brightness.MBLh) / 2;
	float percent = (float)brightness.DMP * 100 / brightness.MBL;


	return percent;

}

int sharpnessValue(Mat & image)
{
	int sum = 0;
	int data = 0;
	if (image.isContinuous())
	{
		cout << "Image is continuous!" << endl;
		for (int i = 0; i < (image.rows - 1); i++)
		{
			const  uchar* inData = image.ptr<uchar>(i);
			for (int j = 0; j < (image.cols - 1); j++)
			{
				data = (*(inData + 1) - *inData)*(*(inData + image.cols) - *inData);
				sum += data;
				inData++;
			}

		}
	}
	else
	{
		cout << "Image is not continuous!" << endl;
		for (int i = 0; i < (image.rows - 1); i++)
		{
			for (int j = 0; j < (image.cols - 1); j++)
			{
				data = (image.at<uchar>(i, j) - image.at<uchar>(i + 1, j))*(image.at<uchar>(i, j) - image.at<uchar>(i, j + 1));
				sum += data;
			}

		}

	}
	sum /= (float)(image.rows*image.cols);
	return sum;
}

void GetCurrentFile(string path, vector<string>& files)
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

int zoneBorderInit(Point p1, Point p2, int* array)
{
	if (p1.x > p2.x)
	{//保证p1，p2的顺序
		Point temp = p2;
		p2 = p1;
		p1 = temp;
	}

	float deltaX1 = p1.x - 0.0;
	float deltaY1 = p1.y - 480.0;
	float slope1 = deltaY1 / deltaX1;
	int b1 = 480;

	float deltaX2 = p2.x - 640.0;
	float deltaY2 = p2.y - 480.0;
	float slope2 = deltaY2 / deltaX2;
	int b2 = 480 - slope2 * 640;

	for (int i = 0; i < 640; i++)
	{
		if (i < p1.x)
		{
			*(array + i) = (float)slope1*i + b1;
			continue;
		}

		if (i > p2.x)
		{
			*(array + i) = (float)slope2*i + b2;
			continue;
		}

		*(array + i) = (p1.y + p2.y) / 2;
	}

	return 0;

}

int MaskInit(Point p1, Point p2, int* array)
{
	if (p1.x > p2.x)
	{//保证p1，p2的顺序
		Point temp = p2;
		p2 = p1;
		p1 = temp;
	}

	float deltaX1 = p1.x - 0.0;
	float deltaY1 = p1.y - 0.0;
	float slope1 = deltaY1 / deltaX1;
	int b1 = 0;

	float deltaX2 = p2.x - 640.0;
	float deltaY2 = p2.y - 0.0;
	float slope2 = deltaY2 / deltaX2;
	int b2 = -slope2 * 640;

	for (int i = 0; i < 640; i++)
	{
		if (i < p1.x)
		{
			*(array + i) = (float)slope1*i + b1;
			continue;
		}

		if (i > p2.x)
		{
			*(array + i) = (float)slope2*i + b2;
			continue;
		}

		*(array + i) = (p1.y + p2.y) / 2;
	}

	return 0;

}