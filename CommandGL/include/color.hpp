#ifndef CGL_COLOR_HPP
#define CGL_COLOR_HPP

#include "numeric_types.hpp"

namespace cgl
{
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
    class Color
    {
    public:
        u8 r = 255;

        u8 g = 0;

        u8 b = 255;

        u8 a = 255;
        Color() = default;
        Color(u8 red, u8 green, u8 blue, u8 alpha = 255) : r(red), g(green), b(blue), a(alpha) {}
        Color(u32 hex);
        u32 toHex() const;
        void fromHex(u32 hex);
        f32 luminance() const;
        Color inverted() const;
        bool operator==(const Color& other) const;

        bool operator!=(const Color& other) const;
        static Color applyBlend(Color destination, Color source, BlendMode blendMode);
        static Color noBlend(Color destination, Color source);

        static Color alphaBlend(Color destination, Color source);

        static Color additiveBlend(Color destination, Color source);

        static Color multiplicativeBlend(Color destination, Color source);

        static Color subtractiveBlend(Color destination, Color source);

        static Color screenBlend(Color destination, Color source);

        static Color overlayBlend(Color destination, Color source);
    };
}

#endif // CGL_COLOR_HPP