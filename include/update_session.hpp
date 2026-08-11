#pragma once

enum class UpdateState
{
    Idle,
    Receiving,
    Verifying,
    ReadyToActivate,
    Failed
};

class UpdateSession
{
public:
    UpdateState state() const;
    bool start();
    bool finishReceiving();
    bool finishVerification(bool passed);

private:
    UpdateState state_{UpdateState::Idle};
};
