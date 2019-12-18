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
	//char *inUrl = "rtsp://test:test123456@192.168.1.64";//���������rtsp url
	char *outnUrl = "rtmp://192.168.1.6/live";//ֱ����������nginx-rtmp����rtmp
	VideoCapture cam;
	Mat frame;
	namedWindow("video");
	SwsContext *vsc = NULL;//���ظ�ʽת��������
	AVFrame *yuv = NULL;//��������ݽṹ
	AVCodecContext *vc = NULL;//������������
	
	avcodec_register_all();//ע�����еı������

	////////////////////////////////////////////////////////////////
	try
	{		
		/// 1 ʹ��opencv��rtsp���
		cam.open(0);
		if (!cam.isOpened()) throw exception("cam open failed");
		cout << "cam open succeed" << endl;		
		int inWidth = cam.get(CAP_PROP_FRAME_WIDTH);
		int inHeight = cam.get(CAP_PROP_FRAME_HEIGHT);
		int fps = 29;//cam.get(CAP_PROP_FPS)
		
		///2 ��ʼ����ʽת��������
		vsc = sws_getCachedContext(vsc,
			inWidth, inHeight, AV_PIX_FMT_BGR24,	 //Դ���ߡ����ظ�ʽ
			inWidth, inHeight, AV_PIX_FMT_YUV420P,//Ŀ����ߡ����ظ�ʽ
			SWS_BICUBIC,  // �ߴ�仯ʹ���㷨
			0, 0, 0);
		if (!vsc) throw exception("sws_getCachedContext failed!");

		///3 ��ʼ����������ݽṹ
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

		///4 ��ʼ������������
		//a �ҵ�������
		AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
		if (!codec)	throw exception("Can`t find h264 encoder!");
		
		//b ����������������
		vc = avcodec_alloc_context3(codec);
		if (!vc)	throw exception("avcodec_alloc_context3 failed!");
		
		//c ���ñ���������
		vc->flags |= AV_CODEC_FLAG_GLOBAL_HEADER; //ȫ�ֲ���
		vc->codec_id = codec->id;
		vc->thread_count = 8;
		vc->bit_rate = 50 * 1024 * 8;//ѹ����ÿ����Ƶ��bitλ��С 50kB
		vc->width = inWidth;
		vc->height = inHeight;
		vc->time_base = { 1,fps };
		vc->framerate = { fps,1 };
		//������Ĵ�С������֡һ���ؼ�֡
		vc->gop_size = 50;
		vc->max_b_frames = 0;
		vc->pix_fmt = AV_PIX_FMT_YUV420P;
		
		//d �򿪱�����������
		ret = avcodec_open2(vc, 0, 0);
		if (ret != 0)
		{
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			throw exception(buf);
		}
		cout << "avcodec_open2 success!" << endl;

		AVPacket pack;
		memset(&pack, 0, sizeof(pack));
		int vpts = 0;

		for (;;)
		{
			///��ȡ��Ƶ֡��������Ƶ֡
			if (!cam.grab()) continue;
			///yuvת��Ϊrgb
			if (!cam.retrieve(frame)) continue;
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
			//cout << h << "" << flush;

			///h264����
			yuv->pts = vpts;
			vpts++;
			ret = avcodec_send_frame(vc, yuv);
			if (ret != 0)continue;
			ret = avcodec_receive_packet(vc, &pack);
			 cout << "*" << pack.size << flush;
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
		if (vc)
		{
			avcodec_free_context(&vc);
		}
		cerr << ex.what() << endl;
	}
	getchar();
	return 0;
}