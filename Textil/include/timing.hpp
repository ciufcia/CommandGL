/**
 * @file timing.hpp
 * @brief High-precision timing and frame rate control for Textil library
 * @details Provides accurate timing measurements and frame rate limiting functionality
 *          using C++11 chrono library. Essential for smooth animations, performance
 *          profiling, and consistent frame timing in real-time applications.
 */

#ifndef TIL_TIMING_HPP
#define TIL_TIMING_HPP

#include <chrono>
#include "numeric_types.hpp"

namespace til
{
    /**
     * @brief High-precision clock for timing measurements and frame rate control
     * @details Provides microsecond-precision timing using std::chrono::steady_clock.
     *          Designed for measuring frame times, implementing frame rate limiting,
     *          and providing consistent time references for animations and updates.
     *          
     *          The clock maintains two key time points:
     *          - Start time: When the clock was constructed (never changes)
     *          - Last tick time: When tick() was last called (updates each tick)
     *          
     *          This design enables both total runtime measurement and per-frame timing.
     */
    class Clock
    {
    public:
        /**
         * @brief Get the clock's start time
         * @return Time point when this clock was constructed
         * @details Returns the fixed start time established when the clock was created.
         *          Useful for measuring total application runtime or elapsed time since initialization.
         */
        std::chrono::steady_clock::time_point getStartTime() const;

        /**
         * @brief Get the time of the last tick() call
         * @return Time point of the most recent tick() call
         * @details Returns when tick() was last called, allowing measurement of time between ticks.
         *          Initially set to construction time if tick() has never been called.
         */
        std::chrono::steady_clock::time_point getLastTickTime() const;

        /**
         * @brief Get the current time
         * @return Current time point from steady_clock
         * @details Convenience function that returns the current high-precision time.
         *          Equivalent to std::chrono::steady_clock::now() but maintains consistency with clock interface.
         */
        std::chrono::steady_clock::time_point getCurrentTime() const;

        /**
         * @brief Get total duration since clock construction
         * @return Duration from start time to current time
         * @details Measures total elapsed time since the clock was created.
         *          Useful for tracking application uptime or total session duration.
         */
        std::chrono::steady_clock::duration getRunningDuration() const;

        /**
         * @brief Get duration since last tick() call
         * @return Duration from last tick to current time
         * @details Measures time elapsed since the last tick() call without updating the tick time.
         *          Useful for real-time monitoring of frame timing without affecting the tick state.
         */
        std::chrono::steady_clock::duration getTickDuration() const;

        /**
         * @brief Update tick time and return elapsed duration
         * @return Duration between previous and current tick
         * @details Primary timing function that updates the internal tick time and returns
         *          the duration since the previous tick. Essential for frame timing measurements:
         *          
         *          ```cpp
         *          Clock clock;
         *          while (running) {
         *              auto frameTime = clock.tick();
         *              // frameTime contains duration of last frame
         *              update(frameTime);
         *              render();
         *          }
         *          ```
         */
        std::chrono::steady_clock::duration tick();
        
        /**
         * @brief Sleep for specified duration
         * @param duration Time to sleep/wait
         * @details Blocks execution for the specified duration using high-precision sleep.
         *          Primarily used for frame rate limiting to maintain consistent timing.
         *          
         *          Example usage for 60 FPS limiting:
         *          ```cpp
         *          const auto targetFrameTime = std::chrono::milliseconds(16); // ~60 FPS
         *          auto frameTime = clock.tick();
         *          if (frameTime < targetFrameTime) {
         *              clock.wait(targetFrameTime - frameTime);
         *          }
         *          ```
         */
        void wait(std::chrono::steady_clock::duration duration);

    private:
        /// Time point when the clock was constructed (fixed reference point)
        std::chrono::steady_clock::time_point m_startTime = std::chrono::steady_clock::now();   
        
        /// Time point of the last tick() call (updated by tick())
        std::chrono::steady_clock::time_point m_lastTickTime = std::chrono::steady_clock::now();
    };

    /**
     * @brief Convert duration to seconds as floating-point
     * @param duration Duration to convert
     * @return Duration in seconds as f32
     * @details Converts chrono duration to seconds with fractional precision.
     *          Useful for physics calculations, animation timing, and user display.
     */
    f32 getDurationInSeconds(const std::chrono::steady_clock::duration &duration);
    
    /**
     * @brief Convert duration to milliseconds as floating-point
     * @param duration Duration to convert  
     * @return Duration in milliseconds as f32
     * @details Converts chrono duration to milliseconds with fractional precision.
     *          Common unit for frame timing display and performance monitoring.
     */
    f32 getDurationInMilliseconds(const std::chrono::steady_clock::duration &duration);
    
    /**
     * @brief Convert duration to microseconds as floating-point
     * @param duration Duration to convert
     * @return Duration in microseconds as f32
     * @details Converts chrono duration to microseconds with fractional precision.
     *          Useful for high-precision timing analysis and performance profiling.
     */
    f32 getDurationInMicroseconds(const std::chrono::steady_clock::duration &duration);
    
    /**
     * @brief Convert duration to nanoseconds as floating-point
     * @param duration Duration to convert
     * @return Duration in nanoseconds as f32
     * @details Converts chrono duration to nanoseconds with fractional precision.
     *          Highest precision timing unit for detailed performance analysis.
     */
    f32 getDurationInNanoseconds(const std::chrono::steady_clock::duration &duration);
    
    /**
     * @brief Calculate frames per second from frame duration
     * @param frameTime Duration of a single frame
     * @return FPS as floating-point value
     * @details Calculates instantaneous FPS from frame timing: FPS = 1.0 / frame_time_in_seconds.
     *          Returns 0.0 for zero or negative frame times to avoid division by zero.
     *          
     *          For stable FPS display, consider averaging over multiple frames:
     *          ```cpp
     *          std::deque<f32> frameHistory;
     *          frameHistory.push_back(calculateFPS(frameTime));
     *          if (frameHistory.size() > 60) frameHistory.pop_front();
     *          f32 avgFPS = std::accumulate(frameHistory.begin(), frameHistory.end(), 0.0f) / frameHistory.size();
     *          ```
     */
    f32 calculateFPS(const std::chrono::steady_clock::duration &frameTime);
}

#endif // TIL_TIMING_HPP