#include "device_version.hpp"
#include "update_session.hpp"

#include <charconv>
#include <iostream>
#include <sstream>
#include <string>
#include <system_error>

namespace
{

const char* stateName(UpdateState state)
{
    switch (state)
    {
    case UpdateState::Idle:
        return "Idle";
    case UpdateState::Receiving:
        return "Receiving";
    case UpdateState::Verifying:
        return "Verifying";
    case UpdateState::ReadyToActivate:
        return "ReadyToActivate";
    case UpdateState::Active:
        return "Active";
    case UpdateState::Failed:
        return "Failed";
    }

    return "Unknown";
}

const char* failureName(UpdateFailureReason reason)
{
    switch (reason)
    {
    case UpdateFailureReason::None:
        return "None";
    case UpdateFailureReason::Cancelled:
        return "Cancelled";
    case UpdateFailureReason::VerificationFailed:
        return "VerificationFailed";
    }

    return "Unknown";
}

bool parseSize(const std::string& text, std::size_t& value)
{
    if (text.empty() || text.front() == '-')
    {
        return false;
    }

    const char* const begin = text.data();
    const char* const end = begin + text.size();
    const auto result = std::from_chars(begin, end, value);

    return result.ec == std::errc{} && result.ptr == end;
}

bool hasNoRemainingArguments(std::istringstream& input)
{
    std::string extra;
    return !(input >> extra);
}

void printStatus(
    const std::string& command,
    bool accepted,
    const UpdateSession& session,
    const DeviceVersion& activeVersion)
{
    std::cout << "command=" << command
              << " accepted=" << (accepted ? "true" : "false")
              << " state=" << stateName(session.state())
              << " active_version=" << activeVersion.value()
              << " expected_bytes=" << session.expectedBytes()
              << " received_bytes=" << session.receivedBytes()
              << " failure=" << failureName(session.failureReason())
              << '\n';
}

}

int main()
{
    DeviceVersion activeVersion{"v1.0"};
    UpdateSession session{};
    std::string line;

    while (std::getline(std::cin, line))
    {
        std::istringstream input{line};
        std::string command;

        if (!(input >> command))
        {
            printStatus("unknown", false, session, activeVersion);
            continue;
        }

        if (command == "status")
        {
            const bool accepted = hasNoRemainingArguments(input);
            printStatus(command, accepted, session, activeVersion);
        }
        else if (command == "start")
        {
            std::string version;
            std::string expectedBytesText;
            std::size_t expectedBytes{};
            const bool argumentsValid =
                static_cast<bool>(input >> version >> expectedBytesText) &&
                parseSize(expectedBytesText, expectedBytes) &&
                hasNoRemainingArguments(input);
            const bool accepted =
                argumentsValid && session.start(version, expectedBytes);

            printStatus(command, accepted, session, activeVersion);
        }
        else if (command == "receive")
        {
            std::string chunkBytesText;
            std::size_t chunkBytes{};
            const bool argumentsValid =
                static_cast<bool>(input >> chunkBytesText) &&
                parseSize(chunkBytesText, chunkBytes) &&
                hasNoRemainingArguments(input);
            const bool accepted =
                argumentsValid && session.receiveChunk(chunkBytes);

            printStatus(command, accepted, session, activeVersion);
        }
        else if (command == "finish-receiving")
        {
            const bool argumentsValid = hasNoRemainingArguments(input);
            const bool accepted =
                argumentsValid && session.finishReceiving();
            printStatus(command, accepted, session, activeVersion);
        }
        else if (command == "finish-verification")
        {
            std::string result;
            const bool argumentsValid =
                static_cast<bool>(input >> result) &&
                hasNoRemainingArguments(input) &&
                (result == "pass" || result == "fail");
            const bool accepted = argumentsValid &&
                session.finishVerification(result == "pass");

            printStatus(command, accepted, session, activeVersion);
        }
        else if (command == "activate")
        {
            const bool argumentsValid = hasNoRemainingArguments(input);
            const bool accepted =
                argumentsValid && session.activate(activeVersion);
            printStatus(command, accepted, session, activeVersion);
        }
        else if (command == "cancel")
        {
            const bool argumentsValid = hasNoRemainingArguments(input);
            const bool accepted = argumentsValid && session.cancel();
            printStatus(command, accepted, session, activeVersion);
        }
        else if (command == "reset")
        {
            const bool argumentsValid = hasNoRemainingArguments(input);
            const bool accepted = argumentsValid && session.resetSession();
            printStatus(command, accepted, session, activeVersion);
        }
        else if (command == "quit")
        {
            const bool accepted = hasNoRemainingArguments(input);
            printStatus(command, accepted, session, activeVersion);

            if (accepted)
            {
                return 0;
            }
        }
        else
        {
            printStatus("unknown", false, session, activeVersion);
        }
    }

    return 0;
}
