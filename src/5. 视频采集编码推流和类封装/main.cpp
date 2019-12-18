#include "opencv2/highgui.hpp"
#include <iostream>
extern "C"{
#include <libswscale/swscale.h>
#include "libavcodec/avcodec.h"
}
#pragma comment(lib, "swscale.lib")
#pragma comment(lib,"opencv_world320d.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")
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
	AVFrame *yuv = NULL;//��������ݽṹ
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

		//��������ݽṹ
		yuv = av_frame_alloc();
		yuv->format = AV_PIX_FMT_YUV420P;
		yuv->width = inWidth;
		yuv->height = inHeight;
		yuv->pts = 0;
		//����yuv�ռ�
		int ret = av_frame_get_buffer(yuv, 32);
		if (ret != 0) {
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			throw exception(buf);
		}

		for (;;)
		{
			if (!cam.grab()) continue;///��ȡ��Ƶ֡��������Ƶ֡
			if (!cam.retrieve(frame)) continue;///yuvת��Ϊrgb
			imshow("video", frame);
			waitKey(1);
			//rgb to yuv
			//��������ݽṹ     ������bgr
			uint8_t *indata[AV_NUM_DATA_POINTERS] = { 0 };
			indata[0] = frame.data;
			int insize[AV_NUM_DATA_POINTERS] = { 0 };//һ�����ݣ������ֽ���
			insize[0] = frame.cols*frame.elemSize();

			int h = sws_scale(vsc, indata, insize, 0, frame.rows,
				yuv->data, yuv->linesize);
			if (h <= 0) continue;
			cout << h << "" << flush;
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