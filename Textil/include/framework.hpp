#ifndef TIL_FRAMEWORK_HPP
#define TIL_FRAMEWORK_HPP

#include "global_memory.hpp"
#include "console.hpp"
#include "filters.hpp"
#include "drawable.hpp"
#include "timing.hpp"
#include "character_cell.hpp"

namespace til
{
    class Framework
    {
    public:
        Framework() = default;
        void initialize();
        void clearDisplay(Color color = {0, 0, 0, 255});
        void draw(std::shared_ptr<Drawable> drawable);
        void update();
        u32 getFPSTarget() const;

        void setFPSTarget(u32 target);
        std::chrono::steady_clock::duration getTargetFrameTime() const;

        void setTargetFrameTime(std::chrono::steady_clock::duration targetFrameTime);
        std::chrono::steady_clock::duration getLastFrameTime() const;

    public:
        Console console;
        EventManager eventManager;
        bool scaleOnBaseSizeDeviation = false;

        Vector2<u32> baseConsoleSize { 1u, 1u };
        FilterPipeline<Color, Color> screenFilterPipeline;
        FilterPipeline<Color, CharacterCell> characterFilterPipeline;

    private:
        struct DrawEntry
        {
            std::shared_ptr<Drawable> drawable;
            Transform transform;
        };

    private:

        void populateGlobalMemory() const;
        void initializeBuffers();
        void initializeFilterPipelines();

        void applyDrawableFragmentOnDrawableBuffer(FilterPipeline<filters::GeometryElementData, filters::GeometryElementData> &pipeline, std::vector<filters::GeometryElementData> &drawableBuffer, const filters::BaseData &baseData);
        void writeDrawableBuffer(BlendMode blendMode);

        void handleResizing(const Vector2<u32> &newSize);

        void scaleToScreen(Transform &transform);
        void render();

    private:

        FilterableBuffer<Color> m_screenBuffer {};
        FilterableBuffer<CharacterCell> m_characterBuffer {};

        std::vector<filters::GeometryElementData> m_drawableBuffer {};
        FilterableBuffer<filters::GeometryElementData> m_filterableBuffer {};
        std::vector<DrawEntry> m_drawQueue {};

        Vector2<u32> m_screenSize { 0u, 0u };
        Vector2<f32> m_screenScaleFactor { 1.f, 1.f };
        bool m_resizedPreviousFrame = false;

        Clock m_clock;

        std::chrono::steady_clock::duration m_targetFrameTime { std::chrono::milliseconds(16) };

        std::chrono::steady_clock::duration m_lastFrameTime { std::chrono::steady_clock::duration::zero() };
    };
}

#endif // TIL_FRAMEWORK_HPP