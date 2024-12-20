#include "DriverFPC1020AM.h"
#include <iostream>
#include <filesystem>

BYTE g_FpImageBuf[300 * 300]; // max size : 300*300
int g_nImageWidth;
int g_nImageHeight;

int SaveBMP8(
	const char *filename,
	int p_nWidth,
	int p_nHeight,
	unsigned char *p_pbImageData);

std::wstring CreateSharedMemoryName(int deviceID)
{
	std::wstringstream wss;
	wss << L"FPC1020AM_SHARED_MEMORY_" << deviceID;
	return wss.str();
}

DriverFPC1020AM::DriverFPC1020AM(int deviceID) : m_nConDeviceID(deviceID),
												 m_sharedMem(CreateSharedMemoryName(deviceID).c_str())
{
}

ConnectResult DriverFPC1020AM::Connect()
{
	// Init connection.
	if (m_clsCommu.InitConnection(
			USB_CON_MODE,
			"",
			-1,
			0,
			m_nConDeviceID) != CONNECTION_SUCCESS)
	{
		return ConnectResult::FAILED;
	}

	// Test connection with device.
	if (m_clsCommu.Run_TestConnection() != ERR_SUCCESS)
	{
		return ConnectResult::FAILED;
	}

	return ConnectResult::SUCCESS;
}

void DriverFPC1020AM::RunCapture()
{
	m_clsCommu.Run_SLEDControl(1);

	int w_nRet;
	int n = 0;
	static std::string userName;
	static std::string fingerNumber;
	static std::string basePath;
	std::cout << "Please input user name: ";
	std::cin >> userName;
	std::cout << "Please input finger number (0-9): ";
	std::cin >> fingerNumber;
	std::cout << "Use default path (D:\\Desktop\\毕设\\code\\fp-seq-stitch\\datasets\\FPC1020\\)? (Y/N): ";
	std::string useDefaultPath;
	std::cin >> useDefaultPath;
	if (useDefaultPath == "N" || useDefaultPath == "n")
	{
		std::cout << "Please input base path: ";
		std::cin >> basePath;
	}
	else
	{
		basePath = "D:\\Desktop\\毕设\\code\\fp-seq-stitch\\datasets\\FPC1020\\" + userName + "\\" + fingerNumber;
	}

	char fname[100];
	std::filesystem::create_directories(basePath);
	while (1)
	{
		w_nRet = m_clsCommu.Run_GetImage();
		if (w_nRet == ERR_SUCCESS)
		{
			m_clsCommu.Run_UpImage(0, g_FpImageBuf, &g_nImageWidth, &g_nImageHeight);
			m_sharedMem.SendMat(g_FpImageBuf, IMAGE_PRE_WIDTH, IMAGE_PRE_HEIGHT, 0);
			sprintf_s(fname, sizeof(fname), "%s\\%d.bmp", basePath.c_str(), n++);
			if (SaveBMP8(fname, g_nImageWidth, g_nImageHeight, g_FpImageBuf) == 0)
			{
				std::cout << "Save BMP failed." << std::endl;
			}
			std::cout << "New frame " << n << std::endl;
		}
	}

	m_clsCommu.Run_SLEDControl(0);
}

DriverFPC1020AM::~DriverFPC1020AM()
{
}

int SaveBMP8(
	const char *filename,
	int p_nWidth,
	int p_nHeight,
	unsigned char *p_pbImageData)
{
	unsigned char head[1078] = {
		/***************************/
		// file header
		0x42, 0x4d,			   // file type
							   // 0x36,0x6c,0x01,0x00, //file size***
		0x0, 0x0, 0x0, 0x00,   // file size***
		0x00, 0x00,			   // reserved
		0x00, 0x00,			   // reserved
		0x36, 0x4, 0x00, 0x00, // head byte***
		/***************************/
		// infoheader
		0x28, 0x00, 0x00, 0x00, // struct size

		// 0x00,0x01,0x00,0x00,//map width***
		0x00, 0x00, 0x0, 0x00, // map width***
		// 0x68,0x01,0x00,0x00,//map height***
		0x00, 0x00, 0x00, 0x00, // map height***

		0x01, 0x00,				// must be 1
		0x08, 0x00,				// color count***
		0x00, 0x00, 0x00, 0x00, // compression
		// 0x00,0x68,0x01,0x00,//data size***
		0x00, 0x00, 0x00, 0x00, // data size***
		0x00, 0x00, 0x00, 0x00, // dpix
		0x00, 0x00, 0x00, 0x00, // dpiy
		0x00, 0x00, 0x00, 0x00, // color used
		0x00, 0x00, 0x00, 0x00, // color important
	};
	FILE *fh;
	int i, j, iImageStep;
	long num;
	unsigned char *p1;
	unsigned char w_bTemp[4];

	if (p_nWidth & 0x03)
	{
		iImageStep = p_nWidth + (4 - (p_nWidth & 0x03));
	}
	else
	{
		iImageStep = p_nWidth;
	}

	num = p_nWidth;
	head[18] = num & 0xFF;
	num = num >> 8;
	head[19] = num & 0xFF;
	num = num >> 8;
	head[20] = num & 0xFF;
	num = num >> 8;
	head[21] = num & 0xFF;

	num = p_nHeight;
	head[22] = num & 0xFF;
	num = num >> 8;
	head[23] = num & 0xFF;
	num = num >> 8;
	head[24] = num & 0xFF;
	num = num >> 8;
	head[25] = num & 0xFF;

	j = 0;
	for (i = 54; i < 1078; i = i + 4)
	{
		head[i] = head[i + 1] = head[i + 2] = j;
		head[i + 3] = 0;
		j++;
	}

	memset(w_bTemp, 0, sizeof(w_bTemp));

	if (fopen_s(&fh, filename, "wb") != 0)
		return 0;

	fwrite(head, sizeof(char), 1078, fh);

	if (iImageStep == p_nWidth)
	{
		p1 = p_pbImageData + (p_nHeight - 1) * p_nWidth;
		for (i = 0; i < p_nHeight; i++)
		{
			fwrite(p1, 1, p_nWidth, fh);
			p1 -= p_nWidth;
		}
	}
	else
	{
		iImageStep -= p_nWidth;
		p1 = p_pbImageData + (p_nHeight - 1) * p_nWidth;
		for (i = 0; i < p_nHeight; i++)
		{
			fwrite(p1, 1, p_nWidth, fh);
			fwrite(w_bTemp, 1, iImageStep, fh);
			p1 -= p_nWidth;
		}
	}
	fclose(fh);

	return 1;
}
