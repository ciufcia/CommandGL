#ifndef CGL_SCREEN_BUFFER_HPP
#define CGL_SCREEN_BUFFER_HPP

#include "vector2.hpp"
#include "numeric_types.hpp"
#include <vector>
#include "color.hpp"

namespace cgl
{
    /**
     * @class ScreenBuffer
     * @brief Represents a screen buffer for rendering operations.
     */
    class ScreenBuffer
    {
    public:

        ScreenBuffer() = default;

        /**
         * @brief Gets the size of the screen buffer.
         * @return The size of the screen buffer as a Vector2<u32> (width, height).
         */
        const Vector2<u32> &getSize() const;

        /**
         * @brief Gets the inverse size of the screen buffer.
         * @return The inverse size of the screen buffer as a Vector2<f32> (1/width, 1/height).
         */
        const Vector2<f32> &getInverseSize() const;

        /**
         * @brief Sets the size of the screen buffer.
         * @param size The new size of the screen buffer.
         * @throws std::invalid_argument if either width or height is zero.
         */
        void setSize(const Vector2<u32> &size);

        /**
         * @brief Gets the color buffer.
         * @return A reference to the color buffer vector.
         */
        std::vector<Color> &getBuffer();

    private:

        Vector2<u32> m_size { 0u, 0u };
        Vector2<f32> m_inverseSize { 0.f, 0.f };

        std::vector<Color> m_buffer {};
    };
}

#endif // CGL_SCREEN_BUFFER_HPP