#ifndef CGL_EVENT_HPP
#define CGL_EVENT_HPP

#include <windows.h>
#include <memory>

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
     * @enum KeyCode
     * @brief Enumeration of all supported keyboard and mouse input codes.
     * 
     * This enumeration maps to Windows Virtual Key Codes (VK_*) and provides
     * a comprehensive set of input identifiers including keyboard keys, mouse
     * buttons, function keys, and multimedia keys.
     */
    enum class KeyCode
    {
        LeftMouseButton        = VK_LBUTTON,
        RightMouseButton       = VK_RBUTTON,
        ControlBreakProcessing = VK_CANCEL,
        MiddleMouseButton      = VK_MBUTTON,
        ForwardMouseButton     = VK_XBUTTON1,
        BackwardMouseButton    = VK_XBUTTON2,
        BackSpace              = VK_BACK,
        Tab                    = VK_TAB,
        Clear                  = VK_CLEAR,
        Enter                  = VK_RETURN,
        Shift                  = VK_SHIFT,
        Ctrl                   = VK_CONTROL,
        Alt                    = VK_MENU,
        Pause                  = VK_PAUSE,
        CapsLock               = VK_CAPITAL,
        ImeKanaMode            = VK_KANA,
        ImeHangulMode          = VK_HANGUL,
        ImeOn                  = VK_IME_ON,
        ImeJunjaMode           = VK_JUNJA,
        ImeFinal               = VK_FINAL,
        ImeHanja               = VK_HANJA,
        ImeKanji               = VK_KANJI,
        ImeOff                 = VK_IME_OFF,
        Escape                 = VK_ESCAPE,
        ImeConvert             = VK_CONVERT,
        ImeNonConvert          = VK_NONCONVERT,
        ImeAccept              = VK_ACCEPT,
        ImeModeChange          = VK_MODECHANGE,
        Spacebar               = VK_SPACE,
        PageUp                 = VK_PRIOR,
        PageDown               = VK_NEXT,
        End                    = VK_END,
        Home                   = VK_HOME,
        Left                   = VK_LEFT,
        Up                     = VK_UP,
        Right                  = VK_RIGHT,
        Down                   = VK_DOWN,
        Select                 = VK_SELECT,
        Print                  = VK_PRINT,
        Execute                = VK_EXECUTE,
        PrintScreen            = VK_SNAPSHOT,
        Insert                 = VK_INSERT,
        Delete                 = VK_DELETE,
        Help                   = VK_HELP,
        Zero                   = 0x30,
        One                    = 0x31,
        Two                    = 0x32,
        Three                  = 0x33,
        Four                   = 0x34,
        Five                   = 0x35,
        Six                    = 0x36,
        Seven                  = 0x37,
        Eight                  = 0x38,
        Nine                   = 0x39,
        A                      = 0x41,
        B                      = 0x42,
        C                      = 0x43,
        D                      = 0x44,
        E                      = 0x45,
        F                      = 0x46,
        G                      = 0x47,
        H                      = 0x48,
        I                      = 0x49,
        J                      = 0x4A,
        K                      = 0x4B,
        L                      = 0x4C,
        M                      = 0x4D,
        N                      = 0x4E,
        O                      = 0x4F,
        P                      = 0x50,
        Q                      = 0x51,
        R                      = 0x52,
        S                      = 0x53,
        T                      = 0x54,
        U                      = 0x55,
        V                      = 0x56,
        W                      = 0x57,
        X                      = 0x58,
        Y                      = 0x59,
        Z                      = 0x5A,
        LeftWindowsKey         = VK_LWIN,
        RightWindowsKey        = VK_RWIN,
        Applications           = VK_APPS,
        Sleep                  = VK_SLEEP,
        Numpad0                = VK_NUMPAD0,
        Numpad1                = VK_NUMPAD1,
        Numpad2                = VK_NUMPAD2,
        Numpad3                = VK_NUMPAD3,
        Numpad4                = VK_NUMPAD4,
        Numpad5                = VK_NUMPAD5,
        Numpad6                = VK_NUMPAD6,
        Numpad7                = VK_NUMPAD7,
        Numpad8                = VK_NUMPAD8,
        Numpad9                = VK_NUMPAD9,
        Multiply               = VK_MULTIPLY,
        Add                    = VK_ADD,
        Separator              = VK_SEPARATOR,
        Subtract               = VK_SUBTRACT,
        Decimal                = VK_DECIMAL,
        Divide                 = VK_DIVIDE,
        F1                     = VK_F1,
        F2                     = VK_F2,
        F3                     = VK_F3,
        F4                     = VK_F4,
        F5                     = VK_F5,
        F6                     = VK_F6,
        F7                     = VK_F7,
        F8                     = VK_F8,
        F9                     = VK_F9,
        F10                    = VK_F10,
        F11                    = VK_F11,
        F12                    = VK_F12,
        F13                    = VK_F13,
        F14                    = VK_F14,
        F15                    = VK_F15,
        F16                    = VK_F16,
        F17                    = VK_F17,
        F18                    = VK_F18,
        F19                    = VK_F19,
        F20                    = VK_F20,
        F21                    = VK_F21,
        F22                    = VK_F22,
        F23                    = VK_F23,
        F24                    = VK_F24,
        NumLock                = VK_NUMLOCK,
        ScrollLock             = VK_SCROLL,
        LeftShift              = VK_LSHIFT,
        RightShift             = VK_RSHIFT,
        LeftControl            = VK_LCONTROL,
        RightControl           = VK_RCONTROL,
        LeftAlt                = VK_LMENU,
        RightAlt               = VK_RMENU,
        BrowserBack            = VK_BROWSER_BACK,
        BrowserForward         = VK_BROWSER_FORWARD,
        BrowserRefresh         = VK_BROWSER_REFRESH,
        BrowserStop            = VK_BROWSER_STOP,
        BrowserSearch          = VK_BROWSER_SEARCH,
        BrowserFavorites       = VK_BROWSER_FAVORITES,
        BrowserHome            = VK_BROWSER_HOME,
        VolumeMute             = VK_VOLUME_MUTE,
        VolumeDown             = VK_VOLUME_DOWN,
        VolumeUp               = VK_VOLUME_UP,
        NextTrack              = VK_MEDIA_NEXT_TRACK,
        PreviousTrack          = VK_MEDIA_PREV_TRACK,
        Stop                   = VK_MEDIA_STOP,
        PlayPause              = VK_MEDIA_PLAY_PAUSE,
        StartMail              = VK_LAUNCH_MAIL,
        SelectMedia            = VK_LAUNCH_MEDIA_SELECT,
        StartApplication1      = VK_LAUNCH_APP1,
        StartApplication2      = VK_LAUNCH_APP2,
        Semicolon              = VK_OEM_1,
        Plus                   = VK_OEM_PLUS,
        Comma                  = VK_OEM_COMMA,
        Minus                  = VK_OEM_MINUS,
        Period                 = VK_OEM_PERIOD,
        ForwardSlash           = VK_OEM_2,
        Backquote              = VK_OEM_3,
        SquareBracketOpen      = VK_OEM_4,
        BackwardSlash          = VK_OEM_5,
        SquareBracketClose     = VK_OEM_6,
        Apostrophe             = VK_OEM_7,
        ImeProcess             = VK_PROCESSKEY,
        Packet                 = VK_PACKET,
        Attn                   = VK_ATTN,
        CrSel                  = VK_CRSEL,
        ExSel                  = VK_EXSEL,
        ErEOF                  = VK_EREOF,
        Play                   = VK_PLAY,
        Zoom                   = VK_ZOOM,
        PA1                    = VK_PA1,
        OEMClear               = VK_OEM_CLEAR,
        Count
    };

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
         * For other event types, this field may contain KeyCode::Count.
         */
        KeyCode key { KeyCode::Count };

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