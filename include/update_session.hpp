#pragma once

enum class UpdateState
{
    Idle,
    Receiving
};

class UpdateSession
{
public:
    UpdateState state() const;
    bool start();

private:
    UpdateState state_{UpdateState::Idle};
};
