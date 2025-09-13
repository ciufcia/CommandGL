#include "til.hpp"

namespace til
{
    Window &WindowManager::createWindow() {
        m_windows.emplace_back();
        Window &window = m_windows.back();
        window.id = getNextId();
        m_usedIds.insert(window.id);
        return window;
    }

    void WindowManager::destroyWindow(u32 id) {
        auto it = std::find_if(m_windows.begin(), m_windows.end(), [id](const Window &window) {
            return window.id == id;
        });
        if (it == m_windows.end()) {
            invokeError<InvalidArgumentError>("Invalid window ID");
        }
        m_usedIds.erase(id);
        m_windows.erase(it);
    }

    Window &WindowManager::getWindow(u32 id) {
        auto it = std::find_if(m_windows.begin(), m_windows.end(), [id](const Window &window) {
            return window.id == id;
        });
        if (it == m_windows.end()) {
            invokeError<InvalidArgumentError>("Invalid window ID");
        }
        return *it;
    }

    const std::list<Window>& WindowManager::getWindows() const {
        return m_windows;
    }

    std::list<Window>& WindowManager::getWindows() {
        return m_windows;
    }

    void WindowManager::renderWindows() {
        for (auto &window : m_windows) {
            window.render();
            window.runPostProcessingPipeline();
            window.runCharacterPipeline();
        }
    }

    void WindowManager::sortByDepth() {
        m_windows.sort([](const Window &a, const Window &b) {
            return a.depth > b.depth;
        });
    }

    u32 WindowManager::getNextId() {
        while (m_usedIds.count(m_nextId)) {
            ++m_nextId;
        }
        return m_nextId++;
    }
}