#include "til.hpp"

namespace til
{
    std::optional<Event> EventManager::pollEvent() {
        if (m_events.empty()) {
            return std::nullopt;
        }

        Event event = m_events.front();
        m_events.erase(m_events.begin());
        return event;
    }

    void EventManager::discardEvents() {
        m_events.clear();
    }

    std::vector<Event> &EventManager::getEvents() {
        return m_events;
    }

    const std::vector<Event> &EventManager::peekEvents() const {
        return m_events;
    }
}