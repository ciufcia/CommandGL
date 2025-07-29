#ifndef CGL_CONSOLE_HPP
#define CGL_CONSOLE_HPP

#ifdef _WIN32
#include <windows.h>
#include "console_font.hpp"
#endif // _WIN32
#ifdef __linux__
#include <libevdev/libevdev.h>
#endif // __linux__
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

    private:

#ifdef _WIN32

        struct Handles
        {
            HANDLE input;
            HANDLE output;
        };

#endif // _WIN32

    public:

        Console();

        void init();

#ifdef _WIN32

        void getHandles();
        void setInputMode();
		void setOutputMode();

#endif // _WIN32

#ifdef __linux__
        std::string findKeyboardDevice();
#endif // __linux__

        void clear();

        void destroy();

        void writeCharacterBuffer(const CharacterBuffer &buffer);

        void getEvents(std::vector<Event> &events);

#ifdef _WIN32
        void parseInputRecords(const std::vector<INPUT_RECORD> &inputRecords, std::vector<Event> &events);
#endif // _WIN32

        const std::array<bool, static_cast<size_t>(KeyCode::Count)> &getKeyStates() const;

        std::wstring stringToWideString(const std::string &str) const;
        std::string wideStringToString(const std::wstring &wstr) const;

    private:
       
#ifdef _WIN32
        Handles m_handles;

        DWORD m_firstInputMode;
		DWORD m_firstOutputMode;
        
        DWORD m_lastMouseButtonState { 0u };
#endif // _WIN32

#ifdef __linux__

        libevdev *keyboardDevice { nullptr };
        int keyboardFd { -1 };

#endif // __linux__

        std::array<bool, static_cast<size_t>(KeyCode::Count)> m_keyStates { false };
        
        Vector2<u32> m_lastMousePosition { 0u, 0u };

    friend class EventManager;
    friend class Framework;
    };
}

#endif // CGL_CONSOLE_HPP