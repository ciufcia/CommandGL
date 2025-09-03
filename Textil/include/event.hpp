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