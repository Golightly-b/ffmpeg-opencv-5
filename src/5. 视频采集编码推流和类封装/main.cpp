#include "opencv2/highgui.hpp"
#include <iostream>
#include "XMediaEncode.h"
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

#pragma comment(lib,"opencv_world320d.lib")
#pragma comment(lib,"avformat.lib")
using namespace std;
using namespace cv;
int main(int argc, char *argv[])
{
	//char *inUrl = "rtsp://test:test123456@192.168.1.64";//���������rtsp url
	char *outnUrl = "rtmp://192.168.1.111/live";//ֱ����������nginx-rtmp����rtmp
	VideoCapture cam;
	Mat frame;
	namedWindow("video");
	
	XMediaEncode *me = XMediaEncode::Get(0);
	
	AVCodecContext *vc = NULL;//������������
	AVFormatContext *ic = NULL;//rtmp flv ��װ��
	
	
	av_register_all();//ע�����еķ�װ��
	avformat_network_init();//ע����������Э��
	////////////////////////////////////////////////////////////////
	int ret = 0;
	try
	{		
		/// 1 ʹ��opencv��rtsp���
		cam.open(0);
		if (!cam.isOpened()) throw exception("cam open failed");
		cout << "cam open succeed" << endl;		
		int inWidth = cam.get(CAP_PROP_FRAME_WIDTH);
		int inHeight = cam.get(CAP_PROP_FRAME_HEIGHT);
		int fps = 29;//cam.get(CAP_PROP_FPS)
		
		///2.��ʼ����ʽת��������
		///3.��ʼ����������ݽṹ
		me->inWidth = inWidth;
		me->inHeight = inHeight;
		me->outHeight = inHeight;
		me->outWidth = inWidth;
		me->InitScale();
		

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

		///5.�����װ������Ƶ������
		//a.������װ��������
		ret = avformat_alloc_output_context2(&ic, 0, "flv", outnUrl);
		if (ret != 0)
		{
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			throw exception(buf);
		}
		//b.�����Ƶ��
		AVStream *vs = avformat_new_stream(ic, NULL);
		if (!vs) throw exception("avformat_new_stream failed");
		vs->codecpar->codec_tag = 0;
		//�ӱ��������Ʋ���
		avcodec_parameters_from_context(vs->codecpar, vc);
		av_dump_format(ic, 0, outnUrl, 1);

		///6.��rtmp���������IO
		ret = avio_open(&ic->pb, outnUrl,AVIO_FLAG_WRITE);
		if (ret != 0)
		{
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			throw exception(buf);
		}
		//д���װͷ
		ret = avformat_write_header(ic, NULL);
		if (ret != 0)
		{
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			throw exception(buf);
		}

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
			me->inPixSize = frame.elemSize();
			AVFrame *yuv = me->RGBtoYUV((char *)frame.data);
			if (!yuv) continue;


			///h264����
			yuv->pts = vpts;
			vpts++;
			ret = avcodec_send_frame(vc, yuv);
			if (ret != 0)continue;
			ret = avcodec_receive_packet(vc, &pack);
			//cout << "*" << pack.size << flush;

			pack.pts = av_rescale_q(pack.pts, vc->time_base, vs->time_base);
			pack.dts = av_rescale_q(pack.dts, vc->time_base, vs->time_base);
			ret = av_interleaved_write_frame(ic, &pack);
			if (ret == 0) cout << "#" << flush;

		}
	}
	catch (exception&ex)
	{
		if (cam.isOpened()) cam.release();
		
		if (vc)
		{
			avio_closep(&ic->pb);
			avcodec_free_context(&vc);
		}
		cerr << ex.what() << endl;
	}
	getchar();
	return 0;
}