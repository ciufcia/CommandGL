#include "til.hpp"

namespace til
{
    void Framework::initialize() {
        console.init();

        console.getEvents(eventManager.m_events);
        eventManager.discardEvents();

        console.clear();
    }

    void Framework::display() {
        windowManager.renderWindows();
        windowManager.sortByDepth();

        for (const Window& window : windowManager.getWindows()) {
            console.drawWindow(window);
        }

        console.writeBuffer();
    }

    void Framework::update() {
        console.getEvents(eventManager.m_events);
    }
}