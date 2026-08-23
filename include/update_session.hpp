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

enum class UpdateFailureReason
{
    None,
    Cancelled,
    VerificationFailed
};

class UpdateSession
{
public:
    UpdateState state() const;
    UpdateFailureReason failureReason() const;
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
    UpdateFailureReason failureReason_{UpdateFailureReason::None};
    std::string pendingVersion_{};
    std::size_t expectedBytes_{0};
    std::size_t receivedBytes_{0};
};
