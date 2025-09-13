/**
 * @file window.hpp
 * @brief Window class for managing independent rendering contexts
 * @details Provides a window abstraction for organizing graphics into separate
 *          layered rendering contexts. Windows can be positioned, sized, and
 *          have independent transformation and filtering pipelines.
 */

#ifndef TIL_WINDOW_HPP
#define TIL_WINDOW_HPP

#include "render.hpp"
#include "filter_pipeline.hpp"
#include <list>
#include <string>

namespace til
{
    /**
     * @brief Independent rendering context with position, size, and filter pipelines
     * @details A Window represents a rectangular rendering area that can be positioned
     *          anywhere on the screen. It inherits from RenderTarget to provide full
     *          graphics rendering capabilities while adding window-specific features:
     *          - Position and size management
     *          - Depth-based layering for proper window ordering
     *          - Post-processing pipeline for visual effects
     *          - Character conversion pipeline for terminal output
     *          - Independent character buffer for terminal rendering
     * 
     *          Windows are managed by WindowManager and can overlap, with depth
     *          values determining rendering order. Each window maintains its own
     *          coordinate system where (0,0) represents the window's top-left corner.
     */
    class Window : public RenderTarget
    {
    public:
        /**
         * @brief Get current window position in screen coordinates
         * @return Position vector where (0,0) is screen top-left
         * @details Returns the window's position relative to the console screen.
         *          Position affects where the window appears when rendered by WindowManager.
         */
        const Vector2<i32> &getPosition() const;
        
        /**
         * @brief Set window position in screen coordinates
         * @param position New position where (0,0) is screen top-left
         * @details Moves the window to the specified screen position. Negative values
         *          can position windows partially off-screen. Position changes take
         *          effect on the next render cycle.
         */
        void setPosition(const Vector2<i32> &position);
        
        /**
         * @brief Get current window dimensions
         * @return Size vector containing width and height in pixels/characters
         * @details Returns the current window size which determines the rendering area
         *          and the size of internal buffers.
         */
        const Vector2<u32> &getSize() const;
        
        /**
         * @brief Set window dimensions
         * @param size New size vector (width, height)
         * @details Resizes the window and reallocates internal buffers to match.
         *          Larger windows require more memory for rendering buffers.
         *          Size changes take effect immediately.
         */
        void setSize(const Vector2<u32> &size);

        /**
         * @brief Execute post-processing filter pipeline on rendered content
         * @details Runs the post-processing pipeline to apply visual effects to
         *          the rendered graphics before character conversion. This allows
         *          for effects like blur, color correction, or other image filters.
         */
        void runPostProcessingPipeline();
        
        /**
         * @brief Execute character conversion pipeline for terminal output
         * @details Runs the character pipeline to convert rendered graphics into
         *          character cells suitable for terminal display. This process
         *          determines which characters and colors best represent the graphics.
         */
        void runCharacterPipeline();

        /**
         * @brief Get character cell at specific buffer position
         * @param index Linear buffer index (row * width + column)
         * @return CharacterCell at the specified position
         * @details Provides access to the terminal character representation at
         *          a specific position in the window's character buffer.
         *          Index must be within buffer bounds.
         */
        CharacterCell getCharacterCell(u32 index) const;

    public:
        Vector2<i32> m_position { 0, 0 }; ///< Window position in screen coordinates

        f32 depth = 0.f; ///< Depth value for layering (higher values render on top)

        u32 id = 0; ///< Unique identifier for this window

        /// Post-processing pipeline for applying visual effects to rendered graphics
        FilterPipeline<Color, Color> postProcessPipeline {};
        
        /// Character conversion pipeline for transforming graphics to terminal characters
        FilterPipeline<Color, CharacterCell> characterPipeline {};

    private:
        /// Buffer storing character representation for terminal output
        FilterableBuffer<CharacterCell> m_characterBuffer {};
    };
}

#endif // TIL_WINDOW_HPP