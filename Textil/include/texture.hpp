/**
 * @file texture.hpp  
 * @brief 2D texture class for image storage and sampling in Textil library
 * @details Provides texture functionality for storing, loading, and sampling 2D images.
 *          Supports various sampling modes and file I/O operations for common image formats.
 *          Textures are used extensively in the rendering pipeline for sprites, backgrounds,
 *          and filter operations.
 */

#ifndef TIL_TEXTURE_HPP
#define TIL_TEXTURE_HPP

#include "color.hpp"
#include "vector2.hpp"
#include <vector>
#include <string>

namespace til
{
    /**
     * @brief 2D texture class for image data storage and manipulation
     * @details Represents a 2D grid of Color pixels that can be loaded from files,
     *          created programmatically, and sampled with various filtering modes.
     *          Textures use UV coordinates (0.0-1.0 range) for sampling, allowing
     *          resolution-independent access to image data.
     * 
     *          Memory layout is row-major: pixel(x,y) = data[y * width + x]
     *          UV coordinates map as: U=0→left edge, U=1→right edge, V=0→top edge, V=1→bottom edge
     */
    class Texture
    {
    public:
        /**
         * @brief Enumeration of texture sampling methods
         * @details Defines how pixel values are interpolated when sampling between discrete pixels.
         */
        enum class SamplingMode
        {
            NearestNeighbor, ///< Use closest pixel value (sharp, pixelated look)
            Bilinear        ///< Interpolate between 4 nearest pixels (smooth, filtered look)
        };

    public:
        /**
         * @brief Default constructor creating empty texture
         * @details Creates texture with zero size and no data. Use setSize() or load() to initialize.
         */
        Texture() = default;
        
        /**
         * @brief Virtual destructor for proper cleanup
         * @details Ensures derived classes are properly destroyed and memory is released.
         */
        virtual ~Texture() = default;

        /**
         * @brief Constructor creating texture with specified size
         * @param size Dimensions of texture in pixels (width, height)
         * @details Creates texture filled with default Color() values (magenta).
         *          All pixels are initialized to the same default color.
         */
        Texture(const Vector2<u32> &size) : m_size(size), m_data(size.x * size.y, Color()) {}

        /**
         * @brief Constructor loading texture from image file
         * @param filepath Path to image file to load
         * @throws InvalidArgumentError if file cannot be loaded or format unsupported
         * @details Supports common image formats via stb_image library.
         *          Automatically handles format conversion and memory allocation.
         */
        Texture(const std::string &filepath);

        /**
         * @brief Create empty shared_ptr texture
         * @return Smart pointer to new empty texture
         * @details Factory method for creating heap-allocated textures with automatic memory management.
         */
        static std::shared_ptr<Texture> create();

        /**
         * @brief Create shared_ptr texture with specified size
         * @param size Dimensions of texture in pixels
         * @return Smart pointer to new texture with specified dimensions
         * @details Factory method creating initialized texture on the heap.
         */
        static std::shared_ptr<Texture> create(const Vector2<u32> &size);

        /**
         * @brief Create shared_ptr texture from image file
         * @param filepath Path to image file to load
         * @return Smart pointer to new texture loaded from file
         * @throws InvalidArgumentError if file cannot be loaded
         * @details Factory method for loading textures from files with automatic memory management.
         */
        static std::shared_ptr<Texture> create(const std::string &filepath);

        /**
         * @brief Get current texture dimensions
         * @return Size vector containing width and height in pixels
         */
        Vector2<u32> getSize() const;

        /**
         * @brief Resize texture and clear contents
         * @param size New dimensions in pixels  
         * @return Updated size after resize
         * @details Reallocates internal data array and fills with default Color values.
         *          Previous texture content is lost during resize operation.
         */
        Vector2<u32> setSize(const Vector2<u32> &size);

        /**
         * @brief Set texture data from external color array (copy)
         * @param size Dimensions of the source data
         * @param data Vector of Color values in row-major order
         * @details Copies provided color data into texture. Data array must contain
         *          exactly size.x * size.y elements. Existing texture content is replaced.
         */
        void setRawData(const Vector2<u32> &size, const std::vector<Color> &data);

        /**
         * @brief Set texture data from external color array (move)
         * @param size Dimensions of the source data  
         * @param data Vector of Color values to move from
         * @details Moves provided color data into texture for optimal performance.
         *          Source vector is left in valid but unspecified state after move.
         */
        void setRawData(const Vector2<u32> &size, std::vector<Color> &&data);

        /**
         * @brief Load texture from image file
         * @param filepath Path to image file
         * @throws InvalidArgumentError if file cannot be loaded or format unsupported
         * @details Replaces current texture content with data from image file.
         *          Supports PNG, JPG, BMP, TGA and other formats via stb_image.
         */
        void load(const std::string &filepath);

        /**
         * @brief Save texture to image file
         * @param filepath Output file path (extension determines format)
         * @throws InvalidArgumentError if file cannot be written or format unsupported  
         * @details Writes current texture data to file. Format determined by file extension.
         *          Supports PNG, BMP, TGA output formats via stb_image_write.
         */
        void save(const std::string &filepath) const;

    /**
     * @brief Set color of specific pixel
     * @param position Pixel coordinates (x, y) in texture space
     * @param color New color value for the pixel
     * @details Direct pixel access for texture modification. Positions outside the
     *          texture bounds are ignored.
     */
        void setPixel(const Vector2<u32> &position, const Color &color);

        /**
         * @brief Sample texture color at UV coordinates with specified filtering
         * @param uv Normalized coordinates (0.0-1.0 range)
         * @param mode Sampling method to use for interpolation
         * @return Interpolated color at the specified UV position
         * @details Primary texture sampling method supporting multiple filtering modes.
         *          Coordinates outside [0,1] return the default Color() value (no wrapping or clamping).
         */
        Color sample(const Vector2<f32> &uv, SamplingMode mode) const;

        /**
         * @brief Sample texture using nearest neighbor filtering
         * @param uv Normalized coordinates (0.0-1.0 range)
         * @return Color of the nearest pixel to UV position
         * @details Fast sampling method that selects the closest pixel without interpolation.
         *          Produces sharp, pixelated appearance when scaling textures. Returns the
         *          default color when coordinates fall outside the texture.
         */
        Color sampleNearestNeighbor(const Vector2<f32> &uv) const;

        /**
         * @brief Sample texture using bilinear filtering  
         * @param uv Normalized coordinates (0.0-1.0 range)
         * @return Bilinearly interpolated color at UV position
         * @details High-quality sampling method that interpolates between the 4 nearest pixels.
         *          Produces smooth, filtered appearance when scaling textures.
         *          More computationally expensive than nearest neighbor. Coordinates outside
         *          the texture bounds return the default color.
         */
        Color sampleBilinear(const Vector2<f32> &uv) const;

    private:
        Vector2<u32> m_size;     ///< Texture dimensions in pixels (width, height)
        std::vector<Color> m_data; ///< Pixel data in row-major order
    };
}

#endif // TIL_TEXTURE_HPP