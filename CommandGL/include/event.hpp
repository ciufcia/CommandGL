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

const std::unordered_map<KeyCode, u32> keyCodeToWinapiVK = {
    { KeyCode::LeftMouseButton, VK_LBUTTON },
    { KeyCode::RightMouseButton, VK_RBUTTON },
    { KeyCode::MiddleMouseButton, VK_MBUTTON },
    { KeyCode::BackSpace, VK_BACK },
    { KeyCode::Tab, VK_TAB },
    { KeyCode::Clear, VK_CLEAR },
    { KeyCode::Enter, VK_RETURN },
    { KeyCode::Shift, VK_SHIFT },
    { KeyCode::Ctrl, VK_CONTROL },
    { KeyCode::Alt, VK_MENU },
    { KeyCode::Pause, VK_PAUSE },
    { KeyCode::CapsLock, VK_CAPITAL },
    { KeyCode::Spacebar, VK_SPACE },
    { KeyCode::PageUp, VK_PRIOR },
    { KeyCode::PageDown, VK_NEXT },
    { KeyCode::End, VK_END },
    { KeyCode::Home, VK_HOME },
    { KeyCode::Select, VK_SELECT },
    { KeyCode::Print, VK_PRINT },
    { KeyCode::Execute, VK_EXECUTE },
    { KeyCode::PrintScreen, VK_SNAPSHOT },
    { KeyCode::Insert, VK_INSERT },
    { KeyCode::Delete, VK_DELETE },
    { KeyCode::Help, VK_HELP },
    { KeyCode::Multiply, VK_MULTIPLY },
    { KeyCode::Add, VK_ADD },
    { KeyCode::Separator, VK_SEPARATOR },
    { KeyCode::Subtract, VK_SUBTRACT },
    { KeyCode::Decimal, VK_DECIMAL },
    { KeyCode::Divide, VK_DIVIDE },
    { KeyCode::NumLock, VK_NUMLOCK },
    { KeyCode::ScrollLock, VK_SCROLL },
    { KeyCode::LeftShift, VK_LSHIFT },
    { KeyCode::RightShift, VK_RSHIFT },
    { KeyCode::LeftControl, VK_LCONTROL },
    { KeyCode::RightControl, VK_RCONTROL },
    { KeyCode::LeftAlt, VK_LMENU },
    { KeyCode::RightAlt, VK_RMENU },
    { KeyCode::VolumeMute, VK_VOLUME_MUTE },
    { KeyCode::VolumeDown, VK_VOLUME_DOWN },
    { KeyCode::VolumeUp, VK_VOLUME_UP },
    { KeyCode::NextTrack, VK_MEDIA_NEXT_TRACK },
    { KeyCode::PreviousTrack, VK_MEDIA_PREV_TRACK },
    { KeyCode::Stop, VK_MEDIA_STOP },
    { KeyCode::PlayPause, VK_MEDIA_PLAY_PAUSE },
    { KeyCode::Semicolon, VK_OEM_1 },
    { KeyCode::Plus, VK_OEM_PLUS },
    { KeyCode::Comma, VK_OEM_COMMA },
    { KeyCode::Minus, VK_OEM_MINUS },
    { KeyCode::Period, VK_OEM_PERIOD },
    { KeyCode::ForwardSlash, VK_OEM_2 },
    { KeyCode::Backquote, VK_OEM_3 },
    { KeyCode::SquareBracketOpen, VK_OEM_4 },
    { KeyCode::BackwardSlash, VK_OEM_5 },
    { KeyCode::SquareBracketClose, VK_OEM_6 },
    { KeyCode::Apostrophe, VK_OEM_7 },
    { KeyCode::Escape, VK_ESCAPE },

    { KeyCode::Numpad0, VK_NUMPAD0 },
    { KeyCode::Numpad1, VK_NUMPAD1 },
    { KeyCode::Numpad2, VK_NUMPAD2 },
    { KeyCode::Numpad3, VK_NUMPAD3 },
    { KeyCode::Numpad4, VK_NUMPAD4 },
    { KeyCode::Numpad5, VK_NUMPAD5 },
    { KeyCode::Numpad6, VK_NUMPAD6 },
    { KeyCode::Numpad7, VK_NUMPAD7 },
    { KeyCode::Numpad8, VK_NUMPAD8 },
    { KeyCode::Numpad9, VK_NUMPAD9 },

    { KeyCode::Left, VK_LEFT },
    { KeyCode::Up, VK_UP },
    { KeyCode::Right, VK_RIGHT },
    { KeyCode::Down, VK_DOWN },

    { KeyCode::Zero, '0' },
    { KeyCode::One, '1' },
    { KeyCode::Two, '2' },
    { KeyCode::Three, '3' },
    { KeyCode::Four, '4' },
    { KeyCode::Five, '5' },
    { KeyCode::Six, '6' },
    { KeyCode::Seven, '7' },
    { KeyCode::Eight, '8' },
    { KeyCode::Nine, '9' },

    { KeyCode::A, 'A' },
    { KeyCode::B, 'B' },
    { KeyCode::C, 'C' },
    { KeyCode::D, 'D' },
    { KeyCode::E, 'E' },
    { KeyCode::F, 'F' },
    { KeyCode::G, 'G' },
    { KeyCode::H, 'H' },
    { KeyCode::I, 'I' },
    { KeyCode::J, 'J' },
    { KeyCode::K, 'K' },
    { KeyCode::L, 'L' },
    { KeyCode::M, 'M' },
    { KeyCode::N, 'N' },
    { KeyCode::O, 'O' },
    { KeyCode::P, 'P' },
    { KeyCode::Q, 'Q' },
    { KeyCode::R, 'R' },
    { KeyCode::S, 'S' },
    { KeyCode::T, 'T' },
    { KeyCode::U, 'U' },
    { KeyCode::V, 'V' },
    { KeyCode::W, 'W' },
    { KeyCode::X, 'X' },
    { KeyCode::Y, 'Y' },
    { KeyCode::Z, 'Z' },

    { KeyCode::F1, VK_F1 },
    { KeyCode::F2, VK_F2 },
    { KeyCode::F3, VK_F3 },
    { KeyCode::F4, VK_F4 },
    { KeyCode::F5, VK_F5 },
    { KeyCode::F6, VK_F6 },
    { KeyCode::F7, VK_F7 },
    { KeyCode::F8, VK_F8 },
    { KeyCode::F9, VK_F9 },
    { KeyCode::F10, VK_F10 },
    { KeyCode::F11, VK_F11 },
    { KeyCode::F12, VK_F12 },
    { KeyCode::F13, VK_F13 },
    { KeyCode::F14, VK_F14 },
    { KeyCode::F15, VK_F15 },
    { KeyCode::F16, VK_F16 },
    { KeyCode::F17, VK_F17 },
    { KeyCode::F18, VK_F18 },
    { KeyCode::F19, VK_F19 },
    { KeyCode::F20, VK_F20 },
    { KeyCode::F21, VK_F21 },
    { KeyCode::F22, VK_F22 },
    { KeyCode::F23, VK_F23 },
    { KeyCode::F24, VK_F24 }
};

int getWinapiVK(KeyCode key);

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