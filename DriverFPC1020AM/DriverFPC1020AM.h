#pragma once
#include <sstream>
#include "Define.h"
#include "Communication.h"
#include "ShareMemory.h"

enum class ConnectResult {
	SUCCESS,
	FAILED
};

class DriverFPC1020AM
{
public:
	DriverFPC1020AM(int deviceID);
	ConnectResult Connect();
	void RunCapture();
	~DriverFPC1020AM();
	
	int m_nConDeviceID;

private:
	CCommunication m_clsCommu;
	SharedMemory m_sharedMem;
};