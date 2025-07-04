#include "cgl.hpp"
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace cgl
{
    Texture::Texture(const std::string &filepath) {
        load(filepath);
    }

    std::shared_ptr<Texture> Texture::create(const Vector2<u32> &size) {
        return std::make_shared<Texture>(size);
    }

    std::shared_ptr<Texture> Texture::create(const std::string &filepath) {
        return std::make_shared<Texture>(filepath);
    }

    Vector2<u32> Texture::getSize() const {
        return m_size;
    }

    Vector2<u32> Texture::setSize(const Vector2<u32> &size) {
        m_size = size;
        m_data.resize(size.x * size.y, Color());
        return m_size;
    }

    void Texture::load(const std::string &filepath) {
        int width, height, channels;
        unsigned char *imageData = stbi_load(filepath.c_str(), &width, &height, &channels, 4);

        if (!imageData) {
            throw std::runtime_error("Failed to load texture from file: " + filepath);
        }

        m_size = { static_cast<u32>(width), static_cast<u32>(height) };
        m_data.resize(m_size.x * m_size.y);

        for (u32 y = 0; y < m_size.y; ++y) {
            for (u32 x = 0; x < m_size.x; ++x) {
                int index = (y * m_size.x + x) * 4;

                u8 r = imageData[index];
                u8 g = imageData[index + 1];
                u8 b = imageData[index + 2];
                u8 a = imageData[index + 3];

                m_data[y * m_size.x + x] = Color(r, g, b, a);
            }
        }

        stbi_image_free(imageData);
    }

    void Texture::setPixel(const Vector2<u32> &position, const Color &color) {
        if (position.x < m_size.x && position.y < m_size.y) {
            m_data[position.y * m_size.x + position.x] = color;
        }
    }

    Color Texture::sample(const Vector2<f32> &uv, SamplingMode mode) const {
        switch (mode) {
            case SamplingMode::NearestNeighbor:
                return sampleNearestNeighbor(uv);
            case SamplingMode::Bilinear:
                return sampleBilinear(uv);
            default:
                throw std::invalid_argument("Unsupported sampling mode");
        }
    }

    Color Texture::sampleNearestNeighbor(const Vector2<f32> &uv) const {
        int x = static_cast<int>(uv.x * m_size.x);
        int y = static_cast<int>(uv.y * m_size.y);

        if (x < 0 || x >= static_cast<int>(m_size.x) || y < 0 || y >= static_cast<int>(m_size.y)) {
            return Color();
        }

        return m_data[y * m_size.x + x];
    }

    Color Texture::sampleBilinear(const Vector2<f32> &uv) const {
         f32 x = uv.x * (m_size.x - 1);
        f32 y = uv.y * (m_size.y - 1);

        int x0 = static_cast<int>(x);
        int y0 = static_cast<int>(y);
        int x1 = std::min(x0 + 1, static_cast<int>(m_size.x) - 1);
        int y1 = std::min(y0 + 1, static_cast<int>(m_size.y) - 1);

        f32 xLerp = x - x0;
        f32 yLerp = y - y0;

        Color c00 = m_data[y0 * m_size.x + x0];
        Color c01 = m_data[y0 * m_size.x + x1];
        Color c10 = m_data[y1 * m_size.x + x0];
        Color c11 = m_data[y1 * m_size.x + x1];

        auto lerp = [](u8 a, u8 b, f32 t) {
            return static_cast<u8>(a * (1.0f - t) + b * t);
        };

        Color top{
            lerp(c00.r, c01.r, xLerp),
            lerp(c00.g, c01.g, xLerp),
            lerp(c00.b, c01.b, xLerp),
            lerp(c00.a, c01.a, xLerp)
        };

        Color bottom{
            lerp(c10.r, c11.r, xLerp),
            lerp(c10.g, c11.g, xLerp),
            lerp(c10.b, c11.b, xLerp),
            lerp(c10.a, c11.a, xLerp)
        };

        return {
            lerp(top.r, bottom.r, yLerp),
            lerp(top.g, bottom.g, yLerp),
            lerp(top.b, bottom.b, yLerp),
            lerp(top.a, bottom.a, yLerp)
        };
    }
}