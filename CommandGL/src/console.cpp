#include "cgl.hpp"

#include <stdexcept>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <cstdlib>

namespace cgl
{
    Console::Console() {
        init();
    }
    
    Console::~Console() {     
        SetConsoleMode(
            m_handles.input,
            m_firstInputMode
        );

        SetConsoleMode(
            m_handles.output,
            m_firstOutputMode
        );
    }
    
    Vector2<u32> Console::getSize() const {
        CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
        if (!GetConsoleScreenBufferInfo(
            m_handles.output,
            &consoleScreenBufferInfo
        )) throw std::runtime_error("Failed to get console screen buffer info");

        return { static_cast<u32>(consoleScreenBufferInfo.dwSize.X), static_cast<u32>(consoleScreenBufferInfo.dwSize.Y) };
    }
    
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

    void Console::writeCharacterBuffer(const CharacterBuffer &buffer) {
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
    }
    
    void Console::init() {
        getHandles();

        if (!GetConsoleMode(m_handles.input, &m_firstInputMode))
            throw std::runtime_error("Failed to get initial console input mode");

        if (!GetConsoleMode(m_handles.output, &m_firstOutputMode))
			throw std::runtime_error("Failed to get initial console output mode");

        setInputMode();
		setOutputMode();

        clear();
    }

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
    
    void Console::clear() {
        std::system("cls");
    }

    void Console::destroy() {
        if (!SetConsoleMode(
            m_handles.input,
            m_firstInputMode
        )) throw std::runtime_error("Failed to reset console input mode");

        if (!SetConsoleMode(
            m_handles.output,
            m_firstOutputMode
        )) throw std::runtime_error("Failed to reset console output mode");
    }

    void Console::getEvents(std::vector<Event> &events) {
        events.clear();

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

        return parseInputRecords(inputRecords, events);
    }

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

        for (u32 key = 0; key < static_cast<u32>(KeyCode::Count); ++key)
        {
            if (key == static_cast<u32>(KeyCode::LeftMouseButton) ||
                key == static_cast<u32>(KeyCode::RightMouseButton) ||
                key == static_cast<u32>(KeyCode::MiddleMouseButton)) {
                continue;
            }
            bool was_pressed = (GetAsyncKeyState(key) & 0x8000) != 0;

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

    const std::array<bool, static_cast<size_t>(KeyCode::Count)> &Console::getKeyStates() const {
        return m_keyStates;
    }

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
}