#ifndef CGL_TEXT_HPP
#define CGL_TEXT_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include "color.hpp"
#include "texture.hpp"
#include "drawable.hpp"

namespace cgl
{
    /**
     * @class BitmapFont
     * @brief A bitmap font class for loading and rendering monospaced or proportional fonts from BDF files.
     *
     * Supports Unicode codepoints, per-glyph metrics, and rendering text to a texture with color and background.
     * Handles multi-line text, baseline alignment, and correct glyph placement using font metrics.
     * 
     * Doesn't support escape sequences, except for newline characters.
     */
    class BitmapFont
    {
    public:

        /**
         * @struct Glyph
         * @brief Represents a single glyph in the font, including metrics and bitmap data.
         *
         * - codepoint: Unicode codepoint for this glyph.
         * - size: Width and height of the glyph bitmap in pixels.
         * - offset: Offset from the baseline to the top-left corner of the bitmap.
         * - advance: Horizontal and vertical advance after rendering this glyph.
         * - bitmap: Row-major bitmap (left-to-right, top-to-bottom), true for set pixel.
         */
        struct Glyph
        {
            u32 codepoint;  ///< Unicode codepoint of the glyph
            Vector2<i32> size;  ///< Size of the glyph in pixels
            Vector2<i32> offset;  ///< Offset from the baseline to the top-left corner
            Vector2<i32> advance;  ///< Horizontal and vertical advance for the glyph
            std::vector<bool> bitmap;  ///< Bitmap representation of the glyph
        };

    public:

        /**
         * @brief Loads a bitmap font from a BDF file.
         * @param filepath Path to the BDF font file.
         * @throws std::runtime_error if the file cannot be opened or parsed.
         */
        void loadFromBDF(const std::string &filepath);

        /**
         * @brief Renders a UTF-8 string to a texture using this font.
         * @param text The text to render (UTF-8 encoded).
         * @param texture The target texture to render into (resized as needed).
         * @param color The foreground color for glyph pixels (default: white).
         * @param backgroundColor The background color for empty pixels (default: transparent).
         */
        void renderToTexture(
            const std::string &text,
            Texture &texture,
            Color color = Color(255, 255, 255, 255),
            Color backgroundColor = Color(0, 0, 0, 0)
        );

        /**
         * @brief Returns the glyph for a given Unicode codepoint.
         * @param codepoint The Unicode codepoint to look up.
         * @return Reference to the corresponding Glyph.
         * @throws std::runtime_error if the glyph is not found.
         */
        const Glyph &getGlyph(u32 codepoint) const;

    private:

        void textureRenderPrepass(const std::string &text, Texture &texture);

    private:

        std::unordered_map<u32, u32> m_glyphMap;
        std::vector<Glyph> m_glyphs;

        std::vector<Glyph> m_prepassGlyphBuffer;
        std::vector<std::pair<i32, i32>> m_ascentDescentPrepassBuffer;
    };
}

#endif // CGL_TEXT_HPP