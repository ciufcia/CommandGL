#include "cgl.hpp"

#include <iostream>
#include <algorithm>

namespace cgl
{
    void Framework::initialize() {
        console.init();
        baseConsoleSize = console.getSize();

        populateGlobalMemory();
        initializeBuffers();
        initializeFilterPipelines();

        console.getEvents(eventManager.getEvents());
        eventManager.discardEvents();

        m_resizedPreviousFrame = false;
        
        m_clock.tick();
    }

    void Framework::populateGlobalMemory() const {
        GlobalMemory &globalMemory = GlobalMemory::getInstance();

        globalMemory.add("cgl_version", "1.0.0");
    }

    void Framework::initializeBuffers() {
        handleResizing(console.getSize());
    }

    void Framework::initializeFilterPipelines() {
        screenFilterPipeline.build();
		characterFilterPipeline.build();
    }

    void Framework::clearDisplay(Color color) {
        #pragma omp parallel for
        for (int i = 0; i < static_cast<int>(m_screenBuffer.getSize()); ++i) {
            m_screenBuffer[i] = color;
        }
    }

    void Framework::draw(std::shared_ptr<Drawable> drawable) {
        if (drawable->cloneOnDraw) {
            drawable = drawable->clone();

            if (!drawable) {
                invokeError<InvalidArgumentError>("Failed to clone drawable object.");
            }
        }

        m_drawQueue.push_back({ drawable, drawable->transform });
    }

    void Framework::update() {
        render();

        eventManager.discardEvents();

        console.getEvents(eventManager.getEvents());

        for (const Event &event : eventManager.peekEvents()) {
            if (event.isOfType<ConsoleEvent>()) {
                handleResizing(event.newSize);
            }
        }

        auto frameDuration = m_clock.tick();

        if (frameDuration < m_targetFrameTime) {
            m_clock.wait(m_targetFrameTime - frameDuration);
        }

        m_lastFrameTime = frameDuration + m_clock.getTickDuration();
    }

    u32 Framework::getFPSTarget() const {
        return static_cast<u32>(std::chrono::duration_cast<std::chrono::milliseconds>(m_targetFrameTime).count());
    }

    void Framework::setFPSTarget(u32 target) {
        m_targetFrameTime = std::chrono::milliseconds(1000 / target);
    }

    std::chrono::steady_clock::duration Framework::getTargetFrameTime() const {
        return m_targetFrameTime;
    }

    void Framework::setTargetFrameTime(std::chrono::steady_clock::duration targetFrameTime) {
        m_targetFrameTime = targetFrameTime;
    }

    std::chrono::steady_clock::duration Framework::getLastFrameTime() const {
        return m_lastFrameTime;
    }

    void Framework::scaleToScreen(Transform &transform) {
        if (!scaleOnBaseSizeDeviation) return;

        transform.m_postScale = m_screenScaleFactor;
    }

    void Framework::render() {
        std::sort(
            m_drawQueue.begin(),
            m_drawQueue.end(),
            [](const auto& a, auto& b) {
                return a.drawable->depth > b.drawable->depth;
            }
        );

        filters::BaseData baseData;
        baseData.time = getDurationInSeconds(m_clock.getRunningDuration());
        baseData.buffer_resized = m_resizedPreviousFrame;
        m_resizedPreviousFrame = false;

        for (auto &drawEntry : m_drawQueue) {
            m_drawableBuffer.clear();

            scaleToScreen(drawEntry.transform);

            drawEntry.drawable->generateGeometry(m_drawableBuffer, drawEntry.transform);

            applyDrawableFragmentOnDrawableBuffer(drawEntry.drawable->fragmentPipeline, m_drawableBuffer, baseData);

            writeDrawableBuffer(drawEntry.drawable->blendMode);
        }

        m_drawQueue.clear();

        screenFilterPipeline.run(&m_screenBuffer, &m_screenBuffer, baseData);
        characterFilterPipeline.run(&m_screenBuffer, &m_characterBuffer, baseData);

        console.writeCharacterBuffer(m_characterBuffer, m_screenSize);
    }

    void Framework::applyDrawableFragmentOnDrawableBuffer(FilterPipeline<filters::GeometryElementData, filters::GeometryElementData> &pipeline, std::vector<filters::GeometryElementData> &drawableBuffer, const filters::BaseData &baseData) {
        m_filterableBuffer.setData(drawableBuffer.data(), static_cast<u32>(drawableBuffer.size()));

        pipeline.run(&m_filterableBuffer, &m_filterableBuffer, baseData);
    }

    void Framework::writeDrawableBuffer(BlendMode blendMode) {
        #pragma omp parallel for
        for (int i = 0; i < static_cast<int>(m_drawableBuffer.size()); ++i) {
            filters::GeometryElementData &pixelData = m_drawableBuffer[i];

            if (pixelData.position.x < 0 || pixelData.position.x >= m_screenSize.x ||
                pixelData.position.y < 0 || pixelData.position.y >= m_screenSize.y) {
                continue;
            }
            
            u32 index = static_cast<u32>(pixelData.position.y) * m_screenSize.x + static_cast<u32>(pixelData.position.x);
            Color &destinationColor = m_screenBuffer[index];
            
            destinationColor = Color::applyBlend(destinationColor, pixelData.color, blendMode);
        }
    }

    void Framework::handleResizing(const Vector2<u32> &newSize) {
        m_screenSize = newSize;

        m_characterBuffer.setSize(m_screenSize.x * m_screenSize.y);
        m_screenBuffer.setSize(m_screenSize.x * m_screenSize.y);

        m_screenScaleFactor = {
            static_cast<f32>(console.getSize().x) / std::max(static_cast<f32>(baseConsoleSize.x), 1e-6f),
            static_cast<f32>(console.getSize().y) / std::max(static_cast<f32>(baseConsoleSize.y), 1e-6f)
        };

        m_resizedPreviousFrame = true;
    }
}