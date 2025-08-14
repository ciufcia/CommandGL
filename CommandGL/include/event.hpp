#ifndef CGL_EVENTS_HPP
#define CGL_EVENTS_HPP

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32
#include <memory>
#include <optional>
#include <array>
#include "keycodes.hpp"

namespace cgl
{
    /**
     * @struct EventType
     * @brief Base type for all event types in the event system.
     */
    struct EventType { virtual ~EventType() = default; };

    /**
     * @struct InvalidEvent
     * @brief Event type marker for invalid or uninitialized events.
     */
    struct InvalidEvent : EventType {};

    /**
     * @struct KeyPressEvent
     * @brief Event type marker for keyboard key press events.
     */
    struct KeyPressEvent : EventType {};

    /**
     * @struct KeyReleaseEvent
     * @brief Event type marker for keyboard key release events.
     */
    struct KeyReleaseEvent : EventType {};

    /**
     * @struct MouseMoveEvent
     * @brief Event type marker for mouse movement events.
     */
    struct MouseMoveEvent : EventType {};

    /**
     * @struct MouseScrollEvent
     * @brief Event type marker for mouse scroll wheel events.
     */
    struct MouseScrollEvent : EventType {};

    /**
     * @struct ConsoleEvent
     * @brief Event type marker for console window events (such as resize).
     */
    struct ConsoleEvent : EventType {};

    /**
     * @class Event
     * @brief Represents an input event with type-safe event classification.
     * 
     * The Event class provides a type-safe way to handle different kinds of input events
     * including keyboard presses/releases, mouse movement, mouse scrolling, and console
     * window events.
     */
    class Event
    {
    public:

        /**
         * @brief Checks if the event is of a specific type.
         * @tparam T The event type to check for (must derive from EventType).
         * @return True if the event is of the specified type, false otherwise.
         */
        template<typename T>
        requires std::derived_from<T, EventType>
        bool isOfType() const;

        /**
         * @brief Sets the event to a specific type.
         * @tparam T The event type to set (must derive from EventType).
         */
        template<typename T>
        requires std::derived_from<T, EventType>
        void setType();

        /**
         * @brief The key code associated with keyboard and mouse button events.
         * 
         * This field is valid for KeyPressEvent and KeyReleaseEvent types.
         * For other event types, this field may contain KeyCode::Invalid.
         */
        KeyCode key { KeyCode::Invalid };
        
        /**
         * @brief The change in mouse position since the last mouse event.
         * 
         * This field is valid for MouseMoveEvent and represents the movement delta.
         */
        Vector2<i32> mouseDelta { 0u, 0u };
        
        /**
         * @brief The scroll wheel delta for mouse scroll events.
         * 
         * This field is valid for MouseScrollEvent. Positive values indicate
         * scrolling up/away from the user, negative values indicate scrolling
         * down/toward the user.
         */
        i8 mouseScrollDelta { 0u };

        /**
         * @brief The new console window size for console resize events.
         * 
         * This field is valid for ConsoleEvent and contains the new dimensions
         * of the console window in character cells.
         */
        Vector2<u32> newSize { 0u, 0u };

      private:

        std::shared_ptr<EventType> type = std::make_shared<InvalidEvent>();
    };

    /**
     * @class EventManager
     * @brief Manages input events from the console and provides event handling capabilities.
     * 
     * The EventManager class acts as an interface between the Console and application code,
     * polling for input events, maintaining an event queue, and providing flexible event
     * handling through type-specific callbacks. It supports keyboard and mouse events,
     * as well as console resize events.
     */
    class EventManager
    {
    public:

        EventManager() = default;

        EventManager(const EventManager&) = delete;
        EventManager& operator=(const EventManager&) = delete;

        /**
         * @brief Polls for the next available event
         * 
         * Retrieves and removes the next event from the internal event queue. If no events
         * are available, it will attempt to fetch new events from the console.
         * 
         * @return Optional containing the next Event if available, std::nullopt if no events
         * 
         * @note This method consumes events from the queue
         */
        std::optional<Event> pollEvent();

        /**
         * @brief Processes events using multiple type-specific callbacks
         * 
         * This is the main event processing method that allows you to register multiple
         * callbacks for different event types. Each callback will only be called for
         * events it can handle based on its signature.
         * 
         * Supported callback signatures:
         * - (KeyPressEvent, const Event&) -> void
         * - (KeyReleaseEvent, const Event&) -> void  
         * - (MouseMoveEvent, const Event&) -> void
         * - (MouseScrollEvent, const Event&) -> void
         * - (ConsoleEvent, const Event&) -> void
         * - (InvalidEvent, const Event&) -> void
         * 
         * @tparam Callbacks Variadic template for multiple callback types
         * @param callbacks One or more callback functions/lambdas to process events
         * 
         * @note Events are automatically removed from the queue after being processed
         * @note Each event can be handled by multiple callbacks if their signatures match
         * 
         * Example usage:
         * @code
         * eventManager.handleEvents(
         *     [](KeyPressEvent, const Event& e) { 
         *         std::cout << "Key pressed: " << static_cast<int>(e.key) << std::endl; 
         *     },
         *     [](MouseMoveEvent, const Event& e) { 
         *         std::cout << "Mouse moved to: " << e.mousePosition.x << ", " << e.mousePosition.y << std::endl; 
         *     }
         * );
         * @endcode
         */
        template<typename... Callbacks>
        void handleEvents(Callbacks&&... callbacks);

        /**
         * @brief Discards all events in the event queue
         * 
         * This method clears the internal event queue, removing all pending events.
         * It can be useful to reset the event state without processing them.
         */
        void discardEvents();

        std::vector<Event> &getEvents();

        const std::vector<Event> &peekEvents() const;

    private:

        std::vector<Event> m_events;
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

    template<typename... Callbacks>
    void EventManager::handleEvents(Callbacks&&... callbacks) {    
        std::vector<bool> handledEvents(m_events.size(), false);
        
        auto processCallback = [this, &handledEvents](auto&& callback) {
            for (size_t i = 0; i < m_events.size(); ++i) {
                const auto& event = m_events[i];
                
                if constexpr (std::is_invocable_v<decltype(callback), KeyPressEvent, const Event&>) {
                    if (event.isOfType<KeyPressEvent>()) {
                        callback(KeyPressEvent{}, event);
                        handledEvents[i] = true;
                    }
                }
                if constexpr (std::is_invocable_v<decltype(callback), KeyReleaseEvent, const Event&>) {
                    if (event.isOfType<KeyReleaseEvent>()) {
                        callback(KeyReleaseEvent{}, event);
                        handledEvents[i] = true;
                    }
                }
                if constexpr (std::is_invocable_v<decltype(callback), MouseMoveEvent, const Event&>) {
                    if (event.isOfType<MouseMoveEvent>()) {
                        callback(MouseMoveEvent{}, event);
                        handledEvents[i] = true;
                    }
                }
                if constexpr (std::is_invocable_v<decltype(callback), MouseScrollEvent, const Event&>) {
                    if (event.isOfType<MouseScrollEvent>()) {
                        callback(MouseScrollEvent{}, event);
                        handledEvents[i] = true;
                    }
                }
                if constexpr (std::is_invocable_v<decltype(callback), ConsoleEvent, const Event&>) {
                    if (event.isOfType<ConsoleEvent>()) {
                        callback(ConsoleEvent{}, event);
                        handledEvents[i] = true;
                    }
                }
                if constexpr (std::is_invocable_v<decltype(callback), InvalidEvent, const Event&>) {
                    if (event.isOfType<InvalidEvent>()) {
                        callback(InvalidEvent{}, event);
                        handledEvents[i] = true;
                    }
                }
            }
        };
        
        (processCallback(callbacks), ...);

        for (int i = static_cast<int>(handledEvents.size()) - 1; i >= 0; --i) {
            if (handledEvents[i]) {
                m_events.erase(m_events.begin() + i);
            }
        }
    }
}

#endif // CGL_EVENTS_HPP