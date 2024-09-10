#pragma once
#include <Windows.h>

//=================================�����ڴ����ݶ���=================================
struct ImgInfo {
	int width;
	int height;
	int nframe;
};

#define FRAME_NUMBER		1
#define IMAGE_PRE_WIDTH		242
#define IMAGE_PRE_HEIGHT	266
#define	FRAME_SIZE			(IMAGE_PRE_WIDTH * IMAGE_PRE_HEIGHT) * sizeof(BYTE) + sizeof(ImgInfo)
#define MEMORY_SIZE			FRAME_NUMBER * FRAME_SIZE

//=================================�����ڴ���Ϣ����=================================
#define INITSUCCESS      0
#define CREATEMAPFAILED  1
#define MAPVIEWFAILED    2

class SharedMemory {
	public:
		SharedMemory(const LPCWSTR& name);
		void SendMat(void* img, UINT img_width, UINT img_height, char indexAddress);
		void SendStr(const char data[]);
		~SharedMemory();

		int state;
		int nframe = 0;

	private:
		HANDLE hShareMem;
		LPCTSTR pBuf;
};
