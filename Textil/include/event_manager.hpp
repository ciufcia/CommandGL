/**
 * @file event_manager.hpp
 * @brief Event queue management and processing for Textil input system
 * @details Provides centralized event management with polling, callback-based handling,
 *          and type-safe event processing. Manages the event queue lifecycle and
 *          provides flexible event consumption patterns for applications.
 */

#ifndef TIL_EVENT_MANAGER_HPP
#define TIL_EVENT_MANAGER_HPP

#include <vector>
#include <optional>
#include "event.hpp"

namespace til
{
    /**
     * @brief Central event queue manager for processing input events
     * @details The EventManager class provides a centralized system for managing
     *          input events from various sources (keyboard, mouse, console). It offers
     *          multiple consumption patterns:
     *          - Polling: Individual event retrieval with pollEvent()
     *          - Callback-based: Automatic event dispatch to type-specific handlers
     *          - Inspection: Non-consuming event queue examination
     * 
     *          Events are processed in FIFO order and can be consumed by applications
     *          or handled automatically through callback registration. The manager
     *          tracks processing state and provides efficient event removal after handling.
     * 
     *          Thread safety: This class is not thread-safe and should be used from
     *          a single thread (typically the main application thread).
     */
    class EventManager
    {
    public:
        /**
         * @brief Default constructor creating empty event manager
         * @details Initializes event manager with empty queue ready to receive events.
         */
        EventManager() = default;

        /**
         * @brief Deleted copy constructor to prevent copying
         * @details EventManager manages unique event state and cannot be safely copied.
         */
        EventManager(const EventManager&) = delete;
        
        /**
         * @brief Deleted copy assignment operator
         * @details EventManager manages unique event state and cannot be safely copied.
         */
        EventManager& operator=(const EventManager&) = delete;

        /**
         * @brief Poll and consume next event from queue
         * @return Optional containing next event, or empty if no events available
         * @details Retrieves and removes the next event from the front of the queue.
         *          Returns std::nullopt when no events are pending. This provides
         *          manual event consumption for applications that need direct control
         *          over event processing timing.
         * 
         *          Example usage:
         *          ```cpp
         *          while (auto event = eventManager.pollEvent()) {
         *              if (event->isOfType<KeyPressEvent>()) {
         *                  handleKeyPress(event->key);
         *              }
         *          }
         *          ```
         */
        std::optional<Event> pollEvent();
        
        /**
         * @brief Process events using type-specific callback functions
         * @tparam Callbacks Variadic template for callback function types
         * @param callbacks Functions to handle specific event types
         * @details Advanced event processing system that automatically dispatches
         *          events to appropriate callback functions based on event type.
         *          Callbacks are invoked with the event type marker and event data.
         *          Processed events are automatically removed from the queue.
         * 
         *          Supported callback signatures:
         *          - `void callback(KeyPressEvent, const Event&)`
         *          - `void callback(KeyReleaseEvent, const Event&)`
         *          - `void callback(MouseMoveEvent, const Event&)`
         *          - `void callback(MouseScrollEvent, const Event&)`
         *          - `void callback(ConsoleEvent, const Event&)`
         *          - `void callback(InvalidEvent, const Event&)`
         * 
         *          Example usage:
         *          ```cpp
         *          eventManager.handleEvents(
         *              [](KeyPressEvent, const Event& e) { 
         *                  std::cout << "Key pressed: " << static_cast<int>(e.key) << std::endl;
         *              },
         *              [](MouseMoveEvent, const Event& e) {
         *                  std::cout << "Mouse moved: " << e.mouseDelta.x << ", " << e.mouseDelta.y << std::endl;
         *              }
         *          );
         *          ```
         */
        template<typename... Callbacks>
        void handleEvents(Callbacks&&... callbacks);
        
        /**
         * @brief Clear all pending events from queue
         * @details Removes all events from the queue without processing them.
         *          Useful for discarding accumulated events during state transitions
         *          or when events become irrelevant (e.g., menu transitions).
         *          Resets internal processing state.
         */
        void discardEvents();

        /**
         * @brief Get read-only access to event queue without consuming
         * @return Const reference to internal event vector
         * @details Provides inspection access to pending events without removing them.
         *          Useful for debugging, event count monitoring, or implementing
         *          custom event filtering logic. Events remain in queue after inspection.
         */
        const std::vector<Event> &peekEvents() const;

    private:
        u32 m_currentEventIndex = 0; ///< Current position in event queue for polling
        std::vector<Event> m_events; ///< Event storage queue in FIFO order

    friend class Framework; ///< Framework needs access for event injection
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

        u32 destinationIndex = 0;
        for (u32 i = 0; i < handledEvents.size(); ++i) {
            if (!handledEvents[i]) {
                m_events[destinationIndex++] = std::move(m_events[i]);
            } else {
                if (m_currentEventIndex > i) {
                    --m_currentEventIndex;
                }
            }
        }
        m_events.resize(destinationIndex);
    }
}

#endif // TIL_EVENT_MANAGER_HPP