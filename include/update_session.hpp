#pragma once

enum class UpdateState
{
    Idle,
    Receiving,
    Verifying
};

class UpdateSession
{
public:
    UpdateState state() const;
    bool start();
    bool finishReceiving();

private:
    UpdateState state_{UpdateState::Idle};
};
