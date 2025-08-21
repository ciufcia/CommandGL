#ifndef TIL_TIMING_HPP
#define TIL_TIMING_HPP

#include <chrono>
#include "numeric_types.hpp"

namespace til
{
    class Clock
    {
    public:
        std::chrono::steady_clock::time_point getStartTime() const;

        std::chrono::steady_clock::time_point getLastTickTime() const;

        std::chrono::steady_clock::time_point getCurrentTime() const;

        std::chrono::steady_clock::duration getRunningDuration() const;

        std::chrono::steady_clock::duration getTickDuration() const;

        std::chrono::steady_clock::duration tick();
        void wait(std::chrono::steady_clock::duration duration);

    private:

        std::chrono::steady_clock::time_point m_startTime = std::chrono::steady_clock::now();   
        std::chrono::steady_clock::time_point m_lastTickTime = std::chrono::steady_clock::now();
    };

    f32 getDurationInSeconds(const std::chrono::steady_clock::duration &duration);
    f32 getDurationInMilliseconds(const std::chrono::steady_clock::duration &duration);
    f32 getDurationInMicroseconds(const std::chrono::steady_clock::duration &duration);
    f32 getDurationInNanoseconds(const std::chrono::steady_clock::duration &duration);
    f32 calculateFPS(const std::chrono::steady_clock::duration &frameTime);
}

#endif // TIL_TIMING_HPP