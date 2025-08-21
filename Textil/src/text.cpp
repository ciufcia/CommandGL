#include "til.hpp"

#include <fstream>
#include <iostream>
#include "bit_manipulation.hpp"
#include "utf8.h"

namespace til
{
    void BitmapFont::loadFromBDF(const std::string &filepath)
    {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            invokeError<InvalidArgumentError>("Failed to open BDF file: " + filepath);
        }

        std::string line;

        Glyph currentGlyph;

        while (std::getline(file, line)) {
            if (line.empty()) {
                continue;
            }

            std::istringstream iss(line);
            std::string buffer;

            while (iss >> buffer) {
                if (buffer == "STARTCHAR") {
                    currentGlyph = Glyph();
                    continue;
                } else if (buffer == "ENCODING") {
                    iss >> currentGlyph.codepoint;
                    continue;
                } else if (buffer == "BBX") {
                    i32 width, height, xOffset, yOffset;
                    iss >> width >> height >> xOffset >> yOffset;
                    currentGlyph.size = { width, height };
                    currentGlyph.offset = { xOffset, yOffset };
                    continue;
                } else if (buffer == "DWIDTH") {
                    i32 xAdvance, yAdvance;
                    iss >> xAdvance >> yAdvance;
                    currentGlyph.advance = { xAdvance, yAdvance };
                } else if (buffer == "BITMAP") {

                    currentGlyph.bitmap.clear();
                    for (i32 y = 0; y < currentGlyph.size.y; ++y) {
                        std::getline(file, line);

                        std::vector<bool> rowBits;
                        int totalBits = static_cast<int>(line.length()) * 4;
                        for (int i = 0; i < line.length(); ++i) {
                            u8 byte = 0;
                            char hexChar = line[i];
                            if (hexChar >= '0' && hexChar <= '9') {
                                byte = hexChar - '0';
                            } else if (hexChar >= 'A' && hexChar <= 'F') {
                                byte = hexChar - 'A' + 10;
                            } else if (hexChar >= 'a' && hexChar <= 'f') {
                                byte = hexChar - 'a' + 10;
                            } else {
                                invokeError<InvalidArgumentError>("Invalid hex character: " + std::string(1, hexChar));
                            }
                            
                            for (int b = 3; b >= 0; --b) {
                                rowBits.push_back((byte >> b) & 1);
                            }
                        }

                        for (int x = 0; x < currentGlyph.size.x; ++x) {
                            currentGlyph.bitmap.push_back(rowBits[x]);
                        }
                    }

                    continue;
                } else if (buffer == "ENDCHAR") {
                    m_glyphMap[currentGlyph.codepoint] = m_glyphs.size();
                    m_glyphs.push_back(currentGlyph);
                    continue;
                }
            }
        }
    }

    void BitmapFont::renderToTexture(
        const std::string &text,
        Texture &texture,
        Color color,
        Color backgroundColor
    ) {
        textureRenderPrepass(text, texture);

        if (text.empty()) {
            return;
        }

        for (i32 y = 0; y < texture.getSize().y; ++y) {
            for (i32 x = 0; x < texture.getSize().x; ++x) {
                texture.setPixel({static_cast<u32>(x), static_cast<u32>(y)}, backgroundColor);
            }
        }

        Vector2<i32> position = {0, m_ascentDescentPrepassBuffer[0].first};

        i32 lineNumber = 0;

        for (const Glyph &glyph : m_prepassGlyphBuffer) {
            if (glyph.codepoint == '\n') {
                position.x = 0;
                position.y += m_ascentDescentPrepassBuffer[lineNumber].second + m_ascentDescentPrepassBuffer[lineNumber + 1].first;
                lineNumber++;
                continue;
            }

            for (i32 y = 0; y < glyph.size.y; ++y) {
                for (i32 x = 0; x < glyph.size.x; ++x) {
                    if (glyph.bitmap[y * glyph.size.x + x]) {
                        texture.setPixel(
                            { static_cast<u32>(position.x + glyph.offset.x + x), static_cast<u32>(position.y + glyph.offset.y + y) },
                            color
                        );
                    }
                }
            }

            position.x += glyph.advance.x;
        }
    }

    void BitmapFont::textureRenderPrepass(const std::string &text, Texture &texture) {
        m_prepassGlyphBuffer.clear();
        m_ascentDescentPrepassBuffer.clear();

        if (text.empty()) {
            texture.setSize({0, 0});
            return;
        }

        Vector2<i32> size {0, 0};

        i32 currentLineWidth = 0;

        bool firstCharacterInLine = true;

        i32 lineNumber = 0;

        m_ascentDescentPrepassBuffer.push_back({0, 0});

        for (
            utf8::iterator<std::string::const_iterator> it(text.begin(), text.begin(), text.end());
            it != utf8::iterator<std::string::const_iterator>(text.end(), text.begin(), text.end());
            ++it
        ) {
            u32 codepoint = *it;

            if (codepoint == '\n') {
                size.x = std::max(currentLineWidth, size.x);

                currentLineWidth = 0;

                lineNumber++;
                m_ascentDescentPrepassBuffer.push_back({0, 0});

                Glyph currentGlyph;
                currentGlyph.codepoint = codepoint;

                m_prepassGlyphBuffer.push_back(currentGlyph);

                firstCharacterInLine = true;

                continue;
            }

            Glyph currentGlyph = getGlyph(codepoint);

            if (firstCharacterInLine) {
                firstCharacterInLine = false;
                currentLineWidth += currentGlyph.offset.x;
            }

            m_prepassGlyphBuffer.push_back(currentGlyph);

            currentLineWidth += currentGlyph.advance.x;

            m_ascentDescentPrepassBuffer[lineNumber].first = std::max(
                m_ascentDescentPrepassBuffer[lineNumber].first,
                -currentGlyph.offset.y
            );

            m_ascentDescentPrepassBuffer[lineNumber].second = std::max(
                m_ascentDescentPrepassBuffer[lineNumber].second,
                currentGlyph.size.y + currentGlyph.offset.y
            );
        }

        size.x = std::max(currentLineWidth, size.x);

        for (auto ascentDescent : m_ascentDescentPrepassBuffer) {
            size.y += ascentDescent.first + ascentDescent.second;
        }

        texture.setSize({ static_cast<u32>(size.x), static_cast<u32>(size.y) });
    }

    const BitmapFont::Glyph &BitmapFont::getGlyph(u32 codepoint) const {
        auto it = m_glyphMap.find(codepoint);

        if (it != m_glyphMap.end()) {
            return m_glyphs[it->second];
        } else {
            invokeError<InvalidArgumentError>("Glyph not found for codepoint: " + std::to_string(codepoint));
            return m_glyphs[0]; // Return a default glyph to avoid compiler warning
        }
    }
} // namespace til