#include <iostream>
#include "DriverFPC1020AM.h"

int main(int argc, char* argv[]) {
    int deviceID = 1;
    if (argc > 2) {
        std::cerr << "������������." << std::endl;
        std::cerr << "ʹ�÷���: " << argv[0] << " [<deviceID>]" << std::endl;
        return 1;
    } else if (argc == 2) {
        deviceID = std::atoi(argv[1]);
    }

    DriverFPC1020AM driver(deviceID);

    // Try to connect the device.
    ConnectResult connectionStatus = driver.Connect();
    if (connectionStatus == ConnectResult::FAILED) {
        std::cout << "FPC1020AM�豸����ʧ��." << std::endl;
        return 1;
    }
    std::cout << "FPC1020AM�豸���ӳɹ�." << std::endl;

    // Start capturing process after successful connection.
    driver.RunCapture();

    return 0;
}