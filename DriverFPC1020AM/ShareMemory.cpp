#pragma once 

#include <afxwin.h>
#include <iostream>
#include "ShareMemory.h"

using namespace std;

/*************************************************************************************
FuncName  :SHAREDMEMORY::SHAREDMEMORY()
Desc      :
Input     :None
Output    :None
**************************************************************************************/
SharedMemory::SharedMemory(const LPCWSTR &name) {
	hShareMem = CreateFileMapping(
		INVALID_HANDLE_VALUE,  // use paging file
		NULL,                  //default security
		PAGE_READWRITE,        //read/write access
		0,                     // maximum object size(high-order DWORD)
		MEMORY_SIZE,           //maximum object size(low-order DWORD)
		name);        //name of mapping object

	if (hShareMem) {
		pBuf = (LPTSTR)MapViewOfFile(
			hShareMem,           //handle to map object
			FILE_MAP_ALL_ACCESS, // read/write permission
			0,
			0,
			MEMORY_SIZE);
		//cout << "memory size:" << MEMORY_SIZE << endl;

		if (pBuf == NULL)
		{
			std::cout << "Could not map view of framebuffer file." << GetLastError() << std::endl;
			CloseHandle(hShareMem);
			state = MAPVIEWFAILED;
		}
	}

	else
	{
		std::cout << "Could not create file mapping object." << GetLastError() << std::endl;
		state = CREATEMAPFAILED;
	}
	state = INITSUCCESS;
}

/*************************************************************************************
FuncName  :SHAREDMEMORY::~SHAREDMEMORY()
Desc      :
Input     :None
Output    :None
**************************************************************************************/
SharedMemory::~SharedMemory() {
	std::cout << "unmap shared addr." << std::endl;
	UnmapViewOfFile(pBuf);
	CloseHandle(hShareMem);
}

/*************************************************************************************
FuncName  :void SHAREDMEMORY::SendMat(cv::Mat img, char indexAddress)
Desc      :
Input     :
	Mat img               
	char indexAddress     
Output    :None
**************************************************************************************/
void SharedMemory::SendMat(void* img, UINT img_width, UINT img_height, char indexAddress) {
	nframe = (nframe + 1) % 10000;

	ImgInfo img_head;
	img_head.width = img_width;
	img_head.height = img_height;
	img_head.nframe = nframe;

	memcpy((char*)pBuf + indexAddress, &img_head, sizeof(ImgInfo));
	memcpy((char*)pBuf + indexAddress + sizeof(ImgInfo), img, img_width * img_height);
}

/*************************************************************************************
FuncName  :void SHAREDMEMORY::SendStr(cv::Mat img, char indexAddress)
Desc      :
Input     :
	Mat img
	char indexAddress
Output    :None
**************************************************************************************/
void SharedMemory::SendStr(const char data[]) {
	memcpy((char*)pBuf, data, sizeof(data));
	cout << "write shared mem successful." << endl;
	getchar();
}
