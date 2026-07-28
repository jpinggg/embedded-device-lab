#pragma once

#include <string>

class DeviceVersion
{
public:
    DeviceVersion(const std::string& initialVersion);

    const std::string& value() const;

    void update(const std::string& newVersion);

private:
    std::string version_;
};
