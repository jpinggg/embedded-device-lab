#include "device_version.hpp"

#include <iostream>

int main()
{
    DeviceVersion deviceVersion{"v1.0"};

    std::cout << "Initial version: "
              << deviceVersion.value() << '\n';

    deviceVersion.update("v1.1");

    std::cout << "Updated version: "
              << deviceVersion.value() << '\n';

    return 0;
}
