#ifndef CGL_TEXTURE_HPP
#define CGL_TEXTURE_HPP

#include "color.hpp"
#include "vector2.hpp"
#include <vector>
#include <string>

namespace cgl
{
    /**
     * @class Texture
     * @brief Represents a 2D image or pixel buffer that can be loaded from a file or created with a specific size.
     *
     * The Texture class provides functionality for loading image data from files, managing pixel data,
     * resizing, and setting individual pixel colors. It is used for representing images and textures
     * in the graphics framework.
     */
    class Texture
    {
    public:

        /**
         * @enum SamplingMode
         * @brief Enumeration of texture sampling modes.
         *
         * Defines how textures are sampled when rendered, affecting quality and performance.
         */
        enum class SamplingMode
        {
            NearestNeighbor,
            Bilinear
        };

    public:

        Texture() = default;
        virtual ~Texture() = default;

        /**
         * @brief Constructs a texture with the given size, filled with default color.
         * @param size The size (width, height) of the texture in pixels.
         */
        Texture(const Vector2<u32> &size) : m_size(size), m_data(size.x * size.y, Color()) {}

        /**
         * @brief Constructs a texture by loading from a file.
         * @param filepath The path to the image file to load.
         * @throws std::runtime_error if loading fails.
         */
        Texture(const std::string &filepath);

        /**
         * @brief Factory method to create a texture with a specific size.
         * @param size The size (width, height) of the texture in pixels.
         * @return A shared pointer to the created Texture instance.
         */
        static std::shared_ptr<Texture> create(const Vector2<u32> &size);

        /**
         * @brief Factory method to create a texture by loading from a file.
         * @param filepath The path to the image file to load.
         * @return A shared pointer to the created Texture instance.
         * @throws std::runtime_error if loading fails.
         */
        static std::shared_ptr<Texture> create(const std::string &filepath);

        /**
         * @brief Gets the current size of the texture.
         * @return The size (width, height) of the texture in pixels.
         */
        Vector2<u32> getSize() const;

        /**
         * @brief Sets the size of the texture and resizes the pixel data.
         * @param size The new size (width, height) of the texture in pixels.
         * @return The new size of the texture.
         */
        Vector2<u32> setSize(const Vector2<u32> &size);

        /**
         * @brief Loads texture data from an image file.
         * @param filepath The path to the image file to load.
         * @throws std::runtime_error if loading fails.
         */
        void load(const std::string &filepath);

        /**
         * @brief Sets the color of a specific pixel in the texture.
         * @param position The (x, y) position of the pixel.
         * @param color The color to set at the specified position.
         */
        void setPixel(const Vector2<u32> &position, const Color &color);

        /**
         * @brief Samples the texture at given UV coordinates using the specified sampling mode.
         * @param uv The UV coordinates to sample (0.0 to 1.0 range).
         * @param mode The sampling mode to use.
         * @return The sampled color at the specified UV coordinates.
         */
        Color sample(const Vector2<f32> &uv, SamplingMode mode) const;

        /**
         * @brief Samples the texture at given UV coordinates using nearest neighbor sampling.
         * @param uv The UV coordinates to sample (0.0 to 1.0 range).
         * @return The sampled color at the specified UV coordinates.
         */
        Color sampleNearestNeighbor(const Vector2<f32> &uv) const;

        /**
         * @brief Samples the texture at given UV coordinates using bilinear sampling.
         * @param uv The UV coordinates to sample (0.0 to 1.0 range).
         * @return The sampled color at the specified UV coordinates.
         */
        Color sampleBilinear(const Vector2<f32> &uv) const;

    private:

        Vector2<u32> m_size;

        std::vector<Color> m_data;
    };
}

#endif // CGL_TEXTURE_HPP