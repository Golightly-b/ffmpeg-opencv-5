#include "XMediaEncode.h"
#include <iostream>
extern "C" {
#include <libswscale/swscale.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib, "swscale.lib")

using namespace std;

class CXMediaEncode :public XMediaEncode
{
public:
	void Close()
	{
		if (vsc)
		{
			sws_freeContext(vsc);
			vsc = NULL;
		}
		if (yuv)
		{
			av_frame_free(&yuv);
		}
	}
	
	bool InitScale()
	{		
		///2 ��ʼ����ʽת��������
		vsc = sws_getCachedContext(vsc,
			inWidth, inHeight, AV_PIX_FMT_BGR24,	 //Դ���ߡ����ظ�ʽ
			inWidth, inHeight, AV_PIX_FMT_YUV420P,//Ŀ����ߡ����ظ�ʽ
			SWS_BICUBIC,  // �ߴ�仯ʹ���㷨
			0, 0, 0);
		if (!vsc) {
			throw exception("sws_getCachedContext failed!");
			return false;
		}
		
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


			return true;
	}

	AVFrame *RGBtoYUV(char *rgb)
	{
		//rgb to yuv
		//��������ݽṹ     ������bgr
		uint8_t *indata[AV_NUM_DATA_POINTERS] = { 0 };
		indata[0] = (uint8_t *)rgb;
		int insize[AV_NUM_DATA_POINTERS] = { 0 };//һ�����ݣ������ֽ���
		insize[0] = inWidth*inPixSize;
		
		int h = sws_scale(vsc, indata, insize, 0, inHeight,
			yuv->data, yuv->linesize);
		if (h <= 0) {
			return NULL;
		}	
		return yuv;
	}
private:
	SwsContext *vsc = NULL;//���ظ�ʽת��������
	AVFrame *yuv = NULL;//��������ݹ���
};


XMediaEncode * XMediaEncode::Get(unsigned char index)
{
	static bool isFirst = true;
	if (isFirst)
	{
		avcodec_register_all();//ע�����еı������
		isFirst = false;
	}
	static CXMediaEncode cxm[255];
	return &cxm[index];
}

XMediaEncode::XMediaEncode()
{
}


XMediaEncode::~XMediaEncode()
{
}
