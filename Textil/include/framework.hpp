#ifndef TIL_FRAMEWORK_HPP
#define TIL_FRAMEWORK_HPP

#include "global_memory.hpp"
#include "console.hpp"
#include "filters.hpp"
#include "filter_pipeline.hpp"
#include "timing.hpp"
#include "character_cell.hpp"
#include "window.hpp"
#include "window_manager.hpp"
#include "event_manager.hpp"

namespace til
{
    class Framework
    {
    public:

        Framework() = default;
        ~Framework() = default;

        void initialize();
        void display();
        void update();

        void setTargetUpdateRate(u32 updatesPerSecond);
        void setTargetUpdateDuration(std::chrono::steady_clock::duration duration);
        std::chrono::steady_clock::duration getLastUpdateDuration() const;

    public:

        Console console;
        Renderer renderer;
        WindowManager windowManager;
        EventManager eventManager;

    private:

        Clock m_clock;

        bool initialized = false;

        std::chrono::steady_clock::duration m_targetUpdateDuration = std::chrono::steady_clock::duration::zero();
        std::chrono::steady_clock::duration m_lastUpdateDuration = std::chrono::steady_clock::duration::zero();
    };
}

#endif // TIL_FRAMEWORK_HPP