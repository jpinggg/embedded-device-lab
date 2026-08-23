#include "update_session.hpp"

#include "device_version.hpp"

UpdateState UpdateSession::state() const
{
    return state_;
}

UpdateFailureReason UpdateSession::failureReason() const
{
    return failureReason_;
}

std::size_t UpdateSession::expectedBytes() const
{
    return expectedBytes_;
}

std::size_t UpdateSession::receivedBytes() const
{
    return receivedBytes_;
}

bool UpdateSession::start(
    const std::string& pendingVersion,
    std::size_t expectedBytes)
{
    if (state_ != UpdateState::Idle ||
        pendingVersion.empty() ||
        expectedBytes == 0)
    {
        return false;
    }

    pendingVersion_ = pendingVersion;
    expectedBytes_ = expectedBytes;
    receivedBytes_ = 0;
    state_ = UpdateState::Receiving;
    return true;
}

bool UpdateSession::receiveChunk(std::size_t chunkBytes)
{
    if (state_ != UpdateState::Receiving ||
        chunkBytes == 0 ||
        chunkBytes > expectedBytes_ - receivedBytes_)
    {
        return false;
    }

    receivedBytes_ += chunkBytes;
    return true;
}

bool UpdateSession::finishReceiving()
{
    if (state_ != UpdateState::Receiving ||
        receivedBytes_ != expectedBytes_)
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
        failureReason_ = UpdateFailureReason::VerificationFailed;
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
    expectedBytes_ = 0;
    receivedBytes_ = 0;
    failureReason_ = UpdateFailureReason::Cancelled;
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
    expectedBytes_ = 0;
    receivedBytes_ = 0;
    failureReason_ = UpdateFailureReason::None;
    state_ = UpdateState::Idle;
    return true;
}
