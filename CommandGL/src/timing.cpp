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

    f32 getDurationInSeconds(const std::chrono::steady_clock::duration &duration) {
        return std::chrono::duration_cast<std::chrono::duration<float>>(duration).count();
    }

    f32 getDurationInMilliseconds(const std::chrono::steady_clock::duration &duration) {
        return std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(duration).count();
    }

    f32 getDurationInMicroseconds(const std::chrono::steady_clock::duration &duration) {
        return std::chrono::duration_cast<std::chrono::duration<float, std::micro>>(duration).count();
    }

    f32 getDurationInNanoseconds(const std::chrono::steady_clock::duration &duration) {
        return std::chrono::duration_cast<std::chrono::duration<float, std::nano>>(duration).count();
    }

    f32 calculateFPS(const std::chrono::steady_clock::duration &frameTime) {
        if (frameTime.count() == 0) {
            return 0.f;
        }

        return 1.f / getDurationInSeconds(frameTime);
    }
}