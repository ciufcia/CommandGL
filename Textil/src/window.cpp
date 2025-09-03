#include "til.hpp"

namespace til
{
    const Vector2<i32> &Window::getPosition() const {
        return m_position;
    }

    void Window::setPosition(const Vector2<i32> &position) {
        m_position = position;
    }

    const Vector2<u32> &Window::getSize() const {
        return getBufferSize();
    }

    void Window::setSize(const Vector2<u32> &size) {
        setBufferSize(size);
        m_characterBuffer.getBuffer().resize(size.x * size.y);
    }

    void Window::runPostProcessingPipeline() {
        postProcessPipeline.run(&m_pixelBuffer, &m_pixelBuffer, getBaseData());
    }

    void Window::runCharacterPipeline() {
        characterPipeline.run(&m_pixelBuffer, &m_characterBuffer, getBaseData());
    }

    CharacterCell Window::getCharacterCell(u32 index) const {
        return m_characterBuffer[index];
    }
}