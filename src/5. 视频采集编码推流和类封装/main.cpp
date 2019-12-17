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
	//���������rtsp url
	//char *inUrl = "rtsp://test:test123456@192.168.1.64";
	//ֱ����������nginx-rtmp����rtmp
	char *outnUrl = "rtmp://192.168.1.6/live";
	VideoCapture cam;
	Mat frame;
	namedWindow("video");
	SwsContext *vsc = NULL;//���ظ�ʽת��������

	try
	{
		////////////////////////////////////////////////////////////////
		/// 1 ʹ��opencv��rtsp���
		cam.open(0);
		if (!cam.isOpened()) throw exception("cam open failed");
		cout << "cam open succeed" << endl;
		///2 ��ʼ����ʽת��������
		int inWidth = cam.get(CAP_PROP_FRAME_WIDTH);
		int inHeight = cam.get(CAP_PROP_FRAME_HEIGHT);
		int fps = cam.get(CAP_PROP_FPS);
		vsc = sws_getCachedContext(vsc,
			inWidth, inHeight, AV_PIX_FMT_BGR24,	 //Դ���ߡ����ظ�ʽ
			inWidth, inHeight, AV_PIX_FMT_YUV420P,//Ŀ����ߡ����ظ�ʽ
			SWS_BICUBIC,  // �ߴ�仯ʹ���㷨
			0, 0, 0);
		if (!vsc) throw exception("sws_getCachedContext failed!");

		for (;;)
		{
			if (!cam.grab()) continue;///��ȡ��Ƶ֡��������Ƶ֡
			if (!cam.retrieve(frame)) continue;///yuvת��Ϊrgb
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