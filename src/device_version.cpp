#include "device_version.hpp"

DeviceVersion::DeviceVersion(const std::string& initialVersion)
    : version_{initialVersion}
{
}

const std::string& DeviceVersion::value() const
{
    return version_;
}

void DeviceVersion::update(const std::string& newVersion)
{
    version_ = newVersion;
}
