#ifndef CGL_CONSOLE_FONT_HPP
#define CGL_CONSOLE_FONT_HPP

#include <string>
#include "vector2.hpp"
#include "numeric_types.hpp"

namespace cgl
{
    /**
     * @enum ConsoleFontWeight
     * @brief Wrapper for Windows font weight constants.
     */
    enum class ConsoleFontWeight : u32 {
        Thin = 100,
        ExtraLight = 200,
        Light = 300,
        Normal = 400, ///< Equivalent to FW_NORMAL
        Medium = 500,
        SemiBold = 600,
        Bold = 700,   ///< Equivalent to FW_BOLD
        ExtraBold = 800,
        Black = 900
    };

    /**
     * @enum ConsoleFontFamily
     * @brief Wrapper for Windows font family constants.
     */
    enum class ConsoleFontFamily : u32 {
        DontCare = 0x00, ///< Equivalent to FF_DONTCARE
        Roman    = 0x10, ///< Equivalent to FF_ROMAN
        Swiss    = 0x20, ///< Equivalent to FF_SWISS
        Modern   = 0x30, ///< Equivalent to FF_MODERN
        Script   = 0x40, ///< Equivalent to FF_SCRIPT
        Decorative = 0x50 ///< Equivalent to FF_DECORATIVE
    };

    /**
     * @class ConsoleFont
     * @brief Represents a font configuration for the Windows console.
     *
     * This class encapsulates the font name, size, weight, and family used for the Windows console.
     */
    class ConsoleFont
    {
    public:
        /**
         * @brief Default constructor. Initializes with default font settings (Consolas, size 16, normal weight, don't care family).
         */
        ConsoleFont() = default;

        /**
         * @brief Constructs a ConsoleFont with the specified parameters.
         * @param name The name of the font (e.g., "Consolas").
         * @param size The font cell size as a Vector2<u32> (width, height).
         * @param weight The font weight (ConsoleFontWeight enum).
         * @param family The font family (ConsoleFontFamily enum).
         */
        ConsoleFont(std::string name, Vector2<u32> size, ConsoleFontWeight weight = ConsoleFontWeight::Normal, ConsoleFontFamily family = ConsoleFontFamily::DontCare)
            : name(std::move(name)), size(size), weight(weight), family(family) {}

        /**
         * @brief The name of the font (e.g., "Consolas").
         */
        std::string name = "Consolas";
        /**
         * @brief The font cell size in character cells (width, height).
         */
        Vector2<u32> size = {8, 16};
        /**
         * @brief The font weight (ConsoleFontWeight enum).
         */
        ConsoleFontWeight weight = ConsoleFontWeight::Normal;
        /**
         * @brief The font family (ConsoleFontFamily enum).
         */
        ConsoleFontFamily family = ConsoleFontFamily::DontCare;
    };
}

#endif // CGL_CONSOLE_FONT_HPP