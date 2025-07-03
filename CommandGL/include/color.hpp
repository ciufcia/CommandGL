#ifndef CGL_COLOR_HPP
#define CGL_COLOR_HPP

#include "numeric_types.hpp"

namespace cgl
{
    /**
     * @enum BlendMode
     * @brief Enumeration of different blending modes for color composition.
     */
    enum class BlendMode
    {
        None,
        Alpha,
        Additive,
        Multiplicative,
        Subtractive,
        Screen,
        Overlay
    };

    /**
     * @class Color
     * @brief Represents an RGBA color with 8-bit channels and blending operations.
     */
    class Color
    {
    public:

        /**
         * @brief Red color component (0-255).
         */
        u8 r = 255;
        
        /**
         * @brief Green color component (0-255).
         */
        u8 g = 0;
        
        /**
         * @brief Blue color component (0-255).
         */
        u8 b = 255;
        
        /**
         * @brief Alpha (transparency) component (0-255, where 0 is fully transparent).
         */
        u8 a = 255;

        /**
         * @brief Default constructor creates a magenta color (255, 0, 255, 255).
         */
        Color() = default;

        /**
         * @brief Constructor that initializes the color with given RGBA values.
         * @param red The red component (0-255).
         * @param green The green component (0-255).
         * @param blue The blue component (0-255).
         * @param alpha The alpha component (0-255), defaults to 255 (fully opaque).
         */
        Color(u8 red, u8 green, u8 blue, u8 alpha = 255) : r(red), g(green), b(blue), a(alpha) {}

        /**
         * @brief Constructor that initializes the color from a 32-bit hexadecimal value.
         * @param hex The hexadecimal color value (e.g., 0xRRGGBBAA).
         */
        Color(u32 hex);

        /**
         * @brief Converts the color to a 32-bit hexadecimal value.
         * @return The color as a hexadecimal value (0xRRGGBBAA format).
         */
        u32 toHex() const;

        /**
         * @brief Sets the color from a 32-bit hexadecimal value.
         * @param hex The hexadecimal color value to set.
         */
        void fromHex(u32 hex);

        /**
         * @brief Calculates the luminance (perceived brightness) of the color.
         * @return The luminance value as a floating-point number (0.0-1.0).
         */
        f32 luminance() const;

        /**
         * @brief Equality comparison operator.
         * @param other The color to compare with.
         * @return True if all RGBA components are equal, false otherwise.
         */
        bool operator==(const Color& other) const;
        
        /**
         * @brief Inequality comparison operator.
         * @param other The color to compare with.
         * @return True if any RGBA component differs, false otherwise.
         */
        bool operator!=(const Color& other) const;

        /**
         * @brief Applies the specified blend mode to combine two colors.
         * @param destination The destination (background) color.
         * @param source The source (foreground) color.
         * @param blendMode The blending mode to apply.
         * @return The resulting blended color.
         */
        static Color applyBlend(Color destination, Color source, BlendMode blendMode);

        /**
         * @brief No blending - returns the source color unchanged.
         * @param destination The destination color (ignored).
         * @param source The source color.
         * @return The source color.
         */
        static Color noBlend(Color destination, Color source);
        
        /**
         * @brief Alpha blending using the source color's alpha channel.
         * @param destination The destination color.
         * @param source The source color.
         * @return The alpha-blended result.
         */
        static Color alphaBlend(Color destination, Color source);
        
        /**
         * @brief Additive blending - adds color values together (clamped to 255).
         * @param destination The destination color.
         * @param source The source color.
         * @return The additive blend result.
         */
        static Color additiveBlend(Color destination, Color source);
        
        /**
         * @brief Multiplicative blending - multiplies color values together.
         * @param destination The destination color.
         * @param source The source color.
         * @return The multiplicative blend result.
         */
        static Color multiplicativeBlend(Color destination, Color source);
        
        /**
         * @brief Subtractive blending - subtracts source from destination (clamped to 0).
         * @param destination The destination color.
         * @param source The source color.
         * @return The subtractive blend result.
         */
        static Color subtractiveBlend(Color destination, Color source);
        
        /**
         * @brief Screen blending - always produces a brighter result than either input.
         * @param destination The destination color.
         * @param source The source color.
         * @return The screen blend result.
         */
        static Color screenBlend(Color destination, Color source);
        
        /**
         * @brief Overlay blending - combines multiply and screen modes based on destination brightness.
         * @param destination The destination color.
         * @param source The source color.
         * @return The overlay blend result.
         */
        static Color overlayBlend(Color destination, Color source);
    };
}

#endif // CGL_COLOR_HPP