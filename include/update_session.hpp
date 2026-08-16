#pragma once

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
    bool start(const std::string& pendingVersion);
    bool finishReceiving();
    bool finishVerification(bool passed);
    bool activate(DeviceVersion& activeVersion);
    bool cancel();

private:
    UpdateState state_{UpdateState::Idle};
    std::string pendingVersion_{};
};
