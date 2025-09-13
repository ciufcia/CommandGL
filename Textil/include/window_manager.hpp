#ifndef TIL_WINDOW_MANAGER_HPP
#define TIL_WINDOW_MANAGER_HPP

#include "numeric_types.hpp"
#include "window.hpp"
#include <vector>

namespace til
{
    class WindowManager
    {
    public:

        WindowManager() = default;

        WindowManager(const WindowManager&) = delete;
        WindowManager& operator=(const WindowManager&) = delete;

        Window &createWindow();
        void destroyWindow(u32 id);
        Window &getWindow(u32 id);

        const std::list<Window>& getWindows() const;

    private:

        void renderWindows();

        void sortByDepth();

        u32 getNextId();

        std::list<Window>& getWindows();

    private:

        std::list<Window> m_windows {};
        u32 m_nextId = 1;
        std::set<u32> m_usedIds;

        std::vector<CharacterCell> m_characterBuffer {};

    friend class Framework;
    };
}

#endif // TIL_WINDOW_MANAGER_HPP