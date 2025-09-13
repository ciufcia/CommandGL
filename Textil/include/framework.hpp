/**
 * @file framework.hpp
 * @brief Main framework class for Textil terminal graphics library
 * @details Provides the central Framework class that coordinates all major subsystems
 *          including console management, rendering, window management, and event handling.
 *          This is the primary interface for applications using Textil.
 */

#ifndef TIL_FRAMEWORK_HPP
#define TIL_FRAMEWORK_HPP

#include "global_memory.hpp"
#include "console.hpp"
#include "filters.hpp"
#include "filter_pipeline.hpp"
#include "timing.hpp"
#include "character_cell.hpp"
#include "window.hpp"
#include "window_manager.hpp"
#include "event_manager.hpp"

namespace til
{
    /**
     * @brief Central framework class coordinating all Textil subsystems
     * @details The Framework class serves as the main entry point for Textil applications.
     *          It manages the initialization and coordination of all major subsystems including:
     *          - Console interface for terminal I/O
     *          - Renderer for graphics processing
     *          - Window manager for UI organization
     *          - Event manager for input handling
     *          - Timing system for frame rate control
     * 
     *          Typical usage pattern:
     *          1. Create Framework instance
     *          2. Call initialize()
     *          3. In main loop: call update(), then display()
     *          4. Framework handles cleanup automatically on destruction
     */
    class Framework
    {
    public:
        /**
         * @brief Default constructor
         * @details Creates an uninitialized framework. Call initialize() before use.
         */
        Framework() = default;
        
        /**
         * @brief Destructor handling cleanup
         * @details Automatically cleans up all subsystems and releases resources.
         */
        ~Framework() = default;

        /**
         * @brief Initialize all framework subsystems
         * @details Must be called before any other framework operations. Performs:
         *          - Console initialization and setup
         *          - Event system initialization  
         *          - Screen clearing and preparation
         *          - Internal timing setup
         * @throws LogicError if initialization fails
         */
        void initialize();
        
        /**
         * @brief Render and display the current frame
         * @details Coordinates the rendering pipeline:
         *          1. Renders all managed windows
         *          2. Sorts windows by depth for proper layering
         *          3. Draws windows to console buffer
         *          4. Flushes buffer to terminal display
         * @throws LogicError if called before initialize()
         */
        void display();
        
        /**
         * @brief Update framework state for next frame
         * @details Performs per-frame updates:
         *          - Processes new input events
         *          - Clears renderer mesh data
         *          - Updates timing information for all windows
         *          - Enforces target frame rate timing
         * @throws LogicError if called before initialize()
         */
        void update();

        /**
         * @brief Set target update rate in updates per second
         * @param updatesPerSecond Desired update frequency (e.g., 60 for 60 FPS)
         * @details Configures frame rate limiting to maintain consistent timing.
         *          The framework will sleep between updates to match this rate.
         *          Setting to 0 disables rate limiting (run as fast as possible).
         */
        void setTargetUpdateRate(u32 updatesPerSecond);
        
        /**
         * @brief Set target update duration directly
         * @param duration Time between updates as chrono duration
         * @details More precise alternative to setTargetUpdateRate() for exact timing control.
         *          Useful for custom frame rate control or synchronization with external systems.
         */
        void setTargetUpdateDuration(std::chrono::steady_clock::duration duration);
        
        /**
         * @brief Get duration of the last update cycle
         * @return Time taken for the most recent update() call
         * @details Useful for performance monitoring and adaptive frame rate control.
         *          Does not include time spent sleeping for rate limiting.
         */
        std::chrono::steady_clock::duration getLastUpdateDuration() const;

    public:
        Console console;        ///< Terminal interface for input/output operations
        Renderer renderer;      ///< Graphics rendering system for drawing operations  
        WindowManager windowManager; ///< Manager for UI windows and layering
        EventManager eventManager;   ///< Input event processing and distribution

    private:
        Clock m_clock; ///< Internal timing system for frame rate control

        bool initialized = false; ///< Tracks whether initialize() has been called

        /// Target duration between update() calls for frame rate limiting
        std::chrono::steady_clock::duration m_targetUpdateDuration = std::chrono::steady_clock::duration::zero();
        
        /// Actual duration of the last update() call for performance monitoring
        std::chrono::steady_clock::duration m_lastUpdateDuration = std::chrono::steady_clock::duration::zero();
    };
}

#endif // TIL_FRAMEWORK_HPP