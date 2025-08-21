#include "vector2.hpp"
#include <cmath>

namespace til
{
    Vector2<f32> normalizeUV(const Vector2<f32> &uv) {
        Vector2<f32> i;
        Vector2<f32> normalizedUV = { std::modf(uv.x, &i.x), std::modf(uv.y, &i.y) };

        if (i.x == 1.f) {
            normalizedUV.x = 0.999f;
        }
        if (i.y == 1.f) {
            normalizedUV.y = 0.999f;
        }

        return normalizedUV;
    }

    Color sampleUVGradient(const Vector2<f32> &uv) {
        Vector2<f32> normalizedUV = normalizeUV(uv);

        return {
            static_cast<u8>(std::lerp(0.f, 255.f, normalizedUV.y)),
            static_cast<u8>(std::lerp(0.f, 255.f, normalizedUV.x)),
            static_cast<u8>(std::lerp(255.f, 0.f, normalizedUV.x))
        };
    }
}