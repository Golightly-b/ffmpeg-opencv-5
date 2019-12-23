#pragma once

struct AVFrame;
struct AVPacket;
class AVCodecContext;
///音视频编码接口类
class XMediaEncode
{
public:

	///输入参数
	int inWidth = 640;
	int inHeight = 480;
	int inPixSize = 3;

	///输出参数
	int outWidth = 640;
	int outHeight = 480;
	int bitrate = 4000000;//压缩后每秒视频的bit位大小 50kB
	int fps = 29;

	//工厂生产方法
	static XMediaEncode * Get(unsigned char index = 0);

	//初始化像素格式转换的上下文初始化
	virtual bool InitScale() = 0;

	virtual AVFrame* RGBToYUV(char *rgb) = 0;

	//视频编码器初始化
	virtual bool InitVideoCodec() = 0;

	//视频编码
	virtual AVPacket * EncodeVideo(AVFrame* frame) = 0;

	virtual ~XMediaEncode();

	AVCodecContext *vc = 0;	//编码器上下文
protected:
	XMediaEncode();
};

