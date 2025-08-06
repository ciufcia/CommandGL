#ifndef CGL_CONSOLE_HPP
#define CGL_CONSOLE_HPP

#ifdef _WIN32
#include <windows.h>
#include "console_font.hpp"
#endif // _WIN32
#ifdef __linux__
#include <libevdev/libevdev.h>
#endif // __linux__
#ifdef __APPLE__
#include <IOKit/hid/IOHIDManager.h>
#include <thread>
#endif // __APPLE__
#include "vector2.hpp"
#include "numeric_types.hpp"
#include "character_buffer.hpp"
#include "event.hpp"
#include <array>
#include <vector>

namespace cgl
{
    /**
     * @class Console
     * @brief Manages Windows console operations including display, input, and font configuration.
     * 
     * The Console class provides a high-level interface for interacting with the Windows console,
     * including resizing the window, changing fonts, writing character buffers, and handling
     * input events. It automatically saves and restores the original console state.
     */
    class Console
    {
    public:
  
        Console(const Console&) = delete;
        Console& operator=(const Console&) = delete;

        ~Console();

        /**
         * @brief Gets the current size of the console window in character cells.
         *
         * This function queries the console screen buffer to determine the current
         * width and height of the console window, in character cells. The result
         * is returned as a reference to a Vector2<u32> where x is the width and y is the height.
         * 
         * @return Reference to a Vector2<u32> containing the console's width (x) and height (y).
         * @throws std::runtime_error if unable to get console screen buffer info.
         */
        Vector2<u32> getSize() const;

#ifdef _WIN32

        /**
         * @brief Sets the size of the console window in character cells.
         *
         * This function attempts to resize the console window to the specified width and height (in character cells).
         *
         * @param size The desired size of the console window as a Vector2<u32> (width, height).
         *
         * @throws std::invalid_argument if either width or height is zero.
         * @throws std::runtime_error if unable to get console screen buffer info, 
         *         get largest console window size, set console window info, 
         *         set console screen buffer size, or if the requested size exceeds 
         *         the largest console window size or is too small for the current font.
         *
         * @note If called in an environment where the console is not resizable (such as Windows Terminal or remote sessions),
         *       the behavior is undefined and may result in errors or no effect. This function is intended to work in the legacy console host.
         */
        void setSize(const Vector2<u32> &size);

        /**
         * @brief Gets the font configuration used by the console.
         *
         * This function returns a reference to the ConsoleFont object representing
         * the font settings (name, size, weight, and family) currently used by the console.
         *
         * @return Reference to a ConsoleFont containing the console's font configuration.
         * @throws std::runtime_error if unable to get console font info.
         */
        ConsoleFont getFont() const;

        /**
         * @brief Sets the font configuration for the console.
         *
         * This function attempts to change the console's font settings (name, size, weight, and family)
         * to the specified ConsoleFont configuration.
         *
         * @param font The desired ConsoleFont configuration to apply to the console.
         *
         * @throws std::runtime_error if unable to set console font.
         *
         * @note If called in an environment where the console font cannot be changed (such as Windows Terminal or remote sessions),
         *       the behavior is undefined and may result in errors or no effect. This function is intended to work in the legacy console host.
         */
        void setFont(const ConsoleFont &font);

        /**
         * @brief Gets the current console title.
         *
         * This function retrieves the current title of the console window.
         * The title is returned as a standard string.
         *
         * @return The current console title as a std::string.
         * @throws std::runtime_error if unable to get console title.
         */
        std::string getTitle() const;

        /**
         * @brief Sets the console title.
         *
         * This function changes the title of the console window to the specified string.
         * The title is displayed in the console window's title bar.
         *
         * @param title The new title for the console window as a std::string.
         * @throws std::runtime_error if unable to set console title.
         */
        void setTitle(const std::string &title);

#endif // _WIN32

#ifdef __linux__


        std::vector<std::string> findValidKeyboardDevices();
        std::vector<std::string> findValidMouseDevices();

        void addKeyboardDevice(const std::string &devicePath);
        void addMouseDevice(const std::string &devicePath);

        void removeKeyboardDevice(const std::string &devicePath);
        void removeMouseDevice(const std::string &devicePath);

#endif // __linux__

    private:

#ifdef _WIN32

        struct Handles
        {
            HANDLE input;
            HANDLE output;
        };

#endif // _WIN32

#ifdef __linux__

        struct DeviceData
        {
            std::string path;
            libevdev *device { nullptr };
            int fd { -1 };
        };

#endif // __linux__

    public:

        Console();

        void init();
        void reset();

        void clear();
        void writeCharacterBuffer(const CharacterBuffer &buffer);
        void getEvents(std::vector<Event> &events);
        void getMouseEvents(std::vector<Event> &events);
        void getKeyboardEvents(std::vector<Event> &events);
        void getConsoleEvents(std::vector<Event> &events);
        const std::array<bool, static_cast<size_t>(KeyCode::Count)> &getKeyStates() const;

#ifdef _WIN32
        void getHandles();
        void setInputMode();
	    void setOutputMode();
        void parseInputRecords(const std::vector<INPUT_RECORD> &inputRecords);
        std::wstring stringToWideString(const std::string &str) const;
        std::string wideStringToString(const std::wstring &wstr) const;
#endif // _WIN32

#ifdef __linux__
        std::string findKeyboardDevice();
        std::string findMouseDevice();
        void clearDevices();
        void processKeyboardDeviceEvents(fd_set &fds, DeviceData &deviceData, std::vector<Event> &events);
        void processMouseDeviceEvents(fd_set &fds, DeviceData &deviceData, std::vector<Event> &events);
#endif // __linux__

#ifdef __APPLE__
        static void keyboardInputCallback(void *context, IOReturn result, void *sender, IOHIDValueRef event);
        CFMutableDictionaryRef setupKeyboardDictionary();

        static void mouseInputCallback(void *context, IOReturn result, void *sender, IOHIDValueRef event);
        CFMutableDictionaryRef setupMouseDictionary();

        static void unifiedInputCallback(void *context, IOReturn result, void *sender, IOHIDValueRef event);

        void inputThreadFunction();
        void setupInputThread();
        void stopInputThread();
#endif // __APPLE__

#if defined(__linux__) || defined(__APPLE__)
        void setTerminalRawMode();
        void resetTerminalMode();
#endif

    private:
       
        std::array<bool, static_cast<size_t>(KeyCode::Count)> m_keyStates { false }; 
        Vector2<i32> m_lastMousePosition { 0, 0 };
        Vector2<i32> m_currentMousePosition { 0, 0 };
        Vector2<i32> m_relativeMouseMovement { 0, 0 };

#ifdef _WIN32
        Handles m_handles;

        DWORD m_firstInputMode;
	    DWORD m_firstOutputMode;
        
        DWORD m_lastMouseButtonState { 0u };

        std::vector<INPUT_RECORD> m_mouseInputRecords;
        std::vector<INPUT_RECORD> m_keyboardInputRecords;
        std::vector<INPUT_RECORD> m_consoleInputRecords;
#endif // _WIN32

#ifdef __linux__
        std::vector<DeviceData> m_keyboardDevices;
        std::vector<DeviceData> m_mouseDevices;
#endif // __linux__

#ifdef __APPLE__
IOHIDManagerRef m_hidManager { nullptr };
CFRunLoopRef m_runLoop { nullptr };
std::thread m_inputThread;
std::vector<Event> m_pendingEvents;
std::mutex m_pendingEventsMutex;
#endif

#if defined(__linux__) || defined(__APPLE__)
        Vector2<u32> m_currentConsoleSize { 0, 0 };
#endif // __linux__ || __APPLE__

    friend class EventManager;
    friend class Framework;
    };
}

#endif // CGL_CONSOLE_HPP