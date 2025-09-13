/**
 * @file color.hpp
 * @brief Color representation and blending operations for Textil library
 * @details Provides RGBA color representation and various blending modes for advanced
 *          color composition effects in terminal graphics rendering.
 */

#ifndef TIL_COLOR_HPP
#define TIL_COLOR_HPP

#include "numeric_types.hpp"

namespace til
{
    /**
     * @brief Enumeration of available color blending modes
     * @details Defines different algorithms for combining source and destination colors.
     *          Each mode produces different visual effects when compositing colors.
     */
    enum class BlendMode
    {
        None,           ///< No blending - source color completely replaces destination
        Alpha,          ///< Standard alpha blending using source alpha channel
        Additive,       ///< Add source color components to destination (brightening effect)
        Multiplicative, ///< Multiply source and destination color components (darkening effect)
        Subtractive,    ///< Subtract source color from destination (darkening effect)
        Screen,         ///< Screen blending mode (inverted multiply, brightening effect)
        Overlay         ///< Overlay blending combining multiply and screen based on destination
    };
    
    /**
     * @brief RGBA color representation with blending operations
     * @details Represents colors using 8-bit red, green, blue, and alpha channels (0-255 range).
     *          Provides color manipulation, conversion utilities, and static blending functions
     *          for compositing operations. Default constructor creates magenta (255,0,255,255) 
     *          for easy identification of uninitialized colors.
     */
    class Color
    {
    public:
        u8 r = 255; ///< Red component (0-255)
        u8 g = 0;   ///< Green component (0-255)  
        u8 b = 255; ///< Blue component (0-255)
        u8 a = 255; ///< Alpha component (0-255, 255 = fully opaque)
        
        /**
         * @brief Default constructor creating magenta color
         * @details Creates Color(255, 0, 255, 255) - bright magenta with full opacity.
         *          This distinctive color helps identify uninitialized or default colors.
         */
        Color() = default;
        
        /**
         * @brief Construct color with RGBA components
         * @param red Red component (0-255)
         * @param green Green component (0-255)  
         * @param blue Blue component (0-255)
         * @param alpha Alpha component (0-255), defaults to 255 (fully opaque)
         * @details Creates a color with specified component values. Alpha defaults to fully opaque.
         */
        Color(u8 red, u8 green, u8 blue, u8 alpha = 255) : r(red), g(green), b(blue), a(alpha) {}
        
        /**
         * @brief Construct color from hexadecimal value
         * @param hex 32-bit hexadecimal color value (0xAARRGGBB format)
         * @details Extracts RGBA components from hex value. Format: 0xAARRGGBB where
         *          AA=alpha, RR=red, GG=green, BB=blue. For RGB-only hex values, 
         *          alpha is assumed to be 255 (fully opaque).
         */
        Color(u32 hex);
        
        /**
         * @brief Convert color to hexadecimal representation
         * @return 32-bit hexadecimal value in 0xAARRGGBB format
         * @details Packs RGBA components into a single 32-bit value for storage or transmission.
         */
        u32 toHex() const;
        
        /**
         * @brief Set color from hexadecimal value
         * @param hex 32-bit hexadecimal color value to parse
         * @details Updates this color's RGBA components from the provided hex value.
         *          Same format as hex constructor: 0xAARRGGBB.
         */
        void fromHex(u32 hex);
        
        /**
         * @brief Calculate relative luminance of the color
         * @return Luminance value as floating-point (0.0 = black, 1.0 = white)
         * @details Computes perceived brightness using standard RGB to luminance conversion:
         *          Y = 0.299*R + 0.587*G + 0.114*B. Useful for determining text contrast
         *          and automatic color adjustments.
         */
        f32 luminance() const;
        
        /**
         * @brief Get color-inverted version of this color
         * @return New color with inverted RGB components (alpha unchanged)
         * @details Creates photographic negative effect by inverting each RGB component:
         *          new_component = 255 - old_component. Alpha channel remains unchanged.
         */
        Color inverted() const;
        
        /**
         * @brief Equality comparison operator
         * @param other Color to compare with
         * @return True if all RGBA components are identical
         */
        bool operator==(const Color& other) const;

        /**
         * @brief Inequality comparison operator  
         * @param other Color to compare with
         * @return True if any RGBA component differs
         */
        bool operator!=(const Color& other) const;
        
        /**
         * @brief Apply specified blending mode between two colors
         * @param destination Base color (background)
         * @param source Overlay color (foreground) 
         * @param blendMode Blending algorithm to use
         * @return Resulting blended color
         * @details Master blending function that dispatches to specific blend implementations
         *          based on the selected mode. Handles all supported BlendMode variants.
         */
        static Color applyBlend(Color destination, Color source, BlendMode blendMode);
        
        /**
         * @brief No blending - source replaces destination
         * @param destination Base color (ignored)
         * @param source Overlay color
         * @return Source color unchanged
         * @details Simple replacement operation where source color completely overwrites destination.
         */
        static Color noBlend(Color destination, Color source);

        /**
         * @brief Standard alpha blending
         * @param destination Base color (background)
         * @param source Overlay color with alpha channel
         * @return Blended color based on source alpha
         * @details Implements standard Porter-Duff alpha compositing:
         *          result = source * source_alpha + destination * (1 - source_alpha)
         */
        static Color alphaBlend(Color destination, Color source);

        /**
         * @brief Additive color blending  
         * @param destination Base color
         * @param source Color to add
         * @return Color with added components (clamped to 255)
         * @details Adds corresponding RGB components together, creating brightening effect.
         *          Components are clamped to maximum value of 255 to prevent overflow.
         */
        static Color additiveBlend(Color destination, Color source);

        /**
         * @brief Multiplicative color blending
         * @param destination Base color  
         * @param source Color to multiply with
         * @return Color with multiplied components
         * @details Multiplies corresponding RGB components, creating darkening effect.
         *          Formula: result = (dest * source) / 255 for each component.
         */
        static Color multiplicativeBlend(Color destination, Color source);

        /**
         * @brief Subtractive color blending
         * @param destination Base color
         * @param source Color to subtract  
         * @return Color with subtracted components (clamped to 0)
         * @details Subtracts source RGB components from destination, creating darkening effect.
         *          Components are clamped to minimum value of 0 to prevent underflow.
         */
        static Color subtractiveBlend(Color destination, Color source);

        /**
         * @brief Screen blending mode
         * @param destination Base color
         * @param source Overlay color
         * @return Screened color (brightening effect)
         * @details Inverted multiply operation that brightens the image:
         *          result = 255 - ((255-dest) * (255-source)) / 255
         */
        static Color screenBlend(Color destination, Color source);

        /**
         * @brief Overlay blending mode  
         * @param destination Base color
         * @param source Overlay color
         * @return Overlaid color combining multiply and screen
         * @details Combines multiply and screen modes based on destination luminance.
         *          Dark areas use multiply (darken), light areas use screen (brighten).
         */
        static Color overlayBlend(Color destination, Color source);
    };
}

#endif // TIL_COLOR_HPP