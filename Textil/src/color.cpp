#include "til.hpp"
#include <algorithm>

namespace til
{
    Color::Color(u32 hex) {
        fromHex(hex);
    }

    u32 Color::toHex() const {
        return (static_cast<u32>(r) << 16) |
               (static_cast<u32>(g) << 8) |
               (static_cast<u32>(b));
    }

    void Color::fromHex(u32 hex) {
        r = static_cast<u8>((hex >> 16) & 0xFF);
        g = static_cast<u8>((hex >> 8) & 0xFF);
        b = static_cast<u8>(hex & 0xFF);
        a = 255;
    }

    f32 Color::luminance() const {
        f32 rNorm = static_cast<f32>(r) / 255.0f;
        f32 gNorm = static_cast<f32>(g) / 255.0f;
        f32 bNorm = static_cast<f32>(b) / 255.0f;
        
        return 0.2126f * rNorm + 0.7152f * gNorm + 0.0722f * bNorm;
    }

    Color Color::inverted() const {
        return Color(255 - r, 255 - g, 255 - b, a);
    }

    bool Color::operator==(const Color& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    bool Color::operator!=(const Color& other) const {
        return !(*this == other);
    }

    Color Color::applyBlend(Color destination, Color source, BlendMode blendMode) {
        switch (blendMode)
        {
            case BlendMode::None:
                return noBlend(destination, source);
            case BlendMode::Alpha:
                return alphaBlend(destination, source);
            case BlendMode::Additive:
                return additiveBlend(destination, source);
            case BlendMode::Multiplicative:
                return multiplicativeBlend(destination, source);
            case BlendMode::Subtractive:
                return subtractiveBlend(destination, source);
            case BlendMode::Screen:
                return screenBlend(destination, source);
            case BlendMode::Overlay:
                return overlayBlend(destination, source);
            default:
                return noBlend(destination, source);
        }
    }

    Color Color::noBlend(Color destination, Color source) {
        return source;
    }

    Color Color::alphaBlend(Color destination, Color source) {
        constexpr f32 inverse255 = 1.f / 255.f;

        f32 sourceAlpha = source.a * inverse255;
        f32 inverseSourceAlpha = 1.f - sourceAlpha;

        destination.r = static_cast<u8>(source.r * sourceAlpha + destination.r * inverseSourceAlpha);
        destination.g = static_cast<u8>(source.g * sourceAlpha + destination.g * inverseSourceAlpha);
        destination.b = static_cast<u8>(source.b * sourceAlpha + destination.b * inverseSourceAlpha);
        destination.a = static_cast<u8>(source.a + destination.a * inverseSourceAlpha);

        return destination;
    }

    Color Color::additiveBlend(Color destination, Color source) {
        destination.r = static_cast<u8>(std::min<unsigned int>(static_cast<unsigned int>(destination.r) + static_cast<unsigned int>(source.r), 255u));
        destination.g = static_cast<u8>(std::min<unsigned int>(static_cast<unsigned int>(destination.g) + static_cast<unsigned int>(source.g), 255u));
        destination.b = static_cast<u8>(std::min<unsigned int>(static_cast<unsigned int>(destination.b) + static_cast<unsigned int>(source.b), 255u));
        destination.a = static_cast<u8>(std::min<unsigned int>(static_cast<unsigned int>(destination.a) + static_cast<unsigned int>(source.a), 255u));

        return destination;
    }

    Color Color::multiplicativeBlend(Color destination, Color source) {
        destination.r = static_cast<u8>((destination.r * source.r) / 255);
        destination.g = static_cast<u8>((destination.g * source.g) / 255);
        destination.b = static_cast<u8>((destination.b * source.b) / 255);
        destination.a = static_cast<u8>((destination.a * source.a) / 255);

        return destination;
    }

    Color Color::subtractiveBlend(Color destination, Color source) {
        destination.r = std::max(0, static_cast<int>(destination.r - source.r));
        destination.g = std::max(0, static_cast<int>(destination.g - source.g));
        destination.b = std::max(0, static_cast<int>(destination.b - source.b));
        destination.a = std::max(0, static_cast<int>(destination.a - source.a));

        return destination;
    }

    Color Color::screenBlend(Color destination, Color source) {
        destination.r = static_cast<u8>(255 - ((255 - destination.r) * (255 - source.r)) / 255);
        destination.g = static_cast<u8>(255 - ((255 - destination.g) * (255 - source.g)) / 255);
        destination.b = static_cast<u8>(255 - ((255 - destination.b) * (255 - source.b)) / 255);
        destination.a = static_cast<u8>(255 - ((255 - destination.a) * (255 - source.a)) / 255);

        return destination;
    }

    Color Color::overlayBlend(Color destination, Color source) {
        auto overlayChannel = [](u8 dest, u8 src) -> u8 {
            f32 d = dest / 255.0f;
            f32 s = src / 255.0f;
            
            f32 result;
            if (d < 0.5f) {
                result = 2.0f * d * s;
            } else {
                result = 1.0f - 2.0f * (1.0f - d) * (1.0f - s);
            }
            
            return static_cast<u8>(std::clamp(result * 255.0f, 0.0f, 255.0f));
        };
        
        destination.r = overlayChannel(destination.r, source.r);
        destination.g = overlayChannel(destination.g, source.g);
        destination.b = overlayChannel(destination.b, source.b);
        destination.a = overlayChannel(destination.a, source.a);
        
        return destination;
    }
}

