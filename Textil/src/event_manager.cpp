#include "til.hpp"

namespace til
{
    std::optional<Event> EventManager::pollEvent() {
        if (m_currentEventIndex >= m_events.size()) {
            discardEvents();
            return std::nullopt;
        }

        return m_events[m_currentEventIndex++];
    }

    void EventManager::discardEvents() {
        m_events.clear();
        m_currentEventIndex = 0;
    }

    const std::vector<Event> &EventManager::peekEvents() const {
        return m_events;
    }
}