#ifndef CGL_EVENT_MANAGER_HPP
#define CGL_EVENT_MANAGER_HPP

#include <optional>
#include <functional>
#include <type_traits>
#include "console.hpp"

namespace cgl
{
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

        EventManager(const EventManager&) = delete;
        EventManager& operator=(const EventManager&) = delete;

        /**
         * @brief Checks if a specific key is currently pressed
         * 
         * @param key The KeyCode to check for pressed state
         * @return true if the key is currently pressed, false otherwise
         * 
         * @note This reflects the current state, not just events in the queue
         */
        bool isKeyPressed(KeyCode key) const;

        /**
         * @brief Gets the current state of all keys
         * 
         * @return Const reference to an array of boolean values representing key states
         *         Index corresponds to KeyCode enum values, true = pressed, false = released
         */
        const std::array<bool, static_cast<size_t>(KeyCode::Count)> &getKeyStates() const;

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

    private:

        EventManager(Console& console) : m_console(console) {}

        void updateEvents();

        const std::vector<Event> &peekEvents() const;

    private:

        Console &m_console;

        std::vector<Event> m_events;

    friend class Framework;
    };    
    
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

#endif // CGL_EVENT_MANAGER_HPP