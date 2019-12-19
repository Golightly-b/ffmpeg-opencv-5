#pragma once
struct AVFrame;
///音视频编码接口类
class XMediaEncode
{
public:
	int inWidth = 640;
	int inHeight = 480;
	int inPixSize = 3;

	int outWidth = 640;
	int outHeight = 480;


	
	//工厂生产方法
	static XMediaEncode * Get(unsigned char index = 0);
	//初始化像素格式转换的上下文
	virtual bool InitScale() = 0;

	virtual AVFrame* RGBtoYUV(char *rgb) = 0;

	virtual ~XMediaEncode();

protected:
	XMediaEncode();
};

