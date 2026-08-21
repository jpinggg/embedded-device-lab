#include "update_session.hpp"

#include "device_version.hpp"

#include <cstddef>
#include <iostream>
#include <string>

namespace
{

bool reachVerifying(
    UpdateSession& session,
    const std::string& pendingVersion)
{
    constexpr std::size_t imageSize{1000};

    return session.start(pendingVersion, imageSize) &&
           session.receiveChunk(imageSize) &&
           session.finishReceiving();
}

}

int main()
{
    UpdateSession session{};

    if (session.state() != UpdateState::Idle)
    {
        std::cerr << "Expected initial state to be Idle\n";
        return 1;
    }

    if (session.expectedBytes() != 0 || session.receivedBytes() != 0)
    {
        std::cerr << "Expected initial byte counts to be zero\n";
        return 1;
    }

    UpdateSession emptyVersionSession{};

    if (emptyVersionSession.start("", 1000))
    {
        std::cerr << "Expected empty pending version to be rejected\n";
        return 1;
    }

    if (emptyVersionSession.state() != UpdateState::Idle)
    {
        std::cerr << "Expected rejected empty version to preserve Idle state\n";
        return 1;
    }

    if (emptyVersionSession.expectedBytes() != 0 ||
        emptyVersionSession.receivedBytes() != 0)
    {
        std::cerr << "Expected rejected empty version to preserve byte counts\n";
        return 1;
    }

    UpdateSession zeroSizeSession{};

    if (zeroSizeSession.start("v2.0", 0))
    {
        std::cerr << "Expected zero image size to be rejected\n";
        return 1;
    }

    if (zeroSizeSession.state() != UpdateState::Idle ||
        zeroSizeSession.expectedBytes() != 0 ||
        zeroSizeSession.receivedBytes() != 0)
    {
        std::cerr << "Expected rejected zero size to preserve Idle session\n";
        return 1;
    }

    if (!session.start("v2.0", 1000))
    {
        std::cerr << "Expected first start to succeed\n";
        return 1;
    }

    if (session.state() != UpdateState::Receiving)
    {
        std::cerr << "Expected state to be Receiving after start\n";
        return 1;
    }

    if (session.expectedBytes() != 1000 || session.receivedBytes() != 0)
    {
        std::cerr << "Expected accepted start to initialize byte counts\n";
        return 1;
    }

    if (session.start("v3.0", 2000))
    {
        std::cerr << "Expected second start to fail\n";
        return 1;
    }

    if (session.state() != UpdateState::Receiving)
    {
        std::cerr << "Expected rejected start to preserve Receiving state\n";
        return 1;
    }

    if (session.expectedBytes() != 1000 || session.receivedBytes() != 0)
    {
        std::cerr << "Expected rejected start to preserve byte counts\n";
        return 1;
    }

    UpdateSession idleSession{};

    if (idleSession.receiveChunk(100))
    {
        std::cerr << "Expected receiveChunk from Idle to fail\n";
        return 1;
    }

    if (idleSession.state() != UpdateState::Idle ||
        idleSession.expectedBytes() != 0 ||
        idleSession.receivedBytes() != 0)
    {
        std::cerr << "Expected rejected Idle chunk to preserve session\n";
        return 1;
    }

    if (idleSession.finishReceiving())
    {
        std::cerr << "Expected finishReceiving from Idle to fail\n";
        return 1;
    }

    if (idleSession.state() != UpdateState::Idle)
    {
        std::cerr << "Expected rejected finishReceiving to preserve Idle state\n";
        return 1;
    }

    if (session.receiveChunk(0))
    {
        std::cerr << "Expected zero-byte chunk to be rejected\n";
        return 1;
    }

    if (session.state() != UpdateState::Receiving ||
        session.receivedBytes() != 0)
    {
        std::cerr << "Expected rejected zero-byte chunk to preserve session\n";
        return 1;
    }

    if (!session.receiveChunk(400))
    {
        std::cerr << "Expected first chunk to be accepted\n";
        return 1;
    }

    if (session.state() != UpdateState::Receiving ||
        session.receivedBytes() != 400)
    {
        std::cerr << "Expected first chunk to advance received bytes\n";
        return 1;
    }

    if (session.finishReceiving())
    {
        std::cerr << "Expected incomplete reception to reject finishReceiving\n";
        return 1;
    }

    if (session.state() != UpdateState::Receiving ||
        session.receivedBytes() != 400)
    {
        std::cerr << "Expected incomplete finish to preserve session\n";
        return 1;
    }

    if (session.receiveChunk(601))
    {
        std::cerr << "Expected oversized chunk to be rejected\n";
        return 1;
    }

    if (session.state() != UpdateState::Receiving ||
        session.receivedBytes() != 400)
    {
        std::cerr << "Expected rejected oversized chunk to preserve session\n";
        return 1;
    }

    if (!session.receiveChunk(600))
    {
        std::cerr << "Expected final chunk to be accepted\n";
        return 1;
    }

    if (session.receivedBytes() != 1000)
    {
        std::cerr << "Expected final chunk to complete received bytes\n";
        return 1;
    }

    if (!session.finishReceiving())
    {
        std::cerr << "Expected finishReceiving from Receiving to succeed\n";
        return 1;
    }

    if (session.receiveChunk(1))
    {
        std::cerr << "Expected receiveChunk from Verifying to fail\n";
        return 1;
    }

    if (session.state() != UpdateState::Verifying ||
        session.receivedBytes() != 1000)
    {
        std::cerr << "Expected rejected Verifying chunk to preserve session\n";
        return 1;
    }

    if (session.state() != UpdateState::Verifying)
    {
        std::cerr << "Expected state to be Verifying after finishReceiving\n";
        return 1;
    }

    if (session.finishReceiving())
    {
        std::cerr << "Expected second finishReceiving to fail\n";
        return 1;
    }

    if (session.state() != UpdateState::Verifying)
    {
        std::cerr << "Expected rejected finishReceiving to preserve Verifying state\n";
        return 1;
    }

    if (idleSession.finishVerification(true))
    {
        std::cerr << "Expected finishVerification from Idle to fail\n";
        return 1;
    }

    if (idleSession.state() != UpdateState::Idle)
    {
        std::cerr << "Expected rejected verification to preserve Idle state\n";
        return 1;
    }

    if (!session.finishVerification(true))
    {
        std::cerr << "Expected passed verification result to be accepted\n";
        return 1;
    }

    if (session.state() != UpdateState::ReadyToActivate)
    {
        std::cerr << "Expected passed verification to reach ReadyToActivate\n";
        return 1;
    }

    UpdateSession failedSession{};

    if (!reachVerifying(failedSession, "v3.0"))
    {
        std::cerr << "Expected failed-session setup to reach Verifying\n";
        return 1;
    }

    if (!failedSession.finishVerification(false))
    {
        std::cerr << "Expected failed verification result to be accepted\n";
        return 1;
    }

    if (failedSession.state() != UpdateState::Failed)
    {
        std::cerr << "Expected failed verification to reach Failed\n";
        return 1;
    }

    if (failedSession.finishVerification(true))
    {
        std::cerr << "Expected repeated verification result to be rejected\n";
        return 1;
    }

    if (failedSession.state() != UpdateState::Failed)
    {
        std::cerr << "Expected rejected result to preserve Failed state\n";
        return 1;
    }

    DeviceVersion idleActiveVersion{"v1.0"};

    if (idleSession.activate(idleActiveVersion))
    {
        std::cerr << "Expected activation from Idle to be rejected\n";
        return 1;
    }

    if (idleSession.state() != UpdateState::Idle)
    {
        std::cerr << "Expected rejected activation to preserve Idle state\n";
        return 1;
    }

    if (idleActiveVersion.value() != "v1.0")
    {
        std::cerr << "Expected rejected activation to preserve active version\n";
        return 1;
    }

    DeviceVersion activeVersion{"v1.0"};

    if (!session.activate(activeVersion))
    {
        std::cerr << "Expected activation from ReadyToActivate to succeed\n";
        return 1;
    }

    if (session.state() != UpdateState::Active)
    {
        std::cerr << "Expected successful activation to reach Active\n";
        return 1;
    }

    if (activeVersion.value() != "v2.0")
    {
        std::cerr << "Expected successful activation to install v2.0\n";
        return 1;
    }

    if (session.activate(activeVersion))
    {
        std::cerr << "Expected repeated activation to be rejected\n";
        return 1;
    }

    if (session.state() != UpdateState::Active)
    {
        std::cerr << "Expected rejected activation to preserve Active state\n";
        return 1;
    }

    if (activeVersion.value() != "v2.0")
    {
        std::cerr << "Expected rejected activation to preserve active version\n";
        return 1;
    }

    if (idleSession.cancel())
    {
        std::cerr << "Expected cancellation from Idle to be rejected\n";
        return 1;
    }

    if (idleSession.state() != UpdateState::Idle)
    {
        std::cerr << "Expected rejected cancellation to preserve Idle state\n";
        return 1;
    }

    if (idleActiveVersion.value() != "v1.0")
    {
        std::cerr << "Expected rejected cancellation to preserve active version\n";
        return 1;
    }

    UpdateSession receivingCancelSession{};
    DeviceVersion receivingActiveVersion{"v1.0"};

    if (!receivingCancelSession.start("v2.0", 1000))
    {
        std::cerr << "Expected receiving-cancel setup to start successfully\n";
        return 1;
    }

    if (!receivingCancelSession.cancel())
    {
        std::cerr << "Expected cancellation from Receiving to succeed\n";
        return 1;
    }

    if (receivingCancelSession.state() != UpdateState::Failed)
    {
        std::cerr << "Expected Receiving cancellation to reach Failed\n";
        return 1;
    }

    if (receivingActiveVersion.value() != "v1.0")
    {
        std::cerr << "Expected Receiving cancellation to preserve active version\n";
        return 1;
    }

    UpdateSession verifyingCancelSession{};

    if (!reachVerifying(verifyingCancelSession, "v3.0"))
    {
        std::cerr << "Expected verifying-cancel setup to reach Verifying\n";
        return 1;
    }

    if (!verifyingCancelSession.cancel())
    {
        std::cerr << "Expected cancellation from Verifying to succeed\n";
        return 1;
    }

    if (verifyingCancelSession.state() != UpdateState::Failed)
    {
        std::cerr << "Expected Verifying cancellation to reach Failed\n";
        return 1;
    }

    UpdateSession readyCancelSession{};
    DeviceVersion readyActiveVersion{"v1.0"};

    if (!reachVerifying(readyCancelSession, "v4.0"))
    {
        std::cerr << "Expected ready-cancel setup to reach Verifying\n";
        return 1;
    }

    if (!readyCancelSession.finishVerification(true))
    {
        std::cerr << "Expected ready-cancel setup to reach ReadyToActivate\n";
        return 1;
    }

    if (!readyCancelSession.cancel())
    {
        std::cerr << "Expected cancellation from ReadyToActivate to succeed\n";
        return 1;
    }

    if (readyCancelSession.state() != UpdateState::Failed)
    {
        std::cerr << "Expected ReadyToActivate cancellation to reach Failed\n";
        return 1;
    }

    if (readyActiveVersion.value() != "v1.0")
    {
        std::cerr << "Expected cancellation to preserve active version\n";
        return 1;
    }

    if (session.cancel())
    {
        std::cerr << "Expected cancellation from Active to be rejected\n";
        return 1;
    }

    if (session.state() != UpdateState::Active)
    {
        std::cerr << "Expected rejected cancellation to preserve Active state\n";
        return 1;
    }

    if (activeVersion.value() != "v2.0")
    {
        std::cerr << "Expected rejected cancellation to preserve active version\n";
        return 1;
    }

    if (failedSession.cancel())
    {
        std::cerr << "Expected cancellation from Failed to be rejected\n";
        return 1;
    }

    if (failedSession.state() != UpdateState::Failed)
    {
        std::cerr << "Expected rejected cancellation to preserve Failed state\n";
        return 1;
    }

    if (idleSession.resetSession())
    {
        std::cerr << "Expected resetSession from Idle to be rejected\n";
        return 1;
    }

    if (idleSession.state() != UpdateState::Idle)
    {
        std::cerr << "Expected rejected reset to preserve Idle state\n";
        return 1;
    }

    UpdateSession receivingResetSession{};

    if (!receivingResetSession.start("v5.0", 1000))
    {
        std::cerr << "Expected receiving-reset setup to start successfully\n";
        return 1;
    }

    if (receivingResetSession.resetSession())
    {
        std::cerr << "Expected resetSession from Receiving to be rejected\n";
        return 1;
    }

    if (receivingResetSession.state() != UpdateState::Receiving)
    {
        std::cerr << "Expected rejected reset to preserve Receiving state\n";
        return 1;
    }

    UpdateSession verifyingResetSession{};

    if (!reachVerifying(verifyingResetSession, "v5.0"))
    {
        std::cerr << "Expected verifying-reset setup to reach Verifying\n";
        return 1;
    }

    if (verifyingResetSession.resetSession())
    {
        std::cerr << "Expected resetSession from Verifying to be rejected\n";
        return 1;
    }

    if (verifyingResetSession.state() != UpdateState::Verifying)
    {
        std::cerr << "Expected rejected reset to preserve Verifying state\n";
        return 1;
    }

    UpdateSession readyResetSession{};

    if (!reachVerifying(readyResetSession, "v5.0") ||
        !readyResetSession.finishVerification(true))
    {
        std::cerr << "Expected ready-reset setup to reach ReadyToActivate\n";
        return 1;
    }

    if (readyResetSession.resetSession())
    {
        std::cerr << "Expected resetSession from ReadyToActivate to be rejected\n";
        return 1;
    }

    if (readyResetSession.state() != UpdateState::ReadyToActivate)
    {
        std::cerr << "Expected rejected reset to preserve ReadyToActivate state\n";
        return 1;
    }

    if (!session.resetSession())
    {
        std::cerr << "Expected resetSession from Active to succeed\n";
        return 1;
    }

    if (session.state() != UpdateState::Idle)
    {
        std::cerr << "Expected Active reset to reach Idle\n";
        return 1;
    }

    if (activeVersion.value() != "v2.0")
    {
        std::cerr << "Expected Active reset to preserve installed version\n";
        return 1;
    }

    if (!session.start("v3.0", 1000))
    {
        std::cerr << "Expected a new update after Active reset to start\n";
        return 1;
    }

    if (session.state() != UpdateState::Receiving)
    {
        std::cerr << "Expected reused Active session to reach Receiving\n";
        return 1;
    }

    DeviceVersion failedActiveVersion{"v1.0"};

    if (!failedSession.resetSession())
    {
        std::cerr << "Expected resetSession from Failed to succeed\n";
        return 1;
    }

    if (failedSession.state() != UpdateState::Idle)
    {
        std::cerr << "Expected Failed reset to reach Idle\n";
        return 1;
    }

    if (failedActiveVersion.value() != "v1.0")
    {
        std::cerr << "Expected Failed reset to preserve installed version\n";
        return 1;
    }

    if (!failedSession.start("v4.0", 1000))
    {
        std::cerr << "Expected a new update after Failed reset to start\n";
        return 1;
    }

    if (failedSession.state() != UpdateState::Receiving)
    {
        std::cerr << "Expected reused Failed session to reach Receiving\n";
        return 1;
    }

    return 0;
}
