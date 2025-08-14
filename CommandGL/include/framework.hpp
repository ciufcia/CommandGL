#ifndef CGL_FRAMEWORK_HPP
#define CGL_FRAMEWORK_HPP

#include "global_memory.hpp"
#include "console.hpp"
#include "filters.hpp"
#include "drawable.hpp"
#include "timing.hpp"
#include "character_cell.hpp"

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
         * @brief Default constructor for the Framework class.
         */
        Framework() = default;

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

        /**
         * @brief Gets the target FPS for the framework.
         * @return The target FPS as an unsigned 32-bit integer.
         */
        u32 getFPSTarget() const;
        
        /**
         * @brief Sets the target FPS for the framework.
         * @param target The target FPS as an unsigned 32-bit integer.
         * 
         * This sets the desired frames per second for the rendering loop. The framework
         * will attempt to maintain this frame rate by adjusting the timing of each frame.
         */
        void setFPSTarget(u32 target);

        /**
         * @brief Gets the target frame time for the framework.
         * @return The target frame time as a duration in steady clock ticks.
         * 
         * This represents the duration that each frame should ideally take to maintain
         * the desired frame rate.
         */
        std::chrono::steady_clock::duration getTargetFrameTime() const;
        
        /**
         * @brief Sets the target frame time for the framework.
         * @param targetFrameTime The target frame time as a duration in steady clock ticks.
         * 
         * This allows you to specify a custom frame time, which can be useful for
         * fine-tuning performance or implementing variable frame rates.
         */
        void setTargetFrameTime(std::chrono::steady_clock::duration targetFrameTime);

        /**
         * @brief Gets the duration of the last frame.
         * @return The duration of the last frame as a steady clock duration.
         * 
         * This can be used for performance monitoring or debugging purposes,
         * allowing you to see how long the last frame took to render.
         */
        std::chrono::steady_clock::duration getLastFrameTime() const;

    public:

        /**
         * @brief Console used for rendering graphics and handling input.
         * 
         * This console provides the screen buffer where all graphics are drawn,
         * and handles input events from the user. It is the main output target
         * for the framework's rendering operations.
         */
        Console console;

        /**
         * @brief Event manager for handling keyboard, mouse, and console events.
         * 
         * Provides access to input event handling and polling functionality.
         * Use this to check key states, handle events, and respond to user input.
         */
        EventManager eventManager;

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

        /**
         * @brief The screen filter pipeline for post-processing effects applied to the entire screen.
         * 
         * This pipeline is used to apply global effects after all drawables have been rendered.
         */
        FilterPipeline<Color, Color> screenFilterPipeline;

        /**
         * @brief The pipeline that handles converting pixel data to character data
         * 
         * This pipeline processes the rendered pixel data and converts it into
         * character data for display in the console. It can apply effects like
         * dithering, color quantization, or other transformations to optimize the
         * character representation of the rendered scene.
         */
        FilterPipeline<Color, CharacterCell> characterFilterPipeline;

    private:

        /**
         * @brief A structure representing a single drawable entry in the render queue.
         * 
         * This structure holds a shared pointer to the drawable object and its associated
         * transform, which defines how the drawable should be rendered on the screen.
         */
        struct DrawEntry
        {
            std::shared_ptr<Drawable> drawable; ///< The drawable object to render.
            Transform transform;                ///< The transform applied to the drawable.
        };

    private:

        void populateGlobalMemory() const;
        void initializeBuffers();
        void initializeFilterPipelines();

        void applyDrawableFragmentOnDrawableBuffer(FilterPipeline<filters::GeometryElementData, filters::GeometryElementData> &pipeline, std::vector<filters::GeometryElementData> &drawableBuffer, f32 time);
        void writeDrawableBuffer(BlendMode blendMode);

        void handleResizing(const Vector2<u32> &newSize);

        void scaleToScreen(Transform &transform);
        void render();

    private:

        FilterableBuffer<Color> m_screenBuffer {};
        FilterableBuffer<CharacterCell> m_characterBuffer {};

        std::vector<filters::GeometryElementData> m_drawableBuffer {};
        FilterableBuffer<filters::GeometryElementData> m_filterableBuffer {};
        std::vector<DrawEntry> m_drawQueue {};

        Vector2<u32> m_screenSize { 0u, 0u };
        Vector2<f32> m_screenScaleFactor { 1.f, 1.f };

        Clock m_clock;

        std::chrono::steady_clock::duration m_targetFrameTime { std::chrono::milliseconds(16) };

        std::chrono::steady_clock::duration m_lastFrameTime { std::chrono::steady_clock::duration::zero() };
    };
}

#endif // CGL_FRAMEWORK_HPP