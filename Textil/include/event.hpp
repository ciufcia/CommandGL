/**
 * @file event.hpp
 * @brief Event system for input handling in Textil library
 * @details Provides a flexible event system for capturing and processing various types of input
 *          including keyboard presses, mouse movement, scrolling, and console resize events.
 *          Uses type-safe event classification with RTTI for efficient event dispatching.
 */

#ifndef TIL_EVENTS_HPP
#define TIL_EVENTS_HPP

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32
#include <memory>
#include <optional>
#include <array>
#include "keycodes.hpp"
#include <vector>

namespace til
{
    /**
     * @brief Base type for all event type markers
     * @details Abstract base class providing virtual destructor for proper inheritance.
     *          Event types are used as type markers to classify different kinds of events
     *          using C++ RTTI (Run-Time Type Information) for safe type checking.
     */
    struct EventType { virtual ~EventType() = default; };

    /**
     * @brief Marker type for invalid or unrecognized events
     * @details Default event type used when an event cannot be properly classified.
     *          Indicates error conditions or unhandled input types.
     */
    struct InvalidEvent : EventType {};

    /**
     * @brief Marker type for keyboard key press events
     * @details Indicates that a keyboard key was pressed down.
     *          Associated event data includes the specific key code.
     */
    struct KeyPressEvent : EventType {};

    /**
     * @brief Marker type for keyboard key release events
     * @details Indicates that a keyboard key was released.
     *          Associated event data includes the specific key code.
     */
    struct KeyReleaseEvent : EventType {};

    /**
     * @brief Marker type for mouse movement events
     * @details Indicates that the mouse cursor position changed.
     *          Associated event data includes movement delta in pixels.
     */
    struct MouseMoveEvent : EventType {};

    /**
     * @brief Marker type for mouse scroll wheel events
     * @details Indicates that the mouse scroll wheel was moved.
     *          Associated event data includes scroll direction and magnitude.
     */
    struct MouseScrollEvent : EventType {};

    /**
     * @brief Marker type for console/terminal events
     * @details Indicates console-specific events such as window resize, focus changes,
     *          or other terminal state modifications that affect the display.
     */
    struct ConsoleEvent : EventType {};
    
    /**
     * @brief Unified event class containing all possible input event data
     * @details Central event representation supporting multiple input types through
     *          a type-safe classification system. Each event has a specific type marker
     *          and contains data fields relevant to different event types.
     *          
     *          Events are lightweight and can be efficiently copied and stored in queues.
     *          The type system allows for safe event handling without dynamic casts
     *          at the application level.
     * 
     *          Usage pattern:
     *          ```cpp
     *          Event event = getNextEvent();
     *          if (event.isOfType<KeyPressEvent>()) {
     *              handleKeyPress(event.key);
     *          } else if (event.isOfType<MouseMoveEvent>()) {
     *              handleMouseMove(event.mouseDelta);
     *          }
     *          ```
     */
    class Event
    {
    public:
        /**
         * @brief Check if event is of specific type
         * @tparam T Event type to check for (must derive from EventType)
         * @return True if event is of the specified type
         * @details Type-safe method for checking event classification using RTTI.
         *          More efficient and safer than manual type checking or switches.
         *          
         *          Example:
         *          ```cpp
         *          if (event.isOfType<KeyPressEvent>()) {
         *              // Handle key press
         *          }
         *          ```
         */
        template<typename T>
        requires std::derived_from<T, EventType>
        bool isOfType() const;

        /**
         * @brief Set the event type classification
         * @tparam T Event type to set (must derive from EventType)
         * @details Changes the event's type classification. Typically used during
         *          event creation and processing rather than in application code.
         *          
         *          Example:
         *          ```cpp
         *          Event event;
         *          event.setType<KeyPressEvent>();
         *          event.key = KeyCode::Space;
         *          ```
         */
        template<typename T>
        requires std::derived_from<T, EventType>
        void setType();

        /// Key code for keyboard events (KeyPressEvent, KeyReleaseEvent)
        /// Contains the specific key that was pressed or released
        KeyCode key { KeyCode::Invalid };

        /// Mouse movement delta for MouseMoveEvent
        /// Represents change in mouse position since last event (dx, dy) in pixels
        Vector2<i32> mouseDelta { 0u, 0u };

        /// Mouse scroll wheel delta for MouseScrollEvent  
        /// Positive values typically indicate scroll up/away, negative values scroll down/toward
        i8 mouseScrollDelta { 0u };

        /// New console size for ConsoleEvent
        /// Contains new terminal dimensions (width, height) in character cells after resize
        Vector2<u32> newSize { 0u, 0u };

      private:
        /// Internal type marker using shared_ptr for efficient copying
        /// Defaults to InvalidEvent for uninitialized events
        std::shared_ptr<EventType> type = std::make_shared<InvalidEvent>();
    };    
    
    template<typename T>
    requires std::derived_from<T, EventType>
    bool Event::isOfType() const {
        return dynamic_cast<const T*>(type.get()) != nullptr;
    }

    template<typename T>
    requires std::derived_from<T, EventType>
    void Event::setType() {
        type = std::make_shared<T>();
    }
}

#endif // TIL_EVENTS_HPP