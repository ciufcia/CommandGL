#include "cgl.hpp"
#include <functional>
#include <vector>

bool cgl::EventManager::isKeyPressed(KeyCode key) const {
    return getKeyStates()[static_cast<size_t>(key)];
}

const std::array<bool, static_cast<size_t>(cgl::KeyCode::Count)> &cgl::EventManager::getKeyStates() const {
    return m_console.getKeyStates();
}

std::optional<cgl::Event> cgl::EventManager::pollEvent() {
    if (m_events.empty()) {
        return std::nullopt;
    }

    Event event = m_events.front();
    m_events.erase(m_events.begin());
    return event;
}

void cgl::EventManager::discardEvents() {
    m_events.clear();
}

void cgl::EventManager::updateEvents() {
    m_console.getEvents(m_events);
}

const std::vector<cgl::Event> &cgl::EventManager::peekEvents() const {
    return m_events;
}