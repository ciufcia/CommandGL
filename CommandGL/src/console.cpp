#include "cgl.hpp"

#include <stdexcept>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <cstdlib>

#if defined(__linux__) || defined(__APPLE__)
#include <sys/ioctl.h>
#include <unistd.h>
#include <libevdev/libevdev.h>
#include <fcntl.h>
#include <dirent.h>
#include <cstring>
#endif

namespace cgl
{
    Console::Console() {
        init();
    }
    
    Console::~Console() {
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

#ifdef __linux__
        libevdev_free(keyboardDevice);
        close(keyboardFd);
#endif // __linux__
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
    void Console::writeCharacterBuffer(const CharacterBuffer &buffer) {
#ifdef _WIN32
        SetConsoleCursorPosition(
            m_handles.output,
            { 0u, 0u }
        );

        DWORD characters_written;

        if (!WriteConsoleW(
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

#ifdef __linux__
        std::string devicePath = findKeyboardDevice();

        if (devicePath.empty()) {
            throw std::runtime_error("Failed to find keyboard device");
        }

        keyboardFd = open(devicePath.c_str(), O_RDONLY | O_NONBLOCK);

        if (keyboardFd < 0) {
            throw std::runtime_error("Failed to open keyboard device: " + devicePath);
        }

        if (libevdev_new_from_fd(keyboardFd, &keyboardDevice) < 0) {
            close(keyboardFd);
            throw std::runtime_error("Failed to initialize libevdev");
        }
#endif // __linux__

        clear();
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
    }

#endif // _WIN32

#ifdef __linux__
    std::string Console::findKeyboardDevice() {
        DIR *dir = opendir("/dev/input");
        if (!dir) return "";

        struct dirent *ent;
        while ((ent = readdir(dir)) != nullptr) {
            if (std::strncmp(ent->d_name, "event", 5) == 0) {
                std::string path = "/dev/input/" + std::string(ent->d_name);
                int fd = open(path.c_str(), O_RDONLY|O_NONBLOCK);
                if (fd >= 0) {
                    libevdev *dev = nullptr;
                    if (libevdev_new_from_fd(fd, &dev) == 0) {
                        const char* name = libevdev_get_name(dev);
                        if (name && std::strstr(name, "keyboard")) {
                            libevdev_free(dev);
                            close(fd);
                            closedir(dir);
                            return path;
                        }
                        libevdev_free(dev);
                    }
                    close(fd);
                }
            }
        }
        closedir(dir);
        return "";
    }
#endif // __linux__

    void Console::clear() {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif // _WIN32
    }

    void Console::destroy() {
#ifdef _WIN32
        if (!SetConsoleMode(
            m_handles.input,
            m_firstInputMode
        )) throw std::runtime_error("Failed to reset console input mode");

        if (!SetConsoleMode(
            m_handles.output,
            m_firstOutputMode
        )) throw std::runtime_error("Failed to reset console output mode");
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

        parseInputRecords(inputRecords, events);
#endif // _WIN32

        for (u32 key = 0; key < static_cast<u32>(KeyCode::Count); ++key) {
            if (key == static_cast<u32>(KeyCode::LeftMouseButton) ||
                key == static_cast<u32>(KeyCode::RightMouseButton) ||
                key == static_cast<u32>(KeyCode::MiddleMouseButton)) {
                continue;
            }
            bool was_pressed = false;

#ifdef _WIN32
            int code = getWinapiVK(static_cast<KeyCode>(key));
            if (code == -1) continue;
            was_pressed = (GetAsyncKeyState(getWinapiVK(static_cast<KeyCode>(key))) & 0x8000) != 0;
#endif // _WIN32

#ifdef __linux__
            int code = getLinuxKey(static_cast<KeyCode>(key));
            if (code == -1) continue;
            was_pressed = libevdev_fetch_event_value(keyboardDevice, EV_KEY, code, nullptr) == 1;
#endif // __linux__

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
    }

#ifdef _WIN32
    void Console::parseInputRecords(const std::vector<INPUT_RECORD> &inputRecords, std::vector<Event> &events) {
        for (const auto &record : inputRecords) {
            Event event;
            Vector2<u32> delta{0u, 0u};
            DWORD currentMouseButtonState = record.Event.MouseEvent.dwButtonState;

            switch (record.EventType) {
                case MOUSE_EVENT:

                    event.mousePosition.x = record.Event.MouseEvent.dwMousePosition.X;
                    event.mousePosition.y = record.Event.MouseEvent.dwMousePosition.Y;

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

                    delta = {
                        static_cast<u32>(record.Event.MouseEvent.dwMousePosition.X) - m_lastMousePosition.x,
                        static_cast<u32>(record.Event.MouseEvent.dwMousePosition.Y) - m_lastMousePosition.y
                    };

                    m_lastMousePosition = event.mousePosition;

                    if (delta != Vector2<u32>{0u, 0u}) {
                        event.setType<MouseMoveEvent>();
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

                    break;

                case WINDOW_BUFFER_SIZE_EVENT:

                    event.setType<ConsoleEvent>();
                    event.newSize.x = record.Event.WindowBufferSizeEvent.dwSize.X;
                    event.newSize.y = record.Event.WindowBufferSizeEvent.dwSize.Y;
                    events.push_back(event);
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