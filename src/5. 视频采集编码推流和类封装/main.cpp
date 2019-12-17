#include "opencv2/highgui.hpp"
#include <iostream>
#pragma comment(lib,"opencv_world320d.lib")
using namespace std;
using namespace cv;
int main(int argc, char *argv[])
{
	//海康相机的rtsp url
	//char *inUrl = "rtsp://test:test123456@192.168.1.64";
	VideoCapture cam;
	namedWindow("video");

	//if(cam.open(inUrl))
	if (cam.open(0))
	{
		cout << "open camera success!" << endl;
	}
	else
	{
		cout << "open camera failed!" << endl;

		waitKey(1);
		return -1;
	}
	Mat frame;
	for (;;)
	{
		cam.read(frame);
		imshow("video", frame);
		waitKey(1);
	}

	return 0;
}