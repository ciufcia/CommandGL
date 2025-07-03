#include "cgl.hpp"

namespace cgl
{
    const Vector2<u32> &ScreenBuffer::getSize() const {
        return m_size;
    }

    const Vector2<f32> &ScreenBuffer::getInverseSize() const {
        return m_inverseSize;
    }

    void ScreenBuffer::setSize(const Vector2<u32> &size) {
        if (size.x == 0u || size.y == 0u)
            throw std::invalid_argument("Size cannot be zero");

        m_size = size;
        m_buffer.resize(size.x * size.y, Color());

        m_inverseSize.x = 1.f / static_cast<f32>(size.x);
        m_inverseSize.y = 1.f / static_cast<f32>(size.y);
    }

    std::vector<Color> &ScreenBuffer::getBuffer() {
        return m_buffer;
    }
}