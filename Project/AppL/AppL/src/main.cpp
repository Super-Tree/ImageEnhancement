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
	// VideoCapture����Ƶ������2��ͨ��Ĭ�Ϲ��캯����������Ȼ��ͨ��open��������Ƶ��
 
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
