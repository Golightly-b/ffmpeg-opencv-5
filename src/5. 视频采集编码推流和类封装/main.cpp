#include "opencv2/highgui.hpp"
#include <iostream>
#pragma comment(lib,"opencv_world320.lib")
using namespace std;
using namespace cv;
int main(int argc, char *argv[])
{
	//海康相机的rtsp url
	//char *inUrl = "rtsp://test:test123456@192.168.1.64";
	//直播服务器（nginx-rtmp）的rtmp
	char *outnUrl = "rtmp://192.168.1.6/live";
	VideoCapture cam;
	Mat frame;
	namedWindow("video");
		
	try
	{
		cam.open(0);
		if (!cam.isOpened()) throw exception("cam open failed");
		cout << "cam open succeed" << endl;
		
		for (;;)
		{
			if (!cam.grab()) continue;///读取视频帧，解码视频帧
			if (!cam.retrieve(frame)) continue;///yuv转换为rgb
			imshow("video", frame);
			waitKey(1);
		}
	}
	catch (exception&ex)
	{
		if (cam.isOpened()) cam.release();
		cerr << ex.what() << endl;
	}
	return 0;
}