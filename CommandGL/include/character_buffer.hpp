#ifndef CGL_CHARACTER_BUFFER_HPP
#define CGL_CHARACTER_BUFFER_HPP

#include "vector2.hpp"
#include "numeric_types.hpp"
#include <string>

namespace cgl
{
    /**
     * @class CharacterBuffer
     * @brief A 2D buffer for storing wide characters with specified dimensions.
     * 
     * CharacterBuffer provides a container for wide characters arranged in a 2D grid,
     * commonly used for console output operations. The buffer maintains both size
     * information and the character data as a contiguous wide string.
     */
    class CharacterBuffer
    {
    public:

        CharacterBuffer() = default;

        /**
         * @brief Gets the size of the character buffer.
         * @return The size of the character buffer as a Vector2<u32> (width, height).
         */
        const Vector2<u32> &getSize() const;

        /**
         * @brief Sets the size of the character buffer.
         * @param size The new size of the character buffer.
         * @throws std::invalid_argument if either width or height is zero.
         */
        void setSize(const Vector2<u32> &size);

        /**
         * @brief Gets the character buffer.
         * @return A reference to the string containing the characters.
         */
        std::string &getCharacters();

        /**
         * @brief Gets the character buffer.
         * @return A const reference to the string containing the characters.
         */
        const std::string &getCharacters() const;

    private:

        Vector2<u32> m_size { 0u, 0u };

        std::string m_characters;
    };
}

#endif // CGL_CHARACTER_BUFFER_HPP