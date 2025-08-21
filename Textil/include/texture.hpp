#ifndef TIL_TEXTURE_HPP
#define TIL_TEXTURE_HPP

#include "color.hpp"
#include "vector2.hpp"
#include <vector>
#include <string>

namespace til
{
    class Texture
    {
    public:
        enum class SamplingMode
        {
            NearestNeighbor,
            Bilinear
        };

    public:

        Texture() = default;
        virtual ~Texture() = default;

        Texture(const Vector2<u32> &size) : m_size(size), m_data(size.x * size.y, Color()) {}

        Texture(const std::string &filepath);

        static std::shared_ptr<Texture> create();

        static std::shared_ptr<Texture> create(const Vector2<u32> &size);

        static std::shared_ptr<Texture> create(const std::string &filepath);

        Vector2<u32> getSize() const;

        Vector2<u32> setSize(const Vector2<u32> &size);

        void setRawData(const Vector2<u32> &size, const std::vector<Color> &data);

        void setRawData(const Vector2<u32> &size, const std::vector<Color> &&data);

        void load(const std::string &filepath);

        void save(const std::string &filepath) const;

        void setPixel(const Vector2<u32> &position, const Color &color);

        Color sample(const Vector2<f32> &uv, SamplingMode mode) const;

        Color sampleNearestNeighbor(const Vector2<f32> &uv) const;

        Color sampleBilinear(const Vector2<f32> &uv) const;

    private:

        Vector2<u32> m_size;

        std::vector<Color> m_data;
    };
}

#endif // TIL_TEXTURE_HPP