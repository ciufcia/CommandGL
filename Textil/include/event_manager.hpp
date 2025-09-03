#ifndef TIL_EVENT_MANAGER_HPP
#define TIL_EVENT_MANAGER_HPP

#include <vector>
#include <optional>
#include "event.hpp"

namespace til
{
    class EventManager
    {
    public:

    EventManager() = default;

    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    std::optional<Event> pollEvent();
    template<typename... Callbacks>
    void handleEvents(Callbacks&&... callbacks);
    void discardEvents();

    const std::vector<Event> &peekEvents() const;

    private:

        u32 m_currentEventIndex = 0;

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