#include "opencv2/highgui.hpp"
#include <iostream>
extern "C"{
#include <libswscale/swscale.h>
}
#pragma comment(lib, "swscale.lib")
#pragma comment(lib,"opencv_world320d.lib")
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
	SwsContext *vsc = NULL;//像素格式转换上下文

	try
	{
		////////////////////////////////////////////////////////////////
		/// 1 使用opencv打开rtsp相机
		cam.open(0);
		if (!cam.isOpened()) throw exception("cam open failed");
		cout << "cam open succeed" << endl;
		///2 初始化格式转换上下文
		int inWidth = cam.get(CAP_PROP_FRAME_WIDTH);
		int inHeight = cam.get(CAP_PROP_FRAME_HEIGHT);
		int fps = cam.get(CAP_PROP_FPS);
		vsc = sws_getCachedContext(vsc,
			inWidth, inHeight, AV_PIX_FMT_BGR24,	 //源宽、高、像素格式
			inWidth, inHeight, AV_PIX_FMT_YUV420P,//目标宽、高、像素格式
			SWS_BICUBIC,  // 尺寸变化使用算法
			0, 0, 0);
		if (!vsc) throw exception("sws_getCachedContext failed!");

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
		if (vsc)
		{
			sws_freeContext(vsc);
			vsc = NULL;
		}
		cerr << ex.what() << endl;
	}
	getchar();
	return 0;
}