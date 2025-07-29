#ifndef CGL_EVENT_HPP
#define CGL_EVENT_HPP

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32
#include <memory>
#include <unordered_map>

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

    enum class KeyCode
    {
        Invalid,

        LeftMouseButton,
        RightMouseButton,
        MiddleMouseButton,

        BackSpace,
        Tab,
        Clear,
        Enter,
        Shift,
        Ctrl,
        Alt,
        Pause,
        CapsLock,
        Spacebar,
        PageUp,
        PageDown,
        End,
        Home,
        Select,
        Print,
        Execute,
        PrintScreen,
        Insert,
        Delete,
        Help,
        Multiply,
        Add,
        Separator,
        Subtract,
        Decimal,
        Divide,
        NumLock,
        ScrollLock,
        LeftShift,
        RightShift,
        LeftControl,
        RightControl,
        LeftAlt,
        RightAlt,
        VolumeMute,
        VolumeDown,
        VolumeUp,
        NextTrack,
        PreviousTrack,
        Stop,
        PlayPause,
        Semicolon,
        Plus,
        Comma,
        Minus,
        Period,
        ForwardSlash,
        Backquote,
        SquareBracketOpen,
        BackwardSlash,
        SquareBracketClose,
        Apostrophe,
        Escape,

        Numpad0,
        Numpad1,
        Numpad2,
        Numpad3,
        Numpad4,
        Numpad5,
        Numpad6,
        Numpad7,
        Numpad8,
        Numpad9,

        Left,
        Up,
        Right,
        Down,

        Zero,
        One,
        Two,
        Three,
        Four,
        Five,
        Six,
        Seven,
        Eight,
        Nine,

        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,

        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,

        Count
    };

#ifdef _WIN32

const std::unordered_map<u32, KeyCode> WinapiVKToKeyCode = {
    { VK_LBUTTON, KeyCode::LeftMouseButton },
    { VK_RBUTTON, KeyCode::RightMouseButton },
    { VK_MBUTTON, KeyCode::MiddleMouseButton },
    { VK_BACK, KeyCode::BackSpace },
    { VK_TAB, KeyCode::Tab },
    { VK_CLEAR, KeyCode::Clear },
    { VK_RETURN, KeyCode::Enter },
    { VK_SHIFT, KeyCode::Shift },
    { VK_CONTROL, KeyCode::Ctrl },
    { VK_MENU, KeyCode::Alt },
    { VK_PAUSE, KeyCode::Pause },
    { VK_CAPITAL, KeyCode::CapsLock },
    { VK_SPACE, KeyCode::Spacebar },
    { VK_PRIOR, KeyCode::PageUp },
    { VK_NEXT, KeyCode::PageDown },
    { VK_END, KeyCode::End },
    { VK_HOME, KeyCode::Home },
    { VK_SELECT, KeyCode::Select },
    { VK_PRINT, KeyCode::Print },
    { VK_EXECUTE, KeyCode::Execute },
    { VK_SNAPSHOT, KeyCode::PrintScreen },
    { VK_INSERT, KeyCode::Insert },
    { VK_DELETE, KeyCode::Delete },
    { VK_HELP, KeyCode::Help },
    { VK_MULTIPLY, KeyCode::Multiply },
    { VK_ADD, KeyCode::Add },
    { VK_SEPARATOR, KeyCode::Separator },
    { VK_SUBTRACT, KeyCode::Subtract },
    { VK_DECIMAL, KeyCode::Decimal },
    { VK_DIVIDE, KeyCode::Divide },
    { VK_NUMLOCK, KeyCode::NumLock },
    { VK_SCROLL, KeyCode::ScrollLock },
    { VK_LSHIFT, KeyCode::LeftShift },
    { VK_RSHIFT, KeyCode::RightShift },
    { VK_LCONTROL, KeyCode::LeftControl },
    { VK_RCONTROL, KeyCode::RightControl },
    { VK_LMENU, KeyCode::LeftAlt },
    { VK_RMENU, KeyCode::RightAlt },
    { VK_VOLUME_MUTE, KeyCode::VolumeMute },
    { VK_VOLUME_DOWN, KeyCode::VolumeDown },
    { VK_VOLUME_UP, KeyCode::VolumeUp },
    { VK_MEDIA_NEXT_TRACK, KeyCode::NextTrack },
    { VK_MEDIA_PREV_TRACK, KeyCode::PreviousTrack },
    { VK_MEDIA_STOP, KeyCode::Stop },
    { VK_MEDIA_PLAY_PAUSE, KeyCode::PlayPause },
    { VK_OEM_1, KeyCode::Semicolon },
    { VK_OEM_PLUS, KeyCode::Plus },
    { VK_OEM_COMMA, KeyCode::Comma },
    { VK_OEM_MINUS, KeyCode::Minus },
    { VK_OEM_PERIOD, KeyCode::Period },
    { VK_OEM_2, KeyCode::ForwardSlash },
    { VK_OEM_3, KeyCode::Backquote },
    { VK_OEM_4, KeyCode::SquareBracketOpen },
    { VK_OEM_5, KeyCode::BackwardSlash },
    { VK_OEM_6, KeyCode::SquareBracketClose },
    { VK_OEM_7, KeyCode::Apostrophe },
    { VK_ESCAPE, KeyCode::Escape },

    { VK_NUMPAD0, KeyCode::Numpad0 },
    { VK_NUMPAD1, KeyCode::Numpad1 },
    { VK_NUMPAD2, KeyCode::Numpad2 },
    { VK_NUMPAD3, KeyCode::Numpad3 },
    { VK_NUMPAD4, KeyCode::Numpad4 },
    { VK_NUMPAD5, KeyCode::Numpad5 },
    { VK_NUMPAD6, KeyCode::Numpad6 },
    { VK_NUMPAD7, KeyCode::Numpad7 },
    { VK_NUMPAD8, KeyCode::Numpad8 },
    { VK_NUMPAD9, KeyCode::Numpad9 },

    { VK_LEFT, KeyCode::Left },
    { VK_UP, KeyCode::Up },
    { VK_RIGHT, KeyCode::Right },
    { VK_DOWN, KeyCode::Down },

    { '0', KeyCode::Zero },
    { '1', KeyCode::One },
    { '2', KeyCode::Two },
    { '3', KeyCode::Three },
    { '4', KeyCode::Four },
    { '5', KeyCode::Five },
    { '6', KeyCode::Six },
    { '7', KeyCode::Seven },
    { '8', KeyCode::Eight },
    { '9', KeyCode::Nine },

    { 'A', KeyCode::A },
    { 'B', KeyCode::B },
    { 'C', KeyCode::C },
    { 'D', KeyCode::D },
    { 'E', KeyCode::E },
    { 'F', KeyCode::F },
    { 'G', KeyCode::G },
    { 'H', KeyCode::H },
    { 'I', KeyCode::I },
    { 'J', KeyCode::J },
    { 'K', KeyCode::K },
    { 'L', KeyCode::L },
    { 'M', KeyCode::M },
    { 'N', KeyCode::N },
    { 'O', KeyCode::O },
    { 'P', KeyCode::P },
    { 'Q', KeyCode::Q },
    { 'R', KeyCode::R },
    { 'S', KeyCode::S },
    { 'T', KeyCode::T },
    { 'U', KeyCode::U },
    { 'V', KeyCode::V },
    { 'W', KeyCode::W },
    { 'X', KeyCode::X },
    { 'Y', KeyCode::Y },
    { 'Z', KeyCode::Z },

    { VK_F1, KeyCode::F1 },
    { VK_F2, KeyCode::F2 },
    { VK_F3, KeyCode::F3 },
    { VK_F4, KeyCode::F4 },
    { VK_F5, KeyCode::F5 },
    { VK_F6, KeyCode::F6 },
    { VK_F7, KeyCode::F7 },
    { VK_F8, KeyCode::F8 },
    { VK_F9, KeyCode::F9 },
    { VK_F10, KeyCode::F10 },
    { VK_F11, KeyCode::F11 },
    { VK_F12, KeyCode::F12 },
    { VK_F13, KeyCode::F13 },
    { VK_F14, KeyCode::F14 },
    { VK_F15, KeyCode::F15 },
    { VK_F16, KeyCode::F16 },
    { VK_F17, KeyCode::F17 },
    { VK_F18, KeyCode::F18 },
    { VK_F19, KeyCode::F19 },
    { VK_F20, KeyCode::F20 },
    { VK_F21, KeyCode::F21 },
    { VK_F22, KeyCode::F22 },
    { VK_F23, KeyCode::F23 },
    { VK_F24, KeyCode::F24 },
};

KeyCode winapiVKToKeyCode(u32 vk) {
    auto it = WinapiVKToKeyCode.find(vk);
    if (it != WinapiVKToKeyCode.end()) {
        return it->second;
    }
    return KeyCode::Invalid;
}

#endif // _WIN32

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
         * @brief The current mouse cursor position in console character coordinates.
         * 
         * This field is valid for MouseMoveEvent and other mouse-related events.
         */
        Vector2<u32> mousePosition { 0u, 0u };
        
        /**
         * @brief The change in mouse position since the last mouse event.
         * 
         * This field is valid for MouseMoveEvent and represents the movement delta.
         */
        Vector2<u32> mouseDelta { 0u, 0u };
        
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

#endif // CGL_EVENT_HPP