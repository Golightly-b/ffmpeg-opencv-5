#pragma once
struct AVFrame;
///����Ƶ����ӿ���
class XMediaEncode
{
public:
	int inWidth = 640;
	int inHeight = 480;
	int inPixSize = 3;

	int outWidth = 640;
	int outHeight = 480;


	
	//������������
	static XMediaEncode * Get(unsigned char index = 0);
	//��ʼ�����ظ�ʽת����������
	virtual bool InitScale() = 0;

	virtual AVFrame* RGBtoYUV(char *rgb) = 0;

	virtual ~XMediaEncode();

protected:
	XMediaEncode();
};

