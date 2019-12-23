#pragma once

struct AVFrame;
struct AVPacket;
class AVCodecContext;
///����Ƶ����ӿ���
class XMediaEncode
{
public:

	///�������
	int inWidth = 640;
	int inHeight = 480;
	int inPixSize = 3;

	///�������
	int outWidth = 640;
	int outHeight = 480;
	int bitrate = 4000000;//ѹ����ÿ����Ƶ��bitλ��С 50kB
	int fps = 29;

	//������������
	static XMediaEncode * Get(unsigned char index = 0);

	//��ʼ�����ظ�ʽת���������ĳ�ʼ��
	virtual bool InitScale() = 0;

	virtual AVFrame* RGBToYUV(char *rgb) = 0;

	//��Ƶ��������ʼ��
	virtual bool InitVideoCodec() = 0;

	//��Ƶ����
	virtual AVPacket * EncodeVideo(AVFrame* frame) = 0;

	virtual ~XMediaEncode();

	AVCodecContext *vc = 0;	//������������
protected:
	XMediaEncode();
};

