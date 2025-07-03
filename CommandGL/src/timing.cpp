#include "cgl.hpp"

namespace cgl
{
    std::chrono::steady_clock::time_point Clock::getStartTime() const {
        return m_startTime;
    }

    std::chrono::steady_clock::time_point Clock::getLastTickTime() const {
        return m_lastTickTime;
    }

    std::chrono::steady_clock::time_point Clock::getCurrentTime() const {
        return std::chrono::steady_clock::now();
    }

    std::chrono::steady_clock::duration Clock::getRunningDuration() const {
        return getCurrentTime() - m_startTime;
    }

    std::chrono::steady_clock::duration Clock::getTickDuration() const {
        return getCurrentTime() - m_lastTickTime;
    }

    std::chrono::steady_clock::duration Clock::tick() {
        auto now = getCurrentTime();
        auto duration = now - m_lastTickTime;
        m_lastTickTime = now;
        return duration;
    }
}