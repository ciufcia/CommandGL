#include "til.hpp"

namespace til
{
    void Framework::initialize() {
        console.init();

        console.getEvents(eventManager.m_events);
        eventManager.discardEvents();

        console.clear();

        m_clock.tick();

        initialized = true;
    }

    void Framework::display() {
        if (!initialized) {
            invokeError<LogicError>("Framework not initialized");
            return;
        }

        windowManager.renderWindows();
        windowManager.sortByDepth();

        for (const Window& window : windowManager.getWindows()) {
            console.drawWindow(window);
        }

        console.writeBuffer();
    }

    void Framework::update() {
        if (!initialized) {
            invokeError<LogicError>("Framework not initialized");
            return;
        }

        console.getEvents(eventManager.m_events);

        renderer.clearMeshes();

        f32 time = getDurationInSeconds(m_clock.getRunningDuration());
        for (Window &window : windowManager.getWindows()) {
            window.getBaseData().time = time;
        }

        if (m_clock.getTickDuration() < m_targetUpdateDuration) {
            m_clock.wait(m_targetUpdateDuration - m_clock.getTickDuration());
        }

        m_lastUpdateDuration = m_clock.tick();
    }

    void Framework::setTargetUpdateRate(u32 updatesPerSecond) {
        m_targetUpdateDuration = std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<f32>(1.f / updatesPerSecond));
    }

    void Framework::setTargetUpdateDuration(std::chrono::steady_clock::duration duration) {
        m_targetUpdateDuration = duration;
    }

    std::chrono::steady_clock::duration Framework::getLastUpdateDuration() const {
        return m_lastUpdateDuration;
    }
}