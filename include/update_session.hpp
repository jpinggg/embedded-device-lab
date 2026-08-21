#pragma once

#include <cstddef>
#include <string>

class DeviceVersion;

enum class UpdateState
{
    Idle,
    Receiving,
    Verifying,
    ReadyToActivate,
    Active,
    Failed
};

class UpdateSession
{
public:
    UpdateState state() const;
    std::size_t expectedBytes() const;
    std::size_t receivedBytes() const;
    bool start(const std::string& pendingVersion, std::size_t expectedBytes);
    bool receiveChunk(std::size_t chunkBytes);
    bool finishReceiving();
    bool finishVerification(bool passed);
    bool activate(DeviceVersion& activeVersion);
    bool cancel();
    bool resetSession();

private:
    UpdateState state_{UpdateState::Idle};
    std::string pendingVersion_{};
    std::size_t expectedBytes_{0};
    std::size_t receivedBytes_{0};
};
