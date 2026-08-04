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

    return 0;
}
