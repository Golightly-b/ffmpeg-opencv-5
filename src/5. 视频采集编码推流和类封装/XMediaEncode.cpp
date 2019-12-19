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
		///2 初始化格式转换上下文
		vsc = sws_getCachedContext(vsc,
			inWidth, inHeight, AV_PIX_FMT_BGR24,	 //源宽、高、像素格式
			inWidth, inHeight, AV_PIX_FMT_YUV420P,//目标宽、高、像素格式
			SWS_BICUBIC,  // 尺寸变化使用算法
			0, 0, 0);
		if (!vsc) {
			throw exception("sws_getCachedContext failed!");
			return false;
		}
		
		///3 初始化输出的数据结构
		yuv = av_frame_alloc();
		yuv->format = AV_PIX_FMT_YUV420P;
		yuv->width = inWidth;
		yuv->height = inHeight;
		yuv->pts = 0;
		//分配yuv空间
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
		//输入的数据结构     交错存放bgr
		uint8_t *indata[AV_NUM_DATA_POINTERS] = { 0 };
		indata[0] = (uint8_t *)rgb;
		int insize[AV_NUM_DATA_POINTERS] = { 0 };//一行数据（宽）的字节数
		insize[0] = inWidth*inPixSize;
		
		int h = sws_scale(vsc, indata, insize, 0, inHeight,
			yuv->data, yuv->linesize);
		if (h <= 0) {
			return NULL;
		}	
		return yuv;
	}
private:
	SwsContext *vsc = NULL;//像素格式转换上下文
	AVFrame *yuv = NULL;//输出的数据构结
};


XMediaEncode * XMediaEncode::Get(unsigned char index)
{
	static bool isFirst = true;
	if (isFirst)
	{
		avcodec_register_all();//注册所有的编解码器
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
