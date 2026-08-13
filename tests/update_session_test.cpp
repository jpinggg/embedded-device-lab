#include "update_session.hpp"

#include "device_version.hpp"

#include <iostream>

int main()
{
    UpdateSession session{};

    if (session.state() != UpdateState::Idle)
    {
        std::cerr << "Expected initial state to be Idle\n";
        return 1;
    }

    UpdateSession emptyVersionSession{};

    if (emptyVersionSession.start(""))
    {
        std::cerr << "Expected empty pending version to be rejected\n";
        return 1;
    }

    if (emptyVersionSession.state() != UpdateState::Idle)
    {
        std::cerr << "Expected rejected empty version to preserve Idle state\n";
        return 1;
    }

    if (!session.start("v2.0"))
    {
        std::cerr << "Expected first start to succeed\n";
        return 1;
    }

    if (session.state() != UpdateState::Receiving)
    {
        std::cerr << "Expected state to be Receiving after start\n";
        return 1;
    }

    if (session.start("v3.0"))
    {
        std::cerr << "Expected second start to fail\n";
        return 1;
    }

    if (session.state() != UpdateState::Receiving)
    {
        std::cerr << "Expected rejected start to preserve Receiving state\n";
        return 1;
    }

    UpdateSession idleSession{};

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

    if (!session.finishReceiving())
    {
        std::cerr << "Expected finishReceiving from Receiving to succeed\n";
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

    if (!failedSession.start("v3.0"))
    {
        std::cerr << "Expected failed-session setup to start successfully\n";
        return 1;
    }

    if (!failedSession.finishReceiving())
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

    return 0;
}
