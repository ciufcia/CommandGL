/**
 * @file console.hpp
 * @brief Cross-platform terminal/console interface for Textil library
 * 
 * @details Provides low-level terminal access and manipulation across Windows, Linux, and macOS.
 * Handles input device management, screen buffer operations, and platform-specific
 * terminal control. This is the foundational layer that enables Textil's graphics
 * capabilities in terminal environments.
 */

#ifndef TIL_CONSOLE_HPP
#define TIL_CONSOLE_HPP

#ifdef _WIN32
#include <windows.h>
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
#include "event.hpp"
#include <array>
#include <vector>
#include "filters.hpp"
#include "character_cell.hpp"
#include "window.hpp"

namespace til
{
    /**
     * @brief Low-level cross-platform terminal interface and input management
     * @details The Console class provides direct access to terminal/console functionality
     *          across Windows, Linux, and macOS platforms. It handles:
     *          - Terminal screen buffer management
     *          - Input device detection and configuration
     *          - Event processing from keyboard, mouse, and console
     *          - Platform-specific terminal mode setup
     *          - Character rendering and display operations
     * 
     *          This class is typically managed by the Framework and not used directly
     *          by applications. It abstracts the complex platform differences in
     *          terminal handling behind a unified interface.
     * 
     *          Platform implementations:
     *          - Windows: Uses WinAPI console functions and input records
     *          - Linux: Uses termios for terminal control and evdev for input
     *          - macOS: Uses HID framework for input and terminal manipulation
     */
    class Console
    {
    public:
        /**
         * @brief Deleted copy constructor to prevent copying
         * @details Console manages unique system resources and cannot be safely copied.
         */
        Console(const Console&) = delete;
        
        /**
         * @brief Deleted copy assignment operator
         * @details Console manages unique system resources and cannot be safely copied.
         */
        Console& operator=(const Console&) = delete;

        /**
         * @brief Destructor handling cleanup of console resources
         * @details Automatically restores original terminal settings and releases
         *          platform-specific resources like device handles and file descriptors.
         */
        ~Console();

        /**
         * @brief Get current terminal size in character cells
         * @return Vector2 containing width and height in characters
         * @details Returns the current dimensions of the terminal window.
         *          This may change if the user resizes the terminal, triggering
         *          ConsoleEvent notifications.
         */
        Vector2<u32> getSize() const;

#ifdef __linux__
        /**
         * @brief Find available keyboard input devices (Linux only)
         * @return Vector of device paths that can provide keyboard input
         * @details Scans /dev/input/event* devices and identifies those capable
         *          of generating keyboard events. Used for automatic device detection
         *          and manual device management.
         */
        std::vector<std::string> findValidKeyboardDevices();
        
        /**
         * @brief Find available mouse input devices (Linux only)
         * @return Vector of device paths that can provide mouse input
         * @details Scans /dev/input/event* devices and identifies those capable
         *          of generating mouse events including movement and button presses.
         */
        std::vector<std::string> findValidMouseDevices();

        /**
         * @brief Add keyboard device for input monitoring (Linux only)
         * @param devicePath Path to input device (e.g., "/dev/input/event0")
         * @details Begins monitoring the specified device for keyboard events.
         *          Requires appropriate permissions to access the device file.
         *          Events from this device will be included in input processing.
         */
        void addKeyboardDevice(const std::string &devicePath);
        
        /**
         * @brief Add mouse device for input monitoring (Linux only)
         * @param devicePath Path to input device (e.g., "/dev/input/event1")
         * @details Begins monitoring the specified device for mouse events.
         *          Requires appropriate permissions to access the device file.
         *          Events from this device will be included in input processing.
         */
        void addMouseDevice(const std::string &devicePath);

        /**
         * @brief Remove keyboard device from monitoring (Linux only)
         * @param devicePath Path to device to stop monitoring
         * @details Stops monitoring the specified keyboard device and releases
         *          associated resources. Device events will no longer be processed.
         */
        void removeKeyboardDevice(const std::string &devicePath);
        
        /**
         * @brief Remove mouse device from monitoring (Linux only)
         * @param devicePath Path to device to stop monitoring
         * @details Stops monitoring the specified mouse device and releases
         *          associated resources. Device events will no longer be processed.
         */
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

    private:

        Console() = default;

        void init();
        void reset();
        void clear();
        void fit(Vector2<u32> newSize);
        void drawWindow(const Window &window);
        void writeBuffer();
        void constructOutputString(const FilterableBuffer<CharacterCell> &buffer, const Vector2<u32> &size);
        void writeCharacterBuffer(const FilterableBuffer<CharacterCell> &buffer, const Vector2<u32> &size);
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
        void setupFds();

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

        Vector2<u32> m_screenSize { 0, 0 };
        FilterableBuffer<CharacterCell> m_characterBuffer {};
        std::string m_outputString = "";

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
        fd_set m_inputFds;
        int m_maxFd = -1;
        struct timeval m_inputTimeval;
        int m_selectReturn = 0;
#endif // __linux__

#ifdef __APPLE__
IOHIDManagerRef m_hidManager { nullptr };
CFRunLoopRef m_runLoop { nullptr };
std::thread m_inputThread;
std::vector<Event> m_pendingEvents;
std::mutex m_pendingEventsMutex;
#endif

        Vector2<u32> m_eventCurrentConsoleSize { 0, 0 };

    friend class EventManager;
    friend class Framework;
    };
}

#endif // TIL_CONSOLE_HPP