#ifndef CGL_TIMING_HPP
#define CGL_TIMING_HPP

#include <chrono>
#include "numeric_types.hpp"

namespace cgl
{
    /**
     * @class Clock
     * @brief High-resolution timer for measuring elapsed time and frame deltas.
     * 
     * The Clock class provides functionality for timing operations, measuring
     * frame deltas, and tracking elapsed time since creation. It uses
     * std::chrono::steady_clock for high-resolution, monotonic timing.
     */
    class Clock
    {
    public:
    
        /**
         * @brief Gets the time point when the clock was created.
         * @return The start time as a steady_clock time_point.
         */
        std::chrono::steady_clock::time_point getStartTime() const;
        
        /**
         * @brief Gets the time point of the last tick() call.
         * @return The last tick time as a steady_clock time_point.
         */
        std::chrono::steady_clock::time_point getLastTickTime() const;
        
        /**
         * @brief Gets the current time.
         * @return The current time as a steady_clock time_point.
         */
        std::chrono::steady_clock::time_point getCurrentTime() const;

        /**
         * @brief Gets the total elapsed time since the clock was created.
         * @return Duration since clock creation.
         */
        std::chrono::steady_clock::duration getRunningDuration() const;
        
        /**
         * @brief Gets the duration since the last tick() call without updating the timer.
         * @return Duration since the last tick() call.
         */
        std::chrono::steady_clock::duration getTickDuration() const;
        
        /**
         * @brief Gets the duration since the last tick() call and updates the tick timer.
         * @return Duration since the last tick() call.
         * 
         * This method updates the internal last tick time to the current time,
         * making it suitable for frame timing in game loops.
         */
        std::chrono::steady_clock::duration tick();

    private:

        std::chrono::steady_clock::time_point m_startTime = std::chrono::steady_clock::now();   
        std::chrono::steady_clock::time_point m_lastTickTime = std::chrono::steady_clock::now();
    };

    /**
     * @brief Converts a std::chrono::steady_clock::duration to seconds as a floating-point value.
     * @param duration The duration to convert.
     * @return The duration in seconds.
     */
    f32 getDurationInSeconds(const std::chrono::steady_clock::duration &duration);
    /**
     * @brief Converts a std::chrono::steady_clock::duration to milliseconds as a floating-point value.
     * @param duration The duration to convert.
     * @return The duration in milliseconds.
     */
    f32 getDurationInMilliseconds(const std::chrono::steady_clock::duration &duration);
    /**
     * @brief Converts a std::chrono::steady_clock::duration to microseconds as a floating-point value.
     * @param duration The duration to convert.
     * @return The duration in microseconds.
     */
    f32 getDurationInMicroseconds(const std::chrono::steady_clock::duration &duration);
    /**
     * @brief Converts a std::chrono::steady_clock::duration to nanoseconds as a floating-point value.
     * @param duration The duration to convert.
     * @return The duration in nanoseconds.
     */
    f32 getDurationInNanoseconds(const std::chrono::steady_clock::duration &duration);
    /**
     * @brief Calculates frames per second (FPS) from a frame time duration.
     * @param frameTime The duration of a single frame.
     * @return The calculated FPS as a floating-point value.
     */
    f32 calculateFPS(const std::chrono::steady_clock::duration &frameTime);
}

#endif // CGL_TIMING_HPP