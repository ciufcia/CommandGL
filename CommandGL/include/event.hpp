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

#ifdef __linux__

const std::unordered_map<KeyCode, int> keyCodeToLinuxKey = {
    { KeyCode::LeftMouseButton, BTN_LEFT },
    { KeyCode::RightMouseButton, BTN_RIGHT },
    { KeyCode::MiddleMouseButton, BTN_MIDDLE },
    { KeyCode::BackSpace, KEY_BACKSPACE },
    { KeyCode::Tab, KEY_TAB },
    { KeyCode::Clear, KEY_CLEAR },
    { KeyCode::Enter, KEY_ENTER },
    { KeyCode::Shift, KEY_LEFTSHIFT },
    { KeyCode::Ctrl, KEY_LEFTCTRL },
    { KeyCode::Alt, KEY_LEFTALT },
    { KeyCode::Pause, KEY_PAUSE },
    { KeyCode::CapsLock, KEY_CAPSLOCK },
    { KeyCode::Spacebar, KEY_SPACE },
    { KeyCode::PageUp, KEY_PAGEUP },
    { KeyCode::PageDown, KEY_PAGEDOWN },
    { KeyCode::End, KEY_END },
    { KeyCode::Home, KEY_HOME },
    { KeyCode::Select, KEY_SELECT },
    { KeyCode::Print, KEY_PRINT },
    { KeyCode::PrintScreen, KEY_SYSRQ },
    { KeyCode::Insert, KEY_INSERT },
    { KeyCode::Delete, KEY_DELETE },
    { KeyCode::Help, KEY_HELP },
    { KeyCode::Multiply, KEY_KPASTERISK },
    { KeyCode::Add, KEY_KPPLUS },
    { KeyCode::Separator, KEY_KPENTER },
    { KeyCode::Subtract, KEY_KPMINUS },
    { KeyCode::Decimal, KEY_KPDOT },
    { KeyCode::Divide, KEY_KPSLASH },
    { KeyCode::NumLock, KEY_NUMLOCK },
    { KeyCode::ScrollLock, KEY_SCROLLLOCK },
    { KeyCode::LeftShift, KEY_LEFTSHIFT },
    { KeyCode::RightShift, KEY_RIGHTSHIFT },
    { KeyCode::LeftControl, KEY_LEFTCTRL },
    { KeyCode::RightControl, KEY_RIGHTCTRL },
    { KeyCode::LeftAlt, KEY_LEFTALT },
    { KeyCode::RightAlt, KEY_RIGHTALT },
    { KeyCode::VolumeMute, KEY_MUTE },
    { KeyCode::VolumeDown, KEY_VOLUMEDOWN },
    { KeyCode::VolumeUp, KEY_VOLUMEUP },
    { KeyCode::NextTrack, KEY_NEXTSONG },
    { KeyCode::PreviousTrack, KEY_PREVIOUSSONG },
    { KeyCode::Stop, KEY_STOPCD },
    { KeyCode::PlayPause, KEY_PLAYPAUSE },
    { KeyCode::Semicolon, KEY_SEMICOLON },
    { KeyCode::Plus, KEY_EQUAL },
    { KeyCode::Comma, KEY_COMMA },
    { KeyCode::Minus, KEY_MINUS },
    { KeyCode::Period, KEY_DOT },
    { KeyCode::ForwardSlash, KEY_SLASH },
    { KeyCode::Backquote, KEY_GRAVE },
    { KeyCode::SquareBracketOpen, KEY_LEFTBRACE },
    { KeyCode::BackwardSlash, KEY_BACKSLASH },
    { KeyCode::SquareBracketClose, KEY_RIGHTBRACE },
    { KeyCode::Apostrophe, KEY_APOSTROPHE },
    { KeyCode::Escape, KEY_ESC },

    { KeyCode::Numpad0, KEY_KP0 },
    { KeyCode::Numpad1, KEY_KP1 },
    { KeyCode::Numpad2, KEY_KP2 },
    { KeyCode::Numpad3, KEY_KP3 },
    { KeyCode::Numpad4, KEY_KP4 },
    { KeyCode::Numpad5, KEY_KP5 },
    { KeyCode::Numpad6, KEY_KP6 },
    { KeyCode::Numpad7, KEY_KP7 },
    { KeyCode::Numpad8, KEY_KP8 },
    { KeyCode::Numpad9, KEY_KP9 },

    { KeyCode::Left, KEY_LEFT },
    { KeyCode::Up, KEY_UP },
    { KeyCode::Right, KEY_RIGHT },
    { KeyCode::Down, KEY_DOWN },

    { KeyCode::Zero, KEY_0 },
    { KeyCode::One, KEY_1 },
    { KeyCode::Two, KEY_2 },
    { KeyCode::Three, KEY_3 },
    { KeyCode::Four, KEY_4 },
    { KeyCode::Five, KEY_5 },
    { KeyCode::Six, KEY_6 },
    { KeyCode::Seven, KEY_7 },
    { KeyCode::Eight, KEY_8 },
    { KeyCode::Nine, KEY_9 },

    { KeyCode::A, KEY_A },
    { KeyCode::B, KEY_B },
    { KeyCode::C, KEY_C },
    { KeyCode::D, KEY_D },
    { KeyCode::E, KEY_E },
    { KeyCode::F, KEY_F },
    { KeyCode::G, KEY_G },
    { KeyCode::H, KEY_H },
    { KeyCode::I, KEY_I },
    { KeyCode::J, KEY_J },
    { KeyCode::K, KEY_K },
    { KeyCode::L, KEY_L },
    { KeyCode::M, KEY_M },
    { KeyCode::N, KEY_N },
    { KeyCode::O, KEY_O },
    { KeyCode::P, KEY_P },
    { KeyCode::Q, KEY_Q },
    { KeyCode::R, KEY_R },
    { KeyCode::S, KEY_S },
    { KeyCode::T, KEY_T },
    { KeyCode::U, KEY_U },
    { KeyCode::V, KEY_V },
    { KeyCode::W, KEY_W },
    { KeyCode::X, KEY_X },
    { KeyCode::Y, KEY_Y },
    { KeyCode::Z, KEY_Z },

    { KeyCode::F1, KEY_F1 },
    { KeyCode::F2, KEY_F2 },
    { KeyCode::F3, KEY_F3 },
    { KeyCode::F4, KEY_F4 },
    { KeyCode::F5, KEY_F5 },
    { KeyCode::F6, KEY_F6 },
    { KeyCode::F7, KEY_F7 },
    { KeyCode::F8, KEY_F8 },
    { KeyCode::F9, KEY_F9 },
    { KeyCode::F10, KEY_F10 },
    { KeyCode::F11, KEY_F11 },
    { KeyCode::F12, KEY_F12 },
    { KeyCode::F13, KEY_F13 },
    { KeyCode::F14, KEY_F14 },
    { KeyCode::F15, KEY_F15 },
    { KeyCode::F16, KEY_F16 },
    { KeyCode::F17, KEY_F17 },
    { KeyCode::F18, KEY_F18 },
    { KeyCode::F19, KEY_F19 },
    { KeyCode::F20, KEY_F20 },
    { KeyCode::F21, KEY_F21 },
    { KeyCode::F22, KEY_F22 },
    { KeyCode::F23, KEY_F23 },
    { KeyCode::F24, KEY_F24 }
};

int getLinuxKey(KeyCode key);

const std::unordered_map<int, KeyCode> linuxKeyToKeyCode = {
    { BTN_LEFT, KeyCode::LeftMouseButton },
    { BTN_RIGHT, KeyCode::RightMouseButton },
    { BTN_MIDDLE, KeyCode::MiddleMouseButton },
    { KEY_BACKSPACE, KeyCode::BackSpace },
    { KEY_TAB, KeyCode::Tab },
    { KEY_CLEAR, KeyCode::Clear },
    { KEY_ENTER, KeyCode::Enter },
    { KEY_LEFTSHIFT, KeyCode::LeftShift },
    { KEY_RIGHTSHIFT, KeyCode::RightShift },
    { KEY_LEFTCTRL, KeyCode::LeftControl },
    { KEY_RIGHTCTRL, KeyCode::RightControl },
    { KEY_LEFTALT, KeyCode::LeftAlt },
    { KEY_RIGHTALT, KeyCode::RightAlt },
    { KEY_PAUSE, KeyCode::Pause },
    { KEY_CAPSLOCK, KeyCode::CapsLock },
    { KEY_SPACE, KeyCode::Spacebar },
    { KEY_PAGEUP, KeyCode::PageUp },
    { KEY_PAGEDOWN, KeyCode::PageDown },
    { KEY_END, KeyCode::End },
    { KEY_HOME, KeyCode::Home },
    { KEY_SELECT, KeyCode::Select },
    { KEY_PRINT, KeyCode::Print },
    { KEY_SYSRQ, KeyCode::PrintScreen },
    { KEY_INSERT, KeyCode::Insert },
    { KEY_DELETE, KeyCode::Delete },
    { KEY_HELP, KeyCode::Help },
    { KEY_KPASTERISK, KeyCode::Multiply },
    { KEY_KPPLUS, KeyCode::Add },
    { KEY_KPENTER, KeyCode::Separator },
    { KEY_KPMINUS, KeyCode::Subtract },
    { KEY_KPDOT, KeyCode::Decimal },
    { KEY_KPSLASH, KeyCode::Divide },
    { KEY_NUMLOCK, KeyCode::NumLock },
    { KEY_SCROLLLOCK, KeyCode::ScrollLock },
    { KEY_MUTE, KeyCode::VolumeMute },
    { KEY_VOLUMEDOWN, KeyCode::VolumeDown },
    { KEY_VOLUMEUP, KeyCode::VolumeUp },
    { KEY_NEXTSONG, KeyCode::NextTrack },
    { KEY_PREVIOUSSONG, KeyCode::PreviousTrack },
    { KEY_STOPCD, KeyCode::Stop },
    { KEY_PLAYPAUSE, KeyCode::PlayPause },
    { KEY_SEMICOLON, KeyCode::Semicolon },
    { KEY_EQUAL, KeyCode::Plus },
    { KEY_COMMA, KeyCode::Comma },
    { KEY_MINUS, KeyCode::Minus },
    { KEY_DOT, KeyCode::Period },
    { KEY_SLASH, KeyCode::ForwardSlash },
    { KEY_GRAVE, KeyCode::Backquote },
    { KEY_LEFTBRACE, KeyCode::SquareBracketOpen },
    { KEY_BACKSLASH, KeyCode::BackwardSlash },
    { KEY_RIGHTBRACE, KeyCode::SquareBracketClose },
    { KEY_APOSTROPHE, KeyCode::Apostrophe },
    { KEY_ESC, KeyCode::Escape },
    { KEY_KP0, KeyCode::Numpad0 },
    { KEY_KP1, KeyCode::Numpad1 },
    { KEY_KP2, KeyCode::Numpad2 },
    { KEY_KP3, KeyCode::Numpad3 },
    { KEY_KP4, KeyCode::Numpad4 },
    { KEY_KP5, KeyCode::Numpad5 },
    { KEY_KP6, KeyCode::Numpad6 },
    { KEY_KP7, KeyCode::Numpad7 },
    { KEY_KP8, KeyCode::Numpad8 },
    { KEY_KP9, KeyCode::Numpad9 },
    { KEY_LEFT, KeyCode::Left },
    { KEY_UP, KeyCode::Up },
    { KEY_RIGHT, KeyCode::Right },
    { KEY_DOWN, KeyCode::Down },
    { KEY_0, KeyCode::Zero },
    { KEY_1, KeyCode::One },
    { KEY_2, KeyCode::Two },
    { KEY_3, KeyCode::Three },
    { KEY_4, KeyCode::Four },
    { KEY_5, KeyCode::Five },
    { KEY_6, KeyCode::Six },
    { KEY_7, KeyCode::Seven },
    { KEY_8, KeyCode::Eight },
    { KEY_9, KeyCode::Nine },
    { KEY_A, KeyCode::A },
    { KEY_B, KeyCode::B },
    { KEY_C, KeyCode::C },
    { KEY_D, KeyCode::D },
    { KEY_E, KeyCode::E },
    { KEY_F, KeyCode::F },
    { KEY_G, KeyCode::G },
    { KEY_H, KeyCode::H },
    { KEY_I, KeyCode::I },
    { KEY_J, KeyCode::J },
    { KEY_K, KeyCode::K },
    { KEY_L, KeyCode::L },
    { KEY_M, KeyCode::M },
    { KEY_N, KeyCode::N },
    { KEY_O, KeyCode::O },
    { KEY_P, KeyCode::P },
    { KEY_Q, KeyCode::Q },
    { KEY_R, KeyCode::R },
    { KEY_S, KeyCode::S },
    { KEY_T, KeyCode::T },
    { KEY_U, KeyCode::U },
    { KEY_V, KeyCode::V },
    { KEY_W, KeyCode::W },
    { KEY_X, KeyCode::X },
    { KEY_Y, KeyCode::Y },
    { KEY_Z, KeyCode::Z },
    { KEY_F1, KeyCode::F1 },
    { KEY_F2, KeyCode::F2 },
    { KEY_F3, KeyCode::F3 },
    { KEY_F4, KeyCode::F4 },
    { KEY_F5, KeyCode::F5 },
    { KEY_F6, KeyCode::F6 },
    { KEY_F7, KeyCode::F7 },
    { KEY_F8, KeyCode::F8 },
    { KEY_F9, KeyCode::F9 },
    { KEY_F10, KeyCode::F10 },
    { KEY_F11, KeyCode::F11 },
    { KEY_F12, KeyCode::F12 },
    { KEY_F13, KeyCode::F13 },
    { KEY_F14, KeyCode::F14 },
    { KEY_F15, KeyCode::F15 },
    { KEY_F16, KeyCode::F16 },
    { KEY_F17, KeyCode::F17 },
    { KEY_F18, KeyCode::F18 },
    { KEY_F19, KeyCode::F19 },
    { KEY_F20, KeyCode::F20 },
    { KEY_F21, KeyCode::F21 },
    { KEY_F22, KeyCode::F22 },
    { KEY_F23, KeyCode::F23 },
    { KEY_F24, KeyCode::F24 }
};

KeyCode getKeyCodeFromLinuxKey(int key);

#endif // __linux__

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