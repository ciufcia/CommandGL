#include "cgl.hpp"

#include <stdexcept>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <cstdlib>

#ifdef __linux__
#include <libevdev/libevdev.h>
#endif

#if defined(__linux__) || defined(__APPLE__)
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <cstring>
#include <termios.h>
#endif

namespace cgl
{
    Console::Console() {
        
    }
    
    Console::~Console() {
        reset();
    }
    
    Vector2<u32> Console::getSize() const {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
        if (!GetConsoleScreenBufferInfo(
            m_handles.output,
            &consoleScreenBufferInfo
        )) throw std::runtime_error("Failed to get console screen buffer info");

        return { static_cast<u32>(consoleScreenBufferInfo.dwSize.X), static_cast<u32>(consoleScreenBufferInfo.dwSize.Y) };
#elif defined(__linux__) || defined(__APPLE__)
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) throw std::runtime_error("Failed to get console size");
        return { static_cast<u32>(w.ws_col), static_cast<u32>(w.ws_row) };
#endif // _WIN32
    }

#ifdef _WIN32
    void Console::setSize(const Vector2<u32> &size)
    {
        if (size.x == 0u || size.y == 0u)
            throw std::invalid_argument("Size cannot be zero");

        CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
        if (!GetConsoleScreenBufferInfo(
            m_handles.output,
            &screen_buffer_info
        )) throw std::runtime_error("Failed to get console screen buffer info");

        COORD largest_console_window_size = GetLargestConsoleWindowSize(m_handles.output);
        if (largest_console_window_size.X + largest_console_window_size.Y == 0) 
            throw std::runtime_error("Failed to get largest console window size");

        const SMALL_RECT max_window_rect { 0u, 0u, largest_console_window_size.X - 1, largest_console_window_size.Y - 1 };
        const COORD min_window_size { static_cast<SHORT>(GetSystemMetrics(SM_CXMIN)), static_cast<SHORT>(GetSystemMetrics(SM_CYMIN)) };

        ConsoleFont font = getFont();

        if (
            screen_buffer_info.dwSize.X > largest_console_window_size.X ||
            screen_buffer_info.dwSize.Y > largest_console_window_size.Y
        ) {
            SMALL_RECT valid_max_rect = max_window_rect;
            if (screen_buffer_info.dwSize.X < largest_console_window_size.X) {
                valid_max_rect.Right = screen_buffer_info.dwSize.X - 1;
            }
            if (screen_buffer_info.dwSize.Y < largest_console_window_size.Y) {
                valid_max_rect.Bottom = screen_buffer_info.dwSize.Y - 1;
            }

            if (
                (valid_max_rect.Right + 1) * static_cast<SHORT>(font.size.x) < min_window_size.X ||
                (valid_max_rect.Bottom + 1) * static_cast<SHORT>(font.size.y) < min_window_size.Y
            ) throw std::runtime_error("Console size is too small for the current font");

            if (!SetConsoleWindowInfo(
                m_handles.output,
                TRUE,
                &valid_max_rect
            )) throw std::runtime_error("Failed to set console window info");

            if (!SetConsoleScreenBufferSize(
                m_handles.output,
                largest_console_window_size
            )) throw std::runtime_error("Failed to set console screen buffer size");
        }

        if (
            static_cast<SHORT>(size.x) > largest_console_window_size.X ||
            static_cast<SHORT>(size.y) > largest_console_window_size.Y
        ) throw std::runtime_error("Requested size exceeds largest console window size");

        const COORD new_window_size { static_cast<SHORT>(size.x), static_cast<SHORT>(size.y) };

        if (new_window_size.X * static_cast<SHORT>(font.size.x) < min_window_size.X ||
            new_window_size.Y * static_cast<SHORT>(font.size.y) < min_window_size.Y
        ) throw std::runtime_error("Requested size is too small for the current font");

        const SMALL_RECT new_window_rect { 0u, 0u, new_window_size.X - 1, new_window_size.Y - 1 };

        if (!SetConsoleScreenBufferSize(
            m_handles.output,
            largest_console_window_size
        )) throw std::runtime_error("Failed to set console screen buffer size");

        if (!GetConsoleScreenBufferInfo(
            m_handles.output,
            &screen_buffer_info
        )) throw std::runtime_error("Failed to get console screen buffer info after setting size");

        if (!SetConsoleWindowInfo(
            m_handles.output,
            TRUE,
            &new_window_rect
        )) throw std::runtime_error("Failed to set console window info");

        if (!SetConsoleScreenBufferSize(
            m_handles.output,
            new_window_size
        )) throw std::runtime_error("Failed to set console screen buffer size after setting size");
    }
    
    ConsoleFont Console::getFont() const
    {
        CONSOLE_FONT_INFOEX fontInfo = { 0 };
        fontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);
        if (!GetCurrentConsoleFontEx(
            m_handles.output,
            FALSE,
            &fontInfo
        )) throw std::runtime_error("Failed to get console font info");

        return ConsoleFont(
            wideStringToString(std::wstring(fontInfo.FaceName, fontInfo.FaceName + wcslen(fontInfo.FaceName))),
            Vector2<u32>(static_cast<u32>(fontInfo.dwFontSize.X), static_cast<u32>(fontInfo.dwFontSize.Y)),
            static_cast<ConsoleFontWeight>(fontInfo.FontWeight),
            static_cast<ConsoleFontFamily>(fontInfo.FontFamily)
        );
    }
    
    void Console::setFont(const ConsoleFont &font)
    {
        CONSOLE_FONT_INFOEX fontInfo = { 0 };
        fontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);
        fontInfo.dwFontSize.X = static_cast<SHORT>(font.size.x);
        fontInfo.dwFontSize.Y = static_cast<SHORT>(font.size.y);
        fontInfo.FontFamily = static_cast<WORD>(font.family);
        fontInfo.FontWeight = static_cast<WORD>(font.weight);
        std::wstring wname(font.name.begin(), font.name.end());
        wcscpy_s(fontInfo.FaceName, wname.c_str());

        if (!SetCurrentConsoleFontEx(
            m_handles.output,
            FALSE,
            &fontInfo
        )) throw std::runtime_error("Failed to set console font");
    }

    std::string Console::getTitle() const {
        DWORD titleLength = GetConsoleTitleW(nullptr, 0);
        if (titleLength == 0) {
            throw std::runtime_error("Failed to get console title length");
        }

        std::vector<wchar_t> title(titleLength + 1);
        if (!GetConsoleTitleW(title.data(), static_cast<DWORD>(title.size()))) {
            throw std::runtime_error("Failed to get console title");
        }

        return wideStringToString(std::wstring(title.data()));
    }

    void Console::setTitle(const std::string &title) {
        std::wstring wide_title = stringToWideString(title);
        if (!SetConsoleTitleW(wide_title.c_str())) {
            throw std::runtime_error("Failed to set console title");
        }
    }

#endif // _WIN32

#ifdef __linux__

    std::vector<std::string> Console::findValidKeyboardDevices() {
        std::vector<std::string> devices;
        DIR *dir = opendir("/dev/input");
        if (!dir) return devices;

        struct dirent *ent;
        while ((ent = readdir(dir)) != nullptr) {
            if (std::strncmp(ent->d_name, "event", 5) == 0) {
                std::string path = "/dev/input/" + std::string(ent->d_name);
                int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
                if (fd >= 0) {
                    libevdev *dev = nullptr;
                    if (libevdev_new_from_fd(fd, &dev) == 0) {
                        if (libevdev_has_event_type(dev, EV_KEY) &&
                            libevdev_has_event_code(dev, EV_KEY, KEY_A)) {
                            devices.push_back(path);
                        }
                        libevdev_free(dev);
                    }
                    close(fd);
                }
            }
        }
        closedir(dir);
        return devices;
    }

    std::vector<std::string> Console::findValidMouseDevices() {
        std::vector<std::string> devices;
        DIR *dir = opendir("/dev/input");
        if (!dir) return devices;

        struct dirent *ent;
        while ((ent = readdir(dir)) != nullptr) {
            if (std::strncmp(ent->d_name, "event", 5) == 0) {
                std::string path = "/dev/input/" + std::string(ent->d_name);
                int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
                if (fd >= 0) {
                    libevdev *dev = nullptr;
                    if (libevdev_new_from_fd(fd, &dev) == 0) {
                        bool has_rel_x = libevdev_has_event_code(dev, EV_REL, REL_X);
                        bool has_rel_y = libevdev_has_event_code(dev, EV_REL, REL_Y);
                        bool has_rel_wheel = libevdev_has_event_code(dev, EV_REL, REL_WHEEL);
                        bool has_btn_left = libevdev_has_event_code(dev, EV_KEY, BTN_LEFT);
                        bool has_btn_right = libevdev_has_event_code(dev, EV_KEY, BTN_RIGHT);
                        bool has_btn_middle = libevdev_has_event_code(dev, EV_KEY, BTN_MIDDLE);
                        if (
                            libevdev_has_event_type(dev, EV_REL) ||
                            libevdev_has_event_type(dev, EV_KEY) ||
                            has_rel_x || has_rel_y || has_rel_wheel ||
                            has_btn_left || has_btn_right || has_btn_middle
                        ) {
                            devices.push_back(path);
                        }
                        libevdev_free(dev);
                    }
                    close(fd);
                }
            }
        }
        closedir(dir);
        return devices;
    }

    void Console::addKeyboardDevice(const std::string &devicePath) {
        for (auto &device : m_keyboardDevices) {
            if (device.path == devicePath) {
                throw std::runtime_error("Keyboard device already added: " + devicePath);
            }
        }

        DeviceData deviceData;
        deviceData.path = devicePath;

        deviceData.fd = open(devicePath.c_str(), O_RDONLY | O_NONBLOCK);
        if (deviceData.fd < 0) {
            throw std::runtime_error("Failed to open keyboard device: " + devicePath);
        }

        if (libevdev_new_from_fd(deviceData.fd, &deviceData.device) < 0) {
            close(deviceData.fd);
            throw std::runtime_error("Failed to initialize libevdev for keyboard");
        }

        m_keyboardDevices.push_back(deviceData);
    }

    void Console::addMouseDevice(const std::string &devicePath) {
        for (auto &device : m_mouseDevices) {
            if (device.path == devicePath) {
                throw std::runtime_error("Mouse device already added: " + devicePath);
            }
        }

        DeviceData deviceData;
        deviceData.path = devicePath;

        deviceData.fd = open(devicePath.c_str(), O_RDONLY | O_NONBLOCK);
        if (deviceData.fd < 0) {
            throw std::runtime_error("Failed to open mouse device: " + devicePath);
        }

        if (libevdev_new_from_fd(deviceData.fd, &deviceData.device) < 0) {
            close(deviceData.fd);
            throw std::runtime_error("Failed to initialize libevdev for mouse");
        }

        m_mouseDevices.push_back(deviceData);
    }

    void Console::removeKeyboardDevice(const std::string &devicePath) {
        for (auto it = m_keyboardDevices.begin(); it != m_keyboardDevices.end(); ++it) {
            if (it->path == devicePath) {
                libevdev_free(it->device);
                close(it->fd);
                m_keyboardDevices.erase(it);
                return;
            }
        }
        throw std::runtime_error("Keyboard device not found: " + devicePath);
    }

    void Console::removeMouseDevice(const std::string &devicePath) {
        for (auto it = m_mouseDevices.begin(); it != m_mouseDevices.end(); ++it) {
            if (it->path == devicePath) {
                libevdev_free(it->device);
                close(it->fd);
                m_mouseDevices.erase(it);
                return;
            }
        }
        throw std::runtime_error("Mouse device not found: " + devicePath);
    }

#endif // __linux__

    void Console::writeCharacterBuffer(const CharacterBuffer &buffer) {
#ifdef _WIN32
        SetConsoleCursorPosition(
            m_handles.output,
            { 0u, 0u }
        );

        DWORD characters_written;

        if (!WriteConsoleA(
            m_handles.output,
            buffer.getCharacters().data(),
            static_cast<DWORD>(buffer.getCharacters().size()),
            &characters_written,
            NULL
        )) {
            throw std::runtime_error("WriteConsole failed. Error code: " + std::to_string(GetLastError()));
        }
#endif // _WIN32
#if defined(__linux__) || defined(__APPLE__)
        std::cout << "\033[H";
        std::string output_buffer(buffer.getCharacters().begin(), buffer.getCharacters().end());
        std::cout << output_buffer;
#endif // __linux__ || __APPLE__
    }
    
    void Console::init() {
#ifdef _WIN32
        getHandles();

        if (!GetConsoleMode(m_handles.input, &m_firstInputMode))
            throw std::runtime_error("Failed to get initial console input mode");

        if (!GetConsoleMode(m_handles.output, &m_firstOutputMode))
            throw std::runtime_error("Failed to get initial console output mode");

        setInputMode();
        setOutputMode();
#endif // _WIN32

#if defined(__linux__) || defined(__APPLE__)
        setTerminalRawMode();
#endif

#ifdef __linux__
        for (const auto &devicePath : findValidKeyboardDevices()) {
            addKeyboardDevice(devicePath);
        }
        
        for (const auto &devicePath : findValidMouseDevices()) {
            addMouseDevice(devicePath);
        }
#endif // __linux__

#ifdef __APPLE__

        setupInputThread();

#endif // __APPLE__

        //clear();
    }

    void Console::reset() {
#ifdef _WIN32
        SetConsoleMode(
            m_handles.input,
            m_firstInputMode
        );

        SetConsoleMode(
            m_handles.output,
            m_firstOutputMode
        );
#endif // _WIN32

#if defined(__linux__) || defined(__APPLE__)
        resetTerminalMode();
#endif

#ifdef __linux__
        clearDevices();
#endif // __linux__

#ifdef __APPLE__
        stopInputThread();
#endif // __APPLE__
    }

#ifdef _WIN32

    void Console::getHandles() {
        m_handles.input = GetStdHandle(STD_INPUT_HANDLE);
        m_handles.output = GetStdHandle(STD_OUTPUT_HANDLE);

        if (
            m_handles.input == INVALID_HANDLE_VALUE || m_handles.input == nullptr ||
            m_handles.output == INVALID_HANDLE_VALUE || m_handles.output == nullptr
        ) throw std::runtime_error("Failed to get console handles");
    }
    
    void Console::setInputMode() {
        DWORD mode = 0u;

        mode = ENABLE_EXTENDED_FLAGS |
               ENABLE_MOUSE_INPUT    |
               ENABLE_WINDOW_INPUT;

        if (!SetConsoleMode(
            m_handles.input,
            mode
        )) throw std::runtime_error("Failed to set console input mode");  
    }

    void Console::setOutputMode() {
        DWORD mode = 0;
        if (!GetConsoleMode(m_handles.output, &mode))
            throw std::runtime_error("GetConsoleMode failed");

        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING
            | ENABLE_PROCESSED_OUTPUT
            | ENABLE_WRAP_AT_EOL_OUTPUT;
        if (!SetConsoleMode(m_handles.output, mode))
            throw std::runtime_error("SetConsoleMode failed");

        SetConsoleOutputCP(CP_UTF8);
    }

#endif // _WIN32

#if defined(__linux__) || defined(__APPLE__)

    void Console::setTerminalRawMode() {
        struct termios raw;
        if (tcgetattr(STDIN_FILENO, &raw) == -1) {
            throw std::runtime_error("Failed to get terminal attributes");
        }

        raw.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo

        if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1) {
            throw std::runtime_error("Failed to set terminal to raw mode");
        }
    }

    void Console::resetTerminalMode() {
        struct termios raw;
        if (tcgetattr(STDIN_FILENO, &raw) == -1) {
            throw std::runtime_error("Failed to get terminal attributes");
        }

        raw.c_lflag |= (ICANON | ECHO);

        if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1) {
            throw std::runtime_error("Failed to reset terminal mode");
        }
    }

#endif // __linux__ || __APPLE__

#ifdef __linux__

    void Console::clearDevices() {
        for (auto &device : m_keyboardDevices) {
            libevdev_free(device.device);
            close(device.fd);
        }
        m_keyboardDevices.clear();

        for (auto &device : m_mouseDevices) {
            libevdev_free(device.device);
            close(device.fd);
        }
        m_mouseDevices.clear();
    }

    void Console::processKeyboardDeviceEvents(fd_set &fds, DeviceData &deviceData, std::vector<Event> &events) {
        struct input_event inputEvent;
        
        if (FD_ISSET(deviceData.fd, &fds)) {
            while (libevdev_next_event(deviceData.device, LIBEVDEV_READ_FLAG_NORMAL, &inputEvent) == 0) {
                if (inputEvent.type == EV_KEY) {
                    KeyCode mappedKey = getKeyCodeFromLinuxKey(inputEvent.code);
                    if (mappedKey == KeyCode::Invalid) continue;

                    if (inputEvent.value == 1 && !m_keyStates[static_cast<u32>(mappedKey)]) {
                        Event event;
                        event.setType<KeyPressEvent>();
                        event.key = mappedKey;
                        events.push_back(event);
                        m_keyStates[static_cast<u32>(mappedKey)] = true;
                    } else if (inputEvent.value == 0 && m_keyStates[static_cast<u32>(mappedKey)]) {
                        Event event;
                        event.setType<KeyReleaseEvent>();
                        event.key = mappedKey;
                        events.push_back(event);
                        m_keyStates[static_cast<u32>(mappedKey)] = false;
                    }
                }
            }
        }
    }

    void Console::processMouseDeviceEvents(fd_set &fds, DeviceData &deviceData, std::vector<Event> &events) {
        struct input_event inputEvent;

        if (FD_ISSET(deviceData.fd, &fds)) {
            while (libevdev_next_event(deviceData.device, LIBEVDEV_READ_FLAG_NORMAL, &inputEvent) == 0) {
                if (inputEvent.type == EV_REL) {
                    if (inputEvent.code == REL_X) {
                        m_relativeMouseMovement.x += inputEvent.value;
                    } else if (inputEvent.code == REL_Y) {
                        m_relativeMouseMovement.y += inputEvent.value;
                    } else if (inputEvent.code == REL_WHEEL) {
                        Event event;
                        event.setType<MouseScrollEvent>();
                        event.mouseScrollDelta = inputEvent.value;
                        events.push_back(event);
                    }
                } else if (inputEvent.type == EV_ABS) {
                    if (inputEvent.code == ABS_X) {
                        m_currentMousePosition.x = inputEvent.value;
                    } else if (inputEvent.code == ABS_Y) {
                        m_currentMousePosition.y = inputEvent.value;
                    }
                } else if (inputEvent.type == EV_KEY) {
                    KeyCode mappedKey = getKeyCodeFromLinuxKey(inputEvent.code);
                    if (mappedKey == KeyCode::Invalid) continue;

                    if (inputEvent.value == 1 && !m_keyStates[static_cast<u32>(mappedKey)]) {
                        Event event;
                        event.setType<KeyPressEvent>();
                        event.key = mappedKey;
                        events.push_back(event);
                        m_keyStates[static_cast<u32>(mappedKey)] = true;
                    } else if (inputEvent.value == 0 && m_keyStates[static_cast<u32>(mappedKey)]) {
                        Event event;
                        event.setType<KeyReleaseEvent>();
                        event.key = mappedKey;
                        events.push_back(event);
                        m_keyStates[static_cast<u32>(mappedKey)] = false;
                    }
                }
            }
        }
    }

#endif // __linux__

#ifdef __APPLE__
    void Console::keyboardInputCallback(void *context, IOReturn result, void *sender, IOHIDValueRef event) {
        Console *console = static_cast<Console *>(context);

        IOHIDElementRef element = IOHIDValueGetElement(event);
        u32 usagePage = IOHIDElementGetUsagePage(element);
        u32 usage = IOHIDElementGetUsage(element);

        if (usagePage == kHIDPage_KeyboardOrKeypad) {
            int pressed = IOHIDValueGetIntegerValue(event);
            
            KeyCode mappedKey = getKeyCodeFromMacVK(usage);
            if (mappedKey == KeyCode::Invalid) return;

            if (pressed == 1 && !console->m_keyStates[static_cast<size_t>(mappedKey)]) {
                Event event;
                event.setType<KeyPressEvent>();
                event.key = mappedKey;
                console->m_keyStates[static_cast<size_t>(mappedKey)] = true;
                std::lock_guard<std::mutex> lock(console->m_pendingEventsMutex);
                console->m_pendingEvents.push_back(event);
            } else if (pressed == 0 && console->m_keyStates[static_cast<size_t>(mappedKey)]) {
                Event event;
                event.setType<KeyReleaseEvent>();
                event.key = mappedKey;
                console->m_keyStates[static_cast<size_t>(mappedKey)] = false;
                std::lock_guard<std::mutex> lock(console->m_pendingEventsMutex);
                console->m_pendingEvents.push_back(event);
            }
        }
    }

    CFMutableDictionaryRef Console::setupKeyboardDictionary() {
        CFMutableDictionaryRef keyboardDictionary = CFDictionaryCreateMutable(
            kCFAllocatorDefault,
            0,
            &kCFTypeDictionaryKeyCallBacks,
            &kCFTypeDictionaryValueCallBacks
        );

        int page = kHIDPage_GenericDesktop;
        CFDictionarySetValue(
            keyboardDictionary,
            CFSTR(kIOHIDDeviceUsagePageKey),
            CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &page)
        );
        int usage = kHIDUsage_GD_Keyboard;
        CFDictionarySetValue(
            keyboardDictionary,
            CFSTR(kIOHIDDeviceUsageKey),
            CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage)
        );

        return keyboardDictionary;
    }

    void Console::mouseInputCallback(void *context, IOReturn result, void *sender, IOHIDValueRef event) {
        Console *console = static_cast<Console *>(context);

        IOHIDElementRef element = IOHIDValueGetElement(event);
        u32 usagePage = IOHIDElementGetUsagePage(element);
        u32 usage = IOHIDElementGetUsage(element);

        if (usagePage == kHIDPage_Button) {
            int pressed = IOHIDValueGetIntegerValue(event);
            KeyCode mappedKey = getKeyCodeFromMacMouseVK(usage);
            if (mappedKey == KeyCode::Invalid) return;

            if (pressed == 1 && !console->m_keyStates[static_cast<size_t>(mappedKey)]) {
                Event event;
                event.setType<KeyPressEvent>();
                event.key = mappedKey;
                console->m_keyStates[static_cast<size_t>(mappedKey)] = true;
                std::lock_guard<std::mutex> lock(console->m_pendingEventsMutex);
                console->m_pendingEvents.push_back(event);
            } else if (pressed == 0 && console->m_keyStates[static_cast<size_t>(mappedKey)]) {
                Event event;
                event.setType<KeyReleaseEvent>();
                event.key = mappedKey;
                console->m_keyStates[static_cast<size_t>(mappedKey)] = false;
                std::lock_guard<std::mutex> lock(console->m_pendingEventsMutex);
                console->m_pendingEvents.push_back(event);
            }
        } else if (usagePage == kHIDPage_GenericDesktop) {
            int value = IOHIDValueGetIntegerValue(event);
            if (usage == kHIDUsage_GD_X) {
                std::lock_guard<std::mutex> lock(console->m_pendingEventsMutex);
                console->m_currentMousePosition.x += value;
            } else if (usage == kHIDUsage_GD_Y) {      
                std::lock_guard<std::mutex> lock(console->m_pendingEventsMutex);
                console->m_currentMousePosition.y += value;
            } else if (usage == kHIDUsage_GD_Wheel) {
                Event event;
                event.setType<MouseScrollEvent>();
                event.mouseScrollDelta = static_cast<i8>(value);
                std::lock_guard<std::mutex> lock(console->m_pendingEventsMutex);
                console->m_pendingEvents.push_back(event);
            }
        }
    }

    CFMutableDictionaryRef Console::setupMouseDictionary() {
        CFMutableDictionaryRef mouseDictionary = CFDictionaryCreateMutable(
            kCFAllocatorDefault,
            0,
            &kCFTypeDictionaryKeyCallBacks,
            &kCFTypeDictionaryValueCallBacks
        );

        int page = kHIDPage_GenericDesktop;
        CFDictionarySetValue(
            mouseDictionary,
            CFSTR(kIOHIDDeviceUsagePageKey),
            CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &page)
        );
        int usage = kHIDUsage_GD_Mouse;
        CFDictionarySetValue(
            mouseDictionary,
            CFSTR(kIOHIDDeviceUsageKey),
            CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage)
        );

        return mouseDictionary;
    }

    void Console::unifiedInputCallback(void *context, IOReturn result, void *sender, IOHIDValueRef event) {
        Console *console = static_cast<Console *>(context);
        IOHIDElementRef element = IOHIDValueGetElement(event);
        u32 usagePage = IOHIDElementGetUsagePage(element);
        u32 usage = IOHIDElementGetUsage(element);

        if (usagePage == kHIDPage_KeyboardOrKeypad) {
            Console::keyboardInputCallback(context, result, sender, event);
        } else if (usagePage == kHIDPage_Button || usagePage == kHIDPage_GenericDesktop) {
            Console::mouseInputCallback(context, result, sender, event);
        }
    }

    void Console::inputThreadFunction() {
        m_runLoop = CFRunLoopGetCurrent();

        IOHIDManagerScheduleWithRunLoop(m_hidManager, m_runLoop, kCFRunLoopDefaultMode);
        IOHIDManagerOpen(m_hidManager, kIOHIDOptionsTypeNone);
        CFRunLoopRun();
    }

    void Console::setupInputThread() {
        m_hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);

        auto mouseDictionary = setupMouseDictionary();
        auto keyboardDictionary = setupKeyboardDictionary();

        CFMutableArrayRef matchingArray = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
        CFArrayAppendValue(matchingArray, mouseDictionary);
        CFArrayAppendValue(matchingArray, keyboardDictionary);

        IOHIDManagerSetDeviceMatchingMultiple(m_hidManager, matchingArray);

        IOHIDManagerRegisterInputValueCallback(m_hidManager, Console::unifiedInputCallback, this);

        IOHIDManagerScheduleWithRunLoop(m_hidManager, m_runLoop, kCFRunLoopDefaultMode);
        IOHIDManagerOpen(m_hidManager, kIOHIDOptionsTypeNone);

        m_inputThread = std::thread(&Console::inputThreadFunction, this);
    }

    void Console::stopInputThread() {
        CFRunLoopStop(m_runLoop);
        m_inputThread.join();
        CFRelease(m_hidManager);
    }
#endif // __APPLE__

    void Console::clear() {
#ifdef _WIN32
        std::system("cls");
#elif defined(__linux__) || defined(__APPLE__)
        std::system("clear");
#endif // _WIN32
    }

    void Console::getEvents(std::vector<Event> &events) {
        events.clear();

#ifdef _WIN32
        DWORD numberOfEvents;

        if (!GetNumberOfConsoleInputEvents(
            m_handles.input,
            &numberOfEvents
        )) throw std::runtime_error("Failed to get number of console input events");

        std::vector<INPUT_RECORD> inputRecords(numberOfEvents);

        DWORD events_read;

        ReadConsoleInput(
            m_handles.input,
            inputRecords.data(),
            numberOfEvents,
            &events_read
        );

        parseInputRecords(inputRecords);
#endif // _WIN32

#ifdef __linux__
        struct input_event inputEvent;

        fd_set fds;
        FD_ZERO(&fds);

        int maxFd = -1;
        for (const auto &device : m_keyboardDevices) {
            FD_SET(device.fd, &fds);
            maxFd = std::max(maxFd, device.fd);
        }
        for (const auto &device : m_mouseDevices) {
            FD_SET(device.fd, &fds);
            maxFd = std::max(maxFd, device.fd);
        }

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        int ret = select(maxFd, &fds, nullptr, nullptr, &tv);

        if (ret > 0) {
            for (auto &device : m_mouseDevices) {
                processMouseDeviceEvents(fds, device, events);
            }

            Event event;
            event.setType<MouseMoveEvent>();

            Vector2<i32> absoluteDelta = m_currentMousePosition - m_lastMousePosition;
            if (absoluteDelta.magnitudeSquared() > m_relativeMouseMovement.magnitudeSquared()) {
                event.mouseDelta = absoluteDelta;
            } else {
                event.mouseDelta = m_relativeMouseMovement;
            }
            if (event.mouseDelta != Vector2<i32>{0, 0}) {
                events.push_back(event);
            }

            m_lastMousePosition = m_currentMousePosition;
            m_relativeMouseMovement = {0, 0};

            for (auto &device : m_keyboardDevices) {
                processKeyboardDeviceEvents(fds, device, events);
            }
        }
#endif // __linux__

        getMouseEvents(events);
        getKeyboardEvents(events);
        getConsoleEvents(events);
    }

    void Console::getMouseEvents(std::vector<Event> &events) {
#ifdef _WIN32
        for (const auto &record : m_mouseInputRecords) {
            Event event;
            DWORD currentMouseButtonState = record.Event.MouseEvent.dwButtonState;
            Vector2<i32> currentMousePosition{
                static_cast<i32>(record.Event.MouseEvent.dwMousePosition.X),
                static_cast<i32>(record.Event.MouseEvent.dwMousePosition.Y)
            };
            Vector2<i32> mouseDelta = currentMousePosition - m_lastMousePosition;
            m_lastMousePosition = currentMousePosition;

            event.setType<KeyPressEvent>();
            if (currentMouseButtonState & FROM_LEFT_1ST_BUTTON_PRESSED && !m_keyStates[static_cast<size_t>(KeyCode::LeftMouseButton)]) {
                event.key = KeyCode::LeftMouseButton;
                m_keyStates[static_cast<size_t>(KeyCode::LeftMouseButton)] = true;
                events.push_back(event);
            }
            if (currentMouseButtonState & RIGHTMOST_BUTTON_PRESSED && !m_keyStates[static_cast<size_t>(KeyCode::RightMouseButton)]) {
                event.key = KeyCode::RightMouseButton;
                m_keyStates[static_cast<size_t>(KeyCode::LeftMouseButton)] = true;
                events.push_back(event);
            }
            if (currentMouseButtonState & FROM_LEFT_2ND_BUTTON_PRESSED && !m_keyStates[static_cast<size_t>(KeyCode::MiddleMouseButton)]) {
                event.key = KeyCode::MiddleMouseButton;
                m_keyStates[static_cast<size_t>(KeyCode::LeftMouseButton)] = true;
                events.push_back(event);
            }

            event.setType<KeyReleaseEvent>();
            if (!(currentMouseButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) && (m_lastMouseButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)) {
                event.key = KeyCode::LeftMouseButton;
                m_keyStates[static_cast<size_t>(KeyCode::LeftMouseButton)] = false;
                events.push_back(event);
            }
            if (!(currentMouseButtonState & RIGHTMOST_BUTTON_PRESSED) && (m_lastMouseButtonState & RIGHTMOST_BUTTON_PRESSED)) {
                event.key = KeyCode::RightMouseButton;
                m_keyStates[static_cast<size_t>(KeyCode::LeftMouseButton)] = false;
                events.push_back(event);
            }
            if (!(currentMouseButtonState & FROM_LEFT_2ND_BUTTON_PRESSED) && (m_lastMouseButtonState & FROM_LEFT_2ND_BUTTON_PRESSED)) {
                event.key = KeyCode::MiddleMouseButton;
                m_keyStates[static_cast<size_t>(KeyCode::LeftMouseButton)] = false;
                events.push_back(event);
            }

            m_lastMouseButtonState = currentMouseButtonState;

            if (mouseDelta != Vector2<i32>{0, 0}) {
                event.setType<MouseMoveEvent>();
                event.mouseDelta = mouseDelta;
                events.push_back(event);
            }

            if (record.Event.MouseEvent.dwEventFlags & MOUSE_WHEELED) {
                event.setType<MouseScrollEvent>();
                i16 d = static_cast<i16>(HIWORD(record.Event.MouseEvent.dwButtonState));
                if (d > 0) {
                    event.mouseScrollDelta = 1;
                } else {
                    event.mouseScrollDelta = -1;
                }
                events.push_back(event);
            }
        }
#endif // _WIN32

#ifdef __APPLE__

        if (m_currentMousePosition != m_lastMousePosition) {
            Event event;
            event.setType<MouseMoveEvent>();
            event.mouseDelta = m_currentMousePosition - m_lastMousePosition;
            m_lastMousePosition = m_currentMousePosition;
            std::lock_guard<std::mutex> lock(m_pendingEventsMutex);
            events.push_back(event);
        }

#endif // __APPLE__
    }

    void Console::getKeyboardEvents(std::vector<Event> &events) {
#ifdef _WIN32
        for (u32 key = 0; key < static_cast<u32>(KeyCode::Count); ++key) {
            if (key == static_cast<u32>(KeyCode::LeftMouseButton) ||
                key == static_cast<u32>(KeyCode::RightMouseButton) ||
                key == static_cast<u32>(KeyCode::MiddleMouseButton)) {
                continue;
            }

            int code = getWinapiVK(static_cast<KeyCode>(key));
            if (code == -1) continue;
            bool was_pressed = (GetAsyncKeyState(getWinapiVK(static_cast<KeyCode>(key))) & 0x8000) != 0;

            if (was_pressed && !m_keyStates[key]) {
                Event event;
                event.setType<KeyPressEvent>();
                event.key = static_cast<KeyCode>(key);
                events.push_back(event);
                m_keyStates[key] = true;
            } else if (!was_pressed && m_keyStates[key]) {
                Event event;
                event.setType<KeyReleaseEvent>();
                event.key = static_cast<KeyCode>(key);
                events.push_back(event);
                m_keyStates[key] = false;
            }
        }
#endif // _WIN32

#ifdef __APPLE__

    std::lock_guard<std::mutex> lock(m_pendingEventsMutex);

    std::copy(m_pendingEvents.begin(), m_pendingEvents.end(), std::back_inserter(events));

    m_pendingEvents.clear();

#endif // __APPLE__
    }

    void Console::getConsoleEvents(std::vector<Event> &events) {
#ifdef _WIN32
        for (const auto &record : m_consoleInputRecords) {
            Event event;
            event.setType<ConsoleEvent>();
            event.newSize.x = record.Event.WindowBufferSizeEvent.dwSize.X;
            event.newSize.y = record.Event.WindowBufferSizeEvent.dwSize.Y;
            events.push_back(event);
        }
#endif // _WIN32

#if defined(__linux__) || defined(__APPLE__)
        Vector2<u32> size = getSize();
        if (size.x != m_currentConsoleSize.x || size.y != m_currentConsoleSize.y) {
            Event event;
            event.setType<ConsoleEvent>();
            event.newSize = size;
            events.push_back(event);
            m_currentConsoleSize = size;
        }
#endif // __linux__ || __APPLE__
    }

#ifdef _WIN32
    void Console::parseInputRecords(const std::vector<INPUT_RECORD> &inputRecords) {
        m_mouseInputRecords.clear();
        m_keyboardInputRecords.clear();
        m_consoleInputRecords.clear();

        for (const auto &record : inputRecords) {
            switch (record.EventType) {
                case MOUSE_EVENT:
                    m_mouseInputRecords.push_back(record);
                    break;
                case WINDOW_BUFFER_SIZE_EVENT:
                    m_consoleInputRecords.push_back(record);
                    break;
            }
        }
    }

#endif // _WIN32

    const std::array<bool, static_cast<size_t>(KeyCode::Count)> &Console::getKeyStates() const {
        return m_keyStates;
    }

#ifdef _WIN32
    std::wstring Console::stringToWideString(const std::string &str) const {      
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), NULL, 0);
        std::wstring wstr(size_needed, 0);
        int result = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), &wstr[0], size_needed);
        return wstr;
    }

    std::string Console::wideStringToString(const std::wstring &wstr) const {
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), NULL, 0, NULL, NULL);
        std::string str(size_needed, 0);
        int result = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), &str[0], size_needed, NULL, NULL);
        return str;
    }
#endif // _WIN32

}