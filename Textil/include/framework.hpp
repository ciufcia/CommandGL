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

    public:

        Console console;
        Renderer renderer;
        WindowManager windowManager;
        EventManager eventManager;

    private:

        bool initialized = false;
    };
}

#endif // TIL_FRAMEWORK_HPP