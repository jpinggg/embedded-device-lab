#include <iostream>
#include <string>
#include <vector>

void updateByValue(std::string version)
{
    version = "v2.0-copy";
    std::cout << "Inside updateByValue: " << version << '\n';
}

void updateByReference(std::string& version)
{
    version = "v2.0-reference";
    std::cout << "Inside updateByReference: " << version << '\n';
}

void printVersion(const std::string& version)
{
    std::cout << "Current version: " << version << '\n';
}

int main()
{
    std::string currentVersion{"v1.0"};

    std::cout << "Before pass by value: " << currentVersion << '\n';
    updateByValue(currentVersion);
    std::cout << "After pass by value: " << currentVersion << '\n';

    updateByReference(currentVersion);
    std::cout << "After pass by reference: " << currentVersion << '\n';

    std::vector<std::string> versions{
        "v1.0",
        "v1.1",
        "v2.0"
    };

    for (const std::string& version : versions) {
        printVersion(version);
    }
    std::cout << "Before modification:\n";

    for (const std::string& version : versions) {
        std::cout << version << '\n';
    }

    for (std::string& version : versions) {
        version += "-tested";
    }

    std::cout << "After modification:\n";

    for (const std::string& version : versions) {
        std::cout << version << '\n';
    }
    return 0;
}
