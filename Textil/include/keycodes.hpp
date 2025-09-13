/**
 * @file keycodes.hpp
 * @brief Cross-platform keyboard and mouse input key code definitions
 * @details Provides a unified key code enumeration and platform-specific mapping functions
 *          for Windows (WinAPI), Linux (evdev), and macOS (HID). Enables consistent
 *          input handling across different operating systems with automatic translation
 *          between platform-specific key representations and Textil's unified key codes.
 */

#ifndef TIL_KEYCODES_HPP
#define TIL_KEYCODES_HPP

#include <unordered_map>
#include "numeric_types.hpp"

namespace til
{
    /**
     * @brief Unified enumeration of all supported keyboard and mouse input codes
     * @details Comprehensive key code enumeration covering:
     *          - Mouse buttons (left, right, middle)
     *          - Standard keyboard keys (letters, numbers, function keys)
     *          - Modifier keys (shift, ctrl, alt with left/right variants)
     *          - Navigation keys (arrows, home, end, page up/down)
     *          - Numpad keys with separate enumeration from main numbers
     *          - Special keys (escape, space, enter, backspace, etc.)
     *          - Media control keys (volume, play/pause, track navigation)
     *          - Punctuation and symbol keys
     * 
     *          The enumeration provides platform-independent key identification,
     *          allowing applications to handle input consistently across Windows, Linux, and macOS.
     *          Key codes are mapped to platform-specific values using lookup tables.
     */
    enum class KeyCode
    {
        Invalid,                ///< Invalid or unrecognized key code

        // Mouse buttons
        LeftMouseButton,        ///< Primary mouse button (usually left)
        RightMouseButton,       ///< Secondary mouse button (usually right)
        MiddleMouseButton,      ///< Middle mouse button or scroll wheel click

        // Control and navigation keys
        BackSpace,              ///< Backspace key for deleting characters
        Tab,                    ///< Tab key for advancing focus or inserting tabs
        Clear,                  ///< Clear key (platform-specific functionality)
        Enter,                  ///< Enter/Return key for confirming input
        Shift,                  ///< Generic shift key (use LeftShift/RightShift for specificity)
        Ctrl,                   ///< Generic control key (use LeftControl/RightControl for specificity)
        Alt,                    ///< Generic alt key (use LeftAlt/RightAlt for specificity)
        Pause,                  ///< Pause/Break key
        CapsLock,               ///< Caps Lock toggle key
        Spacebar,               ///< Space bar for inserting spaces
        PageUp,                 ///< Page Up navigation key
        PageDown,               ///< Page Down navigation key
        End,                    ///< End key for jumping to end of line/document
        Home,                   ///< Home key for jumping to beginning of line/document
        Select,                 ///< Select key (platform-specific)
        Print,                  ///< Print key
        PrintScreen,            ///< Print Screen key for capturing screenshots
        Insert,                 ///< Insert key for toggling insert/overwrite mode
        Delete,                 ///< Delete key for removing characters
        Help,                   ///< Help key (platform-specific)
        
        // Numpad arithmetic operators
        Multiply,               ///< Numpad multiply (*) key
        Add,                    ///< Numpad plus (+) key
        Separator,              ///< Numpad separator key (platform-specific)
        Subtract,               ///< Numpad minus (-) key
        Decimal,                ///< Numpad decimal point (.) key
        Divide,                 ///< Numpad divide (/) key
        
        // Lock keys
        NumLock,                ///< Num Lock toggle key
        ScrollLock,             ///< Scroll Lock toggle key
        
        // Specific modifier keys (left/right variants)
        LeftShift,              ///< Left Shift modifier key
        RightShift,             ///< Right Shift modifier key
        LeftControl,            ///< Left Control modifier key
        RightControl,           ///< Right Control modifier key
        LeftAlt,                ///< Left Alt modifier key
        RightAlt,               ///< Right Alt modifier key (AltGr on some keyboards)
        
        // Media control keys
        VolumeMute,             ///< Volume mute toggle key
        VolumeDown,             ///< Volume decrease key
        VolumeUp,               ///< Volume increase key
        NextTrack,              ///< Next track/forward media key
        PreviousTrack,          ///< Previous track/backward media key
        Stop,                   ///< Stop media playback key
        PlayPause,              ///< Play/Pause media toggle key
        
        // Punctuation and symbol keys
        Semicolon,              ///< Semicolon (;) key
        Plus,                   ///< Plus/Equals (+/=) key
        Comma,                  ///< Comma (,) key
        Minus,                  ///< Minus/Underscore (-/_) key
        Period,                 ///< Period/Greater than (./>) key
        ForwardSlash,           ///< Forward slash/Question mark (/?) key
        Backquote,              ///< Backquote/Tilde (`/~) key
        SquareBracketOpen,      ///< Opening square bracket ([) key
        BackwardSlash,          ///< Backslash/Pipe (\/|) key
        SquareBracketClose,     ///< Closing square bracket (]) key
        Apostrophe,             ///< Apostrophe/Quote ('/") key
        Escape,                 ///< Escape key

        // Numpad number keys (0-9)
        Numpad0,                ///< Numpad 0 key
        Numpad1,                ///< Numpad 1 key
        Numpad2,                ///< Numpad 2 key
        Numpad3,                ///< Numpad 3 key
        Numpad4,                ///< Numpad 4 key
        Numpad5,                ///< Numpad 5 key
        Numpad6,                ///< Numpad 6 key
        Numpad7,                ///< Numpad 7 key
        Numpad8,                ///< Numpad 8 key
        Numpad9,                ///< Numpad 9 key

        // Arrow navigation keys
        Left,                   ///< Left arrow key
        Up,                     ///< Up arrow key
        Right,                  ///< Right arrow key
        Down,                   ///< Down arrow key

        // Main number keys (0-9)
        Zero,                   ///< Main keyboard 0 key
        One,                    ///< Main keyboard 1 key
        Two,                    ///< Main keyboard 2 key
        Three,                  ///< Main keyboard 3 key
        Four,                   ///< Main keyboard 4 key
        Five,                   ///< Main keyboard 5 key
        Six,                    ///< Main keyboard 6 key
        Seven,                  ///< Main keyboard 7 key
        Eight,                  ///< Main keyboard 8 key
        Nine,                   ///< Main keyboard 9 key

        // Letter keys (A-Z)
        A, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

        // Function keys (F1-F24)
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,

        Count                   ///< Total number of key codes (not a valid key)
    };

#ifdef _WIN32
    /**
     * @brief Mapping from Textil key codes to Windows Virtual Key codes
     * @details Lookup table for converting unified KeyCode values to Windows-specific
     *          VK_* constants used by the WinAPI. Enables input processing on Windows
     *          systems by translating platform events to unified key codes.
     */
    const std::unordered_map<KeyCode, u32> keyCodeToWinapiVK {
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

    /**
     * @brief Convert Textil key code to Windows Virtual Key code
     * @param key Textil KeyCode to convert
     * @return Corresponding Windows VK_* constant
     * @details Converts unified KeyCode to Windows-specific virtual key code for use with WinAPI functions.
     *          Returns 0 for unmapped keys. Used internally by Windows input processing.
     */
    int getWinapiVK(KeyCode key);

#endif // _WIN32

#ifdef __linux__

    /**
     * @brief Mapping from Linux evdev key codes to Textil key codes
     * @details Lookup table for converting Linux input event key codes (from linux/input.h)
     *          to unified KeyCode values. Handles both keyboard keys (KEY_*) and mouse buttons (BTN_*).
     */
    const std::unordered_map<int, KeyCode> linuxKeyToKeyCode {
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

    /**
     * @brief Convert Linux evdev key code to Textil key code
     * @param key Linux input event key code (KEY_* or BTN_* constants)
     * @return Corresponding Textil KeyCode value
     * @details Converts Linux-specific input event codes to unified KeyCode values.
     *          Returns KeyCode::Invalid for unmapped keys. Used internally by Linux input processing.
     */
    KeyCode getKeyCodeFromLinuxKey(int key);

#endif // __linux__

#ifdef __APPLE__

    /**
     * @brief Mapping from macOS HID Usage codes to Textil key codes
     * @details Lookup table for converting macOS Human Interface Device (HID) usage codes
     *          to unified KeyCode values. Maps kHIDUsage_* constants to Textil key codes.
     */
    static const std::unordered_map<u32, KeyCode> hidUsageToKeyCode {
        { kHIDUsage_KeyboardDeleteOrBackspace, KeyCode::BackSpace },
        { kHIDUsage_KeyboardTab,                KeyCode::Tab },
        { kHIDUsage_KeyboardClear,              KeyCode::Clear },
        { kHIDUsage_KeyboardReturnOrEnter,      KeyCode::Enter },
        { kHIDUsage_KeyboardLeftShift,          KeyCode::LeftShift },
        { kHIDUsage_KeyboardRightShift,         KeyCode::RightShift },
        { kHIDUsage_KeyboardLeftControl,        KeyCode::LeftControl },
        { kHIDUsage_KeyboardRightControl,       KeyCode::RightControl },
        { kHIDUsage_KeyboardLeftAlt,            KeyCode::LeftAlt },
        { kHIDUsage_KeyboardRightAlt,           KeyCode::RightAlt },
        { kHIDUsage_KeyboardPause,              KeyCode::Pause },
        { kHIDUsage_KeyboardCapsLock,           KeyCode::CapsLock },
        { kHIDUsage_KeyboardSpacebar,           KeyCode::Spacebar },
        { kHIDUsage_KeyboardPageUp,             KeyCode::PageUp },
        { kHIDUsage_KeyboardPageDown,           KeyCode::PageDown },
        { kHIDUsage_KeyboardEnd,                KeyCode::End },
        { kHIDUsage_KeyboardHome,               KeyCode::Home },
        { kHIDUsage_KeyboardSelect,             KeyCode::Select },
        { kHIDUsage_KeyboardPrintScreen,        KeyCode::PrintScreen },
        { kHIDUsage_KeyboardInsert,             KeyCode::Insert },
        { kHIDUsage_KeyboardDeleteForward,      KeyCode::Delete },
        { kHIDUsage_KeyboardHelp,               KeyCode::Help },
        { kHIDUsage_KeypadAsterisk,             KeyCode::Multiply },
        { kHIDUsage_KeypadPlus,                 KeyCode::Add },
        { kHIDUsage_KeypadHyphen,               KeyCode::Subtract },
        { kHIDUsage_KeypadSlash,                KeyCode::Divide },
        { kHIDUsage_KeypadNumLock,              KeyCode::NumLock },
        { kHIDUsage_KeyboardScrollLock,         KeyCode::ScrollLock },
        { kHIDUsage_Csmr_Mute,                  KeyCode::VolumeMute },
        { kHIDUsage_Csmr_VolumeIncrement,       KeyCode::VolumeUp },
        { kHIDUsage_Csmr_VolumeDecrement,       KeyCode::VolumeDown },
        { kHIDUsage_Csmr_ScanNextTrack,         KeyCode::NextTrack },
        { kHIDUsage_Csmr_ScanPreviousTrack,     KeyCode::PreviousTrack },
        { kHIDUsage_Csmr_Stop,                  KeyCode::Stop },
        { kHIDUsage_Csmr_PlayOrPause,           KeyCode::PlayPause },
        { kHIDUsage_KeyboardSemicolon,          KeyCode::Semicolon },
        { kHIDUsage_KeyboardEqualSign,          KeyCode::Plus },
        { kHIDUsage_KeyboardComma,              KeyCode::Comma },
        { kHIDUsage_KeyboardHyphen,             KeyCode::Minus },
        { kHIDUsage_KeyboardPeriod,             KeyCode::Period },
        { kHIDUsage_KeyboardSlash,              KeyCode::ForwardSlash },
        { kHIDUsage_KeyboardGraveAccentAndTilde,KeyCode::Backquote },
        { kHIDUsage_KeyboardOpenBracket,        KeyCode::SquareBracketOpen },
        { kHIDUsage_KeyboardBackslash,          KeyCode::BackwardSlash },
        { kHIDUsage_KeyboardCloseBracket,       KeyCode::SquareBracketClose },
        { kHIDUsage_KeyboardQuote,              KeyCode::Apostrophe },
        { kHIDUsage_KeyboardEscape,             KeyCode::Escape },
        { kHIDUsage_Keyboard0,                  KeyCode::Zero },
        { kHIDUsage_Keyboard1,                  KeyCode::One },
        { kHIDUsage_Keyboard2,                  KeyCode::Two },
        { kHIDUsage_Keyboard3,                  KeyCode::Three },
        { kHIDUsage_Keyboard4,                  KeyCode::Four },
        { kHIDUsage_Keyboard5,                  KeyCode::Five },
        { kHIDUsage_Keyboard6,                  KeyCode::Six },
        { kHIDUsage_Keyboard7,                  KeyCode::Seven },
        { kHIDUsage_Keyboard8,                  KeyCode::Eight },
        { kHIDUsage_Keyboard9,                  KeyCode::Nine },
        { kHIDUsage_KeyboardA,                  KeyCode::A },
        { kHIDUsage_KeyboardB,                  KeyCode::B },
        { kHIDUsage_KeyboardC,                  KeyCode::C },
        { kHIDUsage_KeyboardD,                  KeyCode::D },
        { kHIDUsage_KeyboardE,                  KeyCode::E },
        { kHIDUsage_KeyboardF,                  KeyCode::F },
        { kHIDUsage_KeyboardG,                  KeyCode::G },
        { kHIDUsage_KeyboardH,                  KeyCode::H },
        { kHIDUsage_KeyboardI,                  KeyCode::I },
        { kHIDUsage_KeyboardJ,                  KeyCode::J },
        { kHIDUsage_KeyboardK,                  KeyCode::K },
        { kHIDUsage_KeyboardL,                  KeyCode::L },
        { kHIDUsage_KeyboardM,                  KeyCode::M },
        { kHIDUsage_KeyboardN,                  KeyCode::N },
        { kHIDUsage_KeyboardO,                  KeyCode::O },
        { kHIDUsage_KeyboardP,                  KeyCode::P },
        { kHIDUsage_KeyboardQ,                  KeyCode::Q },
        { kHIDUsage_KeyboardR,                  KeyCode::R },
        { kHIDUsage_KeyboardS,                  KeyCode::S },
        { kHIDUsage_KeyboardT,                  KeyCode::T },
        { kHIDUsage_KeyboardU,                  KeyCode::U },
        { kHIDUsage_KeyboardV,                  KeyCode::V },
        { kHIDUsage_KeyboardW,                  KeyCode::W },
        { kHIDUsage_KeyboardX,                  KeyCode::X },
        { kHIDUsage_KeyboardY,                  KeyCode::Y },
        { kHIDUsage_KeyboardZ,                  KeyCode::Z },
        { kHIDUsage_KeyboardF1,                 KeyCode::F1 },
        { kHIDUsage_KeyboardF2,                 KeyCode::F2 },
        { kHIDUsage_KeyboardF3,                 KeyCode::F3 },
        { kHIDUsage_KeyboardF4,                 KeyCode::F4 },
        { kHIDUsage_KeyboardF5,                 KeyCode::F5 },
        { kHIDUsage_KeyboardF6,                 KeyCode::F6 },
        { kHIDUsage_KeyboardF7,                 KeyCode::F7 },
        { kHIDUsage_KeyboardF8,                 KeyCode::F8 },
        { kHIDUsage_KeyboardF9,                 KeyCode::F9 },
        { kHIDUsage_KeyboardF10,                KeyCode::F10 },
        { kHIDUsage_KeyboardF11,                KeyCode::F11 },
        { kHIDUsage_KeyboardF12,                KeyCode::F12 },
        { kHIDUsage_KeyboardF13,                KeyCode::F13 },
        { kHIDUsage_KeyboardF14,                KeyCode::F14 },
        { kHIDUsage_KeyboardF15,                KeyCode::F15 },
        { kHIDUsage_KeyboardF16,                KeyCode::F16 },
        { kHIDUsage_KeyboardF17,                KeyCode::F17 },
        { kHIDUsage_KeyboardF18,                KeyCode::F18 },
        { kHIDUsage_KeyboardF19,                KeyCode::F19 },
        { kHIDUsage_KeyboardF20,                KeyCode::F20 },
        { kHIDUsage_KeyboardF21,                KeyCode::F21 },
        { kHIDUsage_KeyboardF22,                KeyCode::F22 },
        { kHIDUsage_KeyboardF23,                KeyCode::F23 },
        { kHIDUsage_KeyboardF24,                KeyCode::F24 }
    };

    /**
     * @brief Convert macOS HID usage code to Textil key code
     * @param key macOS HID usage code
     * @return Corresponding Textil KeyCode value
     * @details Converts macOS-specific HID usage codes to unified KeyCode values.
     *          Returns KeyCode::Invalid for unmapped keys. Used internally by macOS input processing.
     */
    KeyCode getKeyCodeFromMacVK(u32 key);

    /**
     * @brief Mapping from macOS mouse button codes to Textil key codes
     * @details Lookup table for converting macOS-specific mouse button identifiers
     *          to unified KeyCode values for mouse input processing.
     */
    const std::unordered_map<u32, KeyCode> macMouseVKToKeyCode {
        { 0x01, KeyCode::LeftMouseButton },
        { 0x02, KeyCode::RightMouseButton },
        { 0x03, KeyCode::MiddleMouseButton }
    };

    /**
     * @brief Convert macOS mouse button code to Textil key code
     * @param key macOS mouse button identifier
     * @return Corresponding Textil KeyCode value for mouse button
     * @details Converts macOS-specific mouse button codes to unified KeyCode values.
     *          Used internally by macOS mouse input processing.
     */
    KeyCode getKeyCodeFromMacMouseVK(u32 key);

#endif // __APPLE__

}

#endif // TIL_KEYCODES_HPP