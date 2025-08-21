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
    struct EventType { virtual ~EventType() = default; };

    struct InvalidEvent : EventType {};

    struct KeyPressEvent : EventType {};

    struct KeyReleaseEvent : EventType {};

    struct MouseMoveEvent : EventType {};

    struct MouseScrollEvent : EventType {};

    struct ConsoleEvent : EventType {};
    class Event
    {
    public:
        template<typename T>
        requires std::derived_from<T, EventType>
        bool isOfType() const;
        template<typename T>
        requires std::derived_from<T, EventType>
        void setType();
        KeyCode key { KeyCode::Invalid };

        Vector2<i32> mouseDelta { 0u, 0u };

        i8 mouseScrollDelta { 0u };

        Vector2<u32> newSize { 0u, 0u };

      private:

        std::shared_ptr<EventType> type = std::make_shared<InvalidEvent>();
    };

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

#endif // TIL_EVENTS_HPP