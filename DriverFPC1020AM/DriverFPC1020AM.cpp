#include "DriverFPC1020AM.h"
#include <iostream>

BYTE	g_FpImageBuf[300 * 300]; // max size : 300*300
int		g_nImageWidth;
int		g_nImageHeight;

std::wstring CreateSharedMemoryName(int deviceID) {
	std::wstringstream wss;
	wss << L"FPC1020AM_SHARED_MEMORY_" << deviceID;
	return wss.str();
}

DriverFPC1020AM::DriverFPC1020AM(int deviceID) :
	m_nConDeviceID(deviceID),
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

	int	w_nRet;
	while (1)
	{
		w_nRet = m_clsCommu.Run_GetImage();
		if (w_nRet == ERR_SUCCESS) {
			m_clsCommu.Run_UpImage(0, g_FpImageBuf, &g_nImageWidth, &g_nImageHeight);
			m_sharedMem.SendMat(g_FpImageBuf, IMAGE_PRE_WIDTH, IMAGE_PRE_HEIGHT, 0);
			std::cout << "New frame." << std::endl;
		}
	}

	m_clsCommu.Run_SLEDControl(0);
}

DriverFPC1020AM::~DriverFPC1020AM()
{
}
