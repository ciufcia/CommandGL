#include "til.hpp"
#include <stdexcept>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace til
{
    Texture::Texture(const std::string &filepath) {
        load(filepath);
    }

    std::shared_ptr<Texture> Texture::create() {
        return std::make_shared<Texture>();
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

    void Texture::setRawData(const Vector2<u32> &size, const std::vector<Color> &data) {
        if (size.x * size.y != data.size()) {
            invokeError<InvalidArgumentError>("Size does not match data length");
        }

        m_size = size;
        m_data = data;
    }

    void Texture::setRawData(const Vector2<u32> &size, const std::vector<Color> &&data) {
        if (size.x * size.y != data.size()) {
            invokeError<InvalidArgumentError>("Size does not match data length");
        }

        m_size = size;
        m_data = std::move(data);
    }

    void Texture::load(const std::string &filepath) {
        int width, height, channels;
        unsigned char *imageData = stbi_load(filepath.c_str(), &width, &height, &channels, 4);

        if (!imageData) {
            invokeError<InvalidArgumentError>("Failed to load texture from file: " + filepath);
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

    void Texture::save(const std::string &filepath) const {
        if (m_data.empty()) {
            invokeError<InvalidArgumentError>("Texture data is empty, cannot save");
        }

        int width = static_cast<int>(m_size.x);
        int height = static_cast<int>(m_size.y);
        std::vector<u8> imageData(width * height * 4);

        for (u32 y = 0; y < m_size.y; ++y) {
            for (u32 x = 0; x < m_size.x; ++x) {
                int index = (y * m_size.x + x) * 4;
                const Color &color = m_data[y * m_size.x + x];

                imageData[index] = color.r;
                imageData[index + 1] = color.g;
                imageData[index + 2] = color.b;
                imageData[index + 3] = color.a;
            }
        }

        if (!stbi_write_png(filepath.c_str(), width, height, 4, imageData.data(), width * 4)) {
            invokeError<LogicError>("Failed to save texture to file: " + filepath);
        }
    }

    void Texture::setPixel(const Vector2<u32> &position, const Color &color) {
        if (position.x < m_size.x && position.y < m_size.y) {
            m_data[position.y * m_size.x + position.x] = color;
        }
    }

    Color Texture::sample(const Vector2<f32> &uv, SamplingMode mode) const {
        Vector2<u32> normalizedUV = normalizeUV(uv);

        switch (mode) {
            case SamplingMode::NearestNeighbor:
                return sampleNearestNeighbor(uv);
            case SamplingMode::Bilinear:
                return sampleBilinear(uv);
            default:
                invokeError<InvalidArgumentError>("Unsupported sampling mode");
                return Color(); // This line will never be reached, but added to avoid compiler warning
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
        f32 x = uv.x * m_size.x;
        f32 y = uv.y * m_size.y;

        int x0 = std::min(static_cast<int>(x), (int)m_size.x - 1);
        int y0 = std::min(static_cast<int>(y), (int)m_size.y - 1);
        int x1 = x0 + 1;
        int y1 = y0 + 1;

        f32 xLerp = x - x0;
        f32 yLerp = y - y0;

        Color c00 = m_data[y0 * m_size.x + x0];
        Color c01;
        (x1 < m_size.x) ? c01 = m_data[y0 * m_size.x + x1] : c01 = Color(0, 0, 0, 0);

        Color c10; Color c11;
        if (y1 < m_size.y) {
            c10 = m_data[y1 * m_size.x + x0];
            (x1 < m_size.x) ? c11 = m_data[y1 * m_size.x + x1] : c11 = Color(0, 0, 0, 0);
        }
        else {
            c10 = Color(0, 0, 0, 0);
            c11 = Color(0, 0, 0, 0);
        }

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