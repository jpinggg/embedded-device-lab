#include <iostream>
#include <string>

class DeviceVersion
{
public:
    DeviceVersion(const std::string& initialVersion)
        : version_{initialVersion}
    {
    }

    const std::string& value() const
    {
        return version_;
    }

    void update(const std::string& newVersion)
    {
        version_ = newVersion;
    }

private:
    std::string version_;
};

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
