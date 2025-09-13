/**
 * @file text.hpp
 * @brief Bitmap font loading and text rendering for Textil library
 * 
 * @details Provides bitmap font functionality supporting BDF (Bitmap Distribution Format) files
 * and text rendering to textures. Designed for pixel-perfect text rendering in terminal
 * graphics applications with support for Unicode text via UTF-8 encoding.
 */

#ifndef TIL_TEXT_HPP
#define TIL_TEXT_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include "color.hpp"
#include "texture.hpp"

namespace til
{
    /**
     * @brief Bitmap font class for loading and rendering pixel-perfect text
     * @details Supports loading bitmap fonts from BDF (Bitmap Distribution Format) files
     *          and rendering text strings to textures. Each character is stored as a bitmap
     *          with precise positioning information for high-quality text layout.
     *          
     *          BDF is a widely-used format for bitmap fonts, originally from the X Window System.
     *          The implementation supports Unicode codepoints and handles UTF-8 encoded text strings.
     */
    class BitmapFont
    {
    public:
        /**
         * @brief Structure representing a single character glyph
         * @details Contains all information needed to render a character, including bitmap data,
         *          positioning offsets, and advance width for proper text layout.
         */
        struct Glyph
        {
            u32 codepoint;              ///< Unicode codepoint this glyph represents
            Vector2<i32> size;          ///< Glyph dimensions in pixels (width, height)
            Vector2<i32> offset;        ///< Offset from baseline to glyph origin (x, y)
            Vector2<i32> advance;       ///< Distance to advance cursor after rendering (x, y)
            std::vector<bool> bitmap;   ///< Glyph bitmap data in row-major order (true = foreground pixel)
        };

    public:
        /**
         * @brief Load bitmap font from BDF file
         * @param filepath Path to BDF font file
         * @throws InvalidArgumentError if file cannot be opened or parsed
         * @details Parses BDF file format and extracts all character glyphs with their positioning data.
         *          BDF files contain ASCII-based font definitions with bitmap data for each character.
         *          The function builds internal lookup tables for efficient character access during rendering.
         * 
         *          Supported BDF features:
         *          - Character encoding and Unicode codepoints
         *          - Bitmap dimensions and positioning offsets  
         *          - Character advance widths for proper spacing
         *          - Baseline and bounding box information
         */
        void loadFromBDF(const std::string &filepath);
        
        /**
         * @brief Render text string to texture with specified colors
         * @param text UTF-8 encoded text string to render
         * @param texture Target texture to render into (automatically resized)
         * @param color Foreground color for text pixels (default: white)
         * @param backgroundColor Background color for non-text pixels (default: transparent)
         * @details Renders the provided text string into the texture using loaded glyph data.
         *          The texture is automatically resized to fit the rendered text dimensions.
         *          
         *          Text layout follows standard typographic rules:
         *          - Characters are positioned using glyph offset and advance information
         *          - Baseline alignment ensures consistent vertical positioning
         *          - UTF-8 decoding handles multi-byte Unicode characters
         * 
         *          Performance considerations:
         *          - Large text strings may require significant texture memory
         *          - Consider using texture atlases for frequently rendered text
         */
        void renderToTexture(
            const std::string &text,
            Texture &texture,
            Color color = Color(255, 255, 255, 255),
            Color backgroundColor = Color(0, 0, 0, 0)
        );
        
        /**
         * @brief Get glyph data for specific Unicode codepoint
         * @param codepoint Unicode codepoint to look up
         * @return Reference to glyph data structure
         * @throws InvalidArgumentError if codepoint is not available in font
         * @details Provides direct access to glyph data for custom text rendering or analysis.
         *          Useful for implementing custom text layout algorithms or measuring text dimensions.
         */
        const Glyph &getGlyph(u32 codepoint) const;

    private:
        /**
         * @brief Perform sizing prepass to determine texture dimensions
         * @param text Text string to measure
         * @param texture Target texture to resize
         * @details Internal function that calculates required texture size by measuring text layout.
         *          Handles line breaks, character spacing, and overall bounding box calculation.
         *          Must be called before actual rendering to ensure adequate texture size.
         */
        void textureRenderPrepass(const std::string &text, Texture &texture);

    private:
        /// Maps Unicode codepoints to indices in the glyph vector for fast lookup
        std::unordered_map<u32, u32> m_glyphMap;
        
        /// Storage for all loaded character glyphs with their rendering data
        std::vector<Glyph> m_glyphs;

        /// Temporary buffer for glyph data during text measurement operations
        std::vector<Glyph> m_prepassGlyphBuffer;
        
        /// Temporary buffer for ascent/descent calculations during text layout
        std::vector<std::pair<i32, i32>> m_ascentDescentPrepassBuffer;
    };
}

#endif // TIL_TEXT_HPP