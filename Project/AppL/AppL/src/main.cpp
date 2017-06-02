#include <iostream>
#include<io.h>
#include <conio.h> 
#include "dehazing.h"
#include "cv.h"
#include "highgui.h"
using namespace std;
using namespace cv;

struct bright
{
};


int main0()
{
	// VideoCapture打开视频流方法2：通过默认构造函数创建对象，然后通过open方法打开视频流
 
	VideoCapture capFromFile(".\\video\\hazeroad.avi");

		int keyBoard = waitKey();
		int i= 0;
		switch (keyBoard)
		{
		case 2424832: if (i > 1) i--; break;
		case 2555904: if (i < (i - 1)) i++; break;
		default: i++; break;
		}

	
	return 0;
}
