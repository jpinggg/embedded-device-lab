#include "update_session.hpp"

#include <iostream>

int main()
{
    UpdateSession session{};

    if (session.state() != UpdateState::Idle)
    {
        std::cerr << "Expected initial state to be Idle\n";
        return 1;
    }

    if (!session.start())
    {
        std::cerr << "Expected first start to succeed\n";
        return 1;
    }

    if (session.state() != UpdateState::Receiving)
    {
        std::cerr << "Expected state to be Receiving after start\n";
        return 1;
    }

    if (session.start())
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

    return 0;
}
