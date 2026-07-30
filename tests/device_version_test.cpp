#include "device_version.hpp"

#include <iostream>

int main()
{
    DeviceVersion deviceVersion{"v1.0"};

    if (deviceVersion.value() != "v1.0")
    {
        std::cerr << "Expected initial version v1.0, but got "
                  << deviceVersion.value() << '\n';
        return 1;
    }

    deviceVersion.update("v1.1");

    if (deviceVersion.value() != "v1.1")
    {
        std::cerr << "Expected updated version v1.1, but got "
                  << deviceVersion.value() << '\n';
        return 1;
    }

    return 0;
}
