#include "update_session.hpp"

#include "device_version.hpp"

UpdateState UpdateSession::state() const
{
    return state_;
}

bool UpdateSession::start(const std::string& pendingVersion)
{
    if (state_ != UpdateState::Idle || pendingVersion.empty())
    {
        return false;
    }

    pendingVersion_ = pendingVersion;
    state_ = UpdateState::Receiving;
    return true;
}

bool UpdateSession::finishReceiving()
{
    if (state_ != UpdateState::Receiving)
    {
        return false;
    }

    state_ = UpdateState::Verifying;
    return true;
}

bool UpdateSession::finishVerification(bool passed)
{
    if (state_ != UpdateState::Verifying)
    {
        return false;
    }

    if (passed)
    {
        state_ = UpdateState::ReadyToActivate;
    }
    else
    {
        state_ = UpdateState::Failed;
    }

    return true;
}

bool UpdateSession::activate(DeviceVersion& activeVersion)
{
    if (state_ != UpdateState::ReadyToActivate)
    {
        return false;
    }

    if (pendingVersion_.empty())
    {
        state_ = UpdateState::Failed;
        return false;
    }

    activeVersion.update(pendingVersion_);
    state_ = UpdateState::Active;
    return true;
}

bool UpdateSession::cancel()
{
    if (state_ != UpdateState::Receiving &&
        state_ != UpdateState::Verifying &&
        state_ != UpdateState::ReadyToActivate)
    {
        return false;
    }

    pendingVersion_.clear();
    state_ = UpdateState::Failed;
    return true;
}

bool UpdateSession::resetSession()
{
    if (state_ != UpdateState::Active &&
        state_ != UpdateState::Failed)
    {
        return false;
    }

    pendingVersion_.clear();
    state_ = UpdateState::Idle;
    return true;
}
