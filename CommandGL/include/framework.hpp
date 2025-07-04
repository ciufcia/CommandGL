#ifndef CGL_FRAMEWORK_HPP
#define CGL_FRAMEWORK_HPP

#include "global_memory.hpp"
#include "console.hpp"
#include "filters.hpp"
#include "drawable.hpp"
#include "timing.hpp"

namespace cgl
{
    /**
     * @class Framework
     * @brief Main framework class that manages the graphics rendering pipeline and application lifecycle.
     * 
     * The Framework class serves as the central coordinator for the CommandGL graphics library,
     * managing console output, event handling, timing, drawable objects, and the rendering
     * pipeline. It provides a high-level interface for creating console-based graphics applications.
     */
    class Framework
    {
    public:

        /**
         * @brief Initializes the framework and sets up all internal systems.
         * 
         * This method must be called before using any other framework functionality.
         * It initializes buffers, filter pipelines, and prepares the console for rendering.
         */
        void initialize();

        /**
         * @brief Clears the display with the specified color.
         * @param color The color to clear the screen with (defaults to black).
         * 
         * This method fills the entire screen buffer with the specified color,
         * effectively clearing any previously rendered content.
         */
        void clearDisplay(Color color = {0, 0, 0, 255});

        /**
         * @brief Adds a drawable object to the render queue.
         * @param drawable A shared pointer to the drawable object to render.
         * 
         * The drawable will be rendered in the next update() call. Drawables are
         * sorted by their depth value before rendering, with higher depth values
         * rendered earlier.
         */
        void draw(std::shared_ptr<Drawable> drawable);

        /**
         * @brief Updates the framework and renders all queued drawables.
         * 
         * This method should be called once per frame. It processes events,
         * renders all queued drawables, applies post-processing filters,
         * and outputs the final result to the console.
         */
        void update();

    public:

        /**
         * @brief Console used for rendering graphics and handling input.
         * 
         * This console provides the screen buffer where all graphics are drawn,
         * and handles input events from the user. It is the main output target
         * for the framework's rendering operations.
         */
        Console console {};

        /**
         * @brief Event manager for handling keyboard, mouse, and console events.
         * 
         * Provides access to input event handling and polling functionality.
         * Use this to check key states, handle events, and respond to user input.
         */
        EventManager eventManager { console };

        /**
         * @brief High-resolution timer for frame timing and elapsed time measurement.
         * 
         * Use this clock for frame delta calculations, animation timing,
         * and performance measurement.
         */
        Clock clock;

        /**
         * @brief Whether to automatically scale drawables when console size differs from base size.
         * 
         * When true, drawables will be scaled to maintain consistent appearance
         * across different console window sizes relative to the baseConsoleSize.
         */
        bool scaleOnBaseSizeDeviation = false;
        
        /**
         * @brief The reference console size used for automatic scaling calculations.
         * 
         * When scaleOnBaseSizeDeviation is true, this size is used as the reference
         * to calculate scaling factors for maintaining consistent drawable sizes.
         */
        Vector2<u32> baseConsoleSize { 1u, 1u };

    private:

        void populateGlobalMemory() const;
        void initializeBuffers();
        void initializeFilterPipelines();

        void writeDrawableBuffer(BlendMode blendMode);

        void runScreenFilterPipeline();
        void runCharacterFilterPipeline();
        void writeCharacterBuffer();

        void handleResizing(const Vector2<u32> &newSize);

        void scaleToScreen(Transform &transform);
        void render();

    private:

        ScreenBuffer m_screenBuffer {};
        CharacterBuffer m_characterBuffer {};

        FilterPipeline m_screenFilterPipeline {};
        FilterPipeline m_characterFilterPipeline {};

        std::vector<filter_pass_data::PixelPass> m_drawableBuffer {};
        std::vector<std::pair<std::shared_ptr<Drawable>, Transform>> m_drawQueue {};

        Vector2<f32> m_screenScaleFactor { 1.f, 1.f };
    };
}

#endif // CGL_FRAMEWORK_HPP