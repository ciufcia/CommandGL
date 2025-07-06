#include "cgl.hpp"

#include <iostream>
#include <algorithm>

namespace cgl
{
    void Framework::initialize() {
        populateGlobalMemory();
        initializeBuffers();
        initializeFilterPipelines();

        m_clock.tick();

        baseConsoleSize = console.getSize();
    }

    void Framework::populateGlobalMemory() const {
        GlobalMemory &globalMemory = GlobalMemory::getInstance();

        globalMemory.add("cgl_version", "1.0.0");
    }

    void Framework::initializeBuffers() {
        Vector2<u32> consoleSize = console.getSize();

        m_screenBuffer.setSize(consoleSize);
        m_characterBuffer.setSize(consoleSize);
    }

    void Framework::initializeFilterPipelines() {
        auto filter = std::make_shared<Filter>();
        filter->type = FilterType::SinglePass;
        filter->function = filters::rgbSingleCharacter;
        filter->data = std::make_shared<filters::RGBSingleCharacterData>();

        characterFilterPipeline.addFilter(filter, 0u);
    }

    void Framework::clearDisplay(Color color) {
        #pragma omp parallel for
        for (int i = 0; i < static_cast<int>(m_screenBuffer.getSize().x) * static_cast<int>(m_screenBuffer.getSize().y); ++i) {
            m_screenBuffer.getBuffer()[i] = color;
        }
    }

    void Framework::draw(std::shared_ptr<Drawable> drawable) {
        m_drawQueue.push_back({ drawable, drawable->transform });
    }

    void Framework::update() {
        m_screenScaleFactor = {
            static_cast<f32>(console.getSize().x) / std::max(static_cast<f32>(baseConsoleSize.x), 1e-6f),
            static_cast<f32>(console.getSize().y) / std::max(static_cast<f32>(baseConsoleSize.y), 1e-6f)
        };

        render();

        eventManager.updateEvents();
        
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
                return a.first->depth > b.first->depth;
            }
        );

        f32 time = getDurationInSeconds(m_clock.getRunningDuration());

        for (auto &pair : m_drawQueue) {
            m_drawableBuffer.clear();

            scaleToScreen(pair.second);

            pair.first->generateGeometry(m_drawableBuffer, pair.second);

            pair.first->applyFragmentPipeline(m_drawableBuffer, time);

            writeDrawableBuffer(pair.first->blendMode);
        }

        m_drawQueue.clear();

        runScreenFilterPipeline();
        runCharacterFilterPipeline();
        writeCharacterBuffer();
    }

    void Framework::writeDrawableBuffer(BlendMode blendMode) {
        #pragma omp parallel for
        for (int i = 0; i < static_cast<int>(m_drawableBuffer.size()); ++i) {
            filter_pass_data::PixelPass &pixelData = m_drawableBuffer[i];

            if (pixelData.position.x < 0 || pixelData.position.x >= m_screenBuffer.getSize().x ||
                pixelData.position.y < 0 || pixelData.position.y >= m_screenBuffer.getSize().y) {
                continue;
            }
            
            u32 index = static_cast<u32>(pixelData.position.y) * m_screenBuffer.getSize().x + static_cast<u32>(pixelData.position.x);
            Color &destinationColor = m_screenBuffer.getBuffer()[index];
            
            destinationColor = Color::applyBlend(destinationColor, pixelData.color, blendMode);
        }
    }

    void Framework::runScreenFilterPipeline() {
        f32 time = getDurationInSeconds(m_clock.getRunningDuration());

        screenFilterPipeline.start();

        while (screenFilterPipeline.step()) {
            auto currentFilter = screenFilterPipeline.getCurrentFilter();
            
            if (!currentFilter->isEnabled)
                continue;

            if (currentFilter->type == FilterType::SinglePass) {
                filter_pass_data::ScreenBufferSinglePass passData;
                passData.screenBuffer = &m_screenBuffer;
                passData.time = time;

                currentFilter->function(currentFilter->data.get(), &passData);
            } else if (currentFilter->type == FilterType::Sequential) {
                filter_pass_data::PixelPass passData;
                passData.time = time;

                for (u32 y = 0u; y < m_screenBuffer.getSize().y; ++y) {
                    for (u32 x = 0u; x < m_screenBuffer.getSize().x; ++x) {
                        Color &color = m_screenBuffer.getBuffer()[y * m_screenBuffer.getSize().x + x];
                        passData.color = color;
                        passData.position = { static_cast<f32>(x), static_cast<f32>(y) };
                        passData.uv = { static_cast<f32>(x) * m_screenBuffer.getInverseSize().x, static_cast<f32>(y) * m_screenBuffer.getInverseSize().y };
                        passData.size = { static_cast<f32>(m_screenBuffer.getSize().x), static_cast<f32>(m_screenBuffer.getSize().y) };
                        passData.inverseSize = m_screenBuffer.getInverseSize();

                        currentFilter->function(currentFilter->data.get(), &passData);

                        color = passData.color;
                    }
                }
            } else if (currentFilter->type == FilterType::Parallel) {
                #pragma omp parallel for
                for (int y = 0; y < m_screenBuffer.getSize().y; ++y) {
                    for (int x = 0; x < m_screenBuffer.getSize().x; ++x) {
                        filter_pass_data::PixelPass passData;
                        passData.time = time;

                        Color &color = m_screenBuffer.getBuffer()[y * m_screenBuffer.getSize().x + x];
                        passData.color = color;
                        passData.position = { static_cast<f32>(x), static_cast<f32>(y) };
                        passData.uv = { static_cast<f32>(x) * m_screenBuffer.getInverseSize().x, static_cast<f32>(y) * m_screenBuffer.getInverseSize().y };
                        passData.size = { static_cast<f32>(m_screenBuffer.getSize().x), static_cast<f32>(m_screenBuffer.getSize().y) };
                        passData.inverseSize = m_screenBuffer.getInverseSize();

                        currentFilter->function(currentFilter->data.get(), &passData);

                        color = passData.color;
                    }
                }
            }
        }
    }

    void Framework::runCharacterFilterPipeline() {
        f32 time = getDurationInSeconds(m_clock.getRunningDuration());

        characterFilterPipeline.start();

        while (characterFilterPipeline.step()) {
            auto currentFilter = characterFilterPipeline.getCurrentFilter();
            
            if (!currentFilter->isEnabled)
                continue;

            if (currentFilter->type != FilterType::SinglePass) {
                throw std::runtime_error("Only SinglePass filters are supported in this pipeline.");
            }

            filter_pass_data::CharacterBufferSinglePass passData;
            passData.time = time;
            passData.characterBuffer = &m_characterBuffer;
            passData.screenBuffer = &m_screenBuffer;

            currentFilter->function(currentFilter->data.get(), &passData);
        }
    }

    void Framework::writeCharacterBuffer() {
        console.writeCharacterBuffer(m_characterBuffer);
    }

    void Framework::handleResizing(const Vector2<u32> &newSize) {
        m_characterBuffer.setSize(newSize);
        m_screenBuffer.setSize(newSize);
    }
}