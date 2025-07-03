#include "cgl.hpp"

namespace cgl
{
    const Vector2<u32> &CharacterBuffer::getSize() const {
        return m_size;
    }

    void CharacterBuffer::setSize(const Vector2<u32> &size) {
        if (size.x == 0u || size.y == 0u)
            throw std::invalid_argument("Size cannot be zero");

        m_size = size;
    }

    std::wstring &CharacterBuffer::getCharacters() {
        return m_characters;
    }

    const std::wstring &CharacterBuffer::getCharacters() const {
        return m_characters;
    }
}