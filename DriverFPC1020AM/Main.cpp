#include <iostream>
#include "DriverFPC1020AM.h"

int main(int argc, char* argv[]) {
    int deviceID = 1;
    if (argc > 2) {
        std::cerr << "参数数量过多." << std::endl;
        std::cerr << "使用方法: " << argv[0] << " [<deviceID>]" << std::endl;
        return 1;
    } else if (argc == 2) {
        deviceID = std::atoi(argv[1]);
    }

    DriverFPC1020AM driver(deviceID);

    // Try to connect the device.
    ConnectResult connectionStatus = driver.Connect();
    if (connectionStatus == ConnectResult::FAILED) {
        std::cout << "FPC1020AM设备连接失败." << std::endl;
        return 1;
    }
    std::cout << "FPC1020AM设备连接成功." << std::endl;

    // Start capturing process after successful connection.
    driver.RunCapture();

    return 0;
}