#include "update_session.hpp"

UpdateState UpdateSession::state() const
{
    return state_;
}

bool UpdateSession::start()
{
    if (state_ != UpdateState::Idle)
    {
        return false;
    }

    state_ = UpdateState::Receiving;
    return true;
}
