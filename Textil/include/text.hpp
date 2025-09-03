#ifndef TIL_TEXT_HPP
#define TIL_TEXT_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include "color.hpp"
#include "texture.hpp"

namespace til
{
    class BitmapFont
    {
    public:
        struct Glyph
        {
            u32 codepoint;
            Vector2<i32> size;
            Vector2<i32> offset;
            Vector2<i32> advance;
            std::vector<bool> bitmap;
        };

    public:

        void loadFromBDF(const std::string &filepath);
        void renderToTexture(
            const std::string &text,
            Texture &texture,
            Color color = Color(255, 255, 255, 255),
            Color backgroundColor = Color(0, 0, 0, 0)
        );
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

#endif // TIL_TEXT_HPP