#ifndef CGL_CONSOLE_HPP
#define CGL_CONSOLE_HPP

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

    private:

        Console() = default;

        void init();
        void reset();

        void clear();
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

#if defined(__linux__) || defined(__APPLE__)
        Vector2<u32> m_currentConsoleSize { 0, 0 };
#endif // __linux__ || __APPLE__

    friend class EventManager;
    friend class Framework;
    };
}

#endif // CGL_CONSOLE_HPP