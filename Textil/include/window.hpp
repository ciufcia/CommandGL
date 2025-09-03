#ifndef TIL_WINDOW_HPP
#define TIL_WINDOW_HPP

#include "render.hpp"
#include "filter_pipeline.hpp"
#include <list>
#include <string>

namespace til
{
    class Window : public RenderTarget
    {
    public:

        const Vector2<i32> &getPosition() const;
        void setPosition(const Vector2<i32> &position);
        const Vector2<u32> &getSize() const;
        void setSize(const Vector2<u32> &size);

        void runPostProcessingPipeline();
        void runCharacterPipeline();

        CharacterCell getCharacterCell(u32 index) const;

    public:

        Vector2<i32> m_position { 0, 0 };

        f32 depth = 0.f;

        u32 id = 0;

        FilterPipeline<Color, Color> postProcessPipeline {};
        FilterPipeline<Color, CharacterCell> characterPipeline {};

    private:

        FilterableBuffer<CharacterCell> m_characterBuffer {};
    };
}

#endif // TIL_WINDOW_HPP