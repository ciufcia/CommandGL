/**
 * @file render.hpp
 * @brief Core rendering system for terminal graphics
 * 
 * @details This file contains the complete rendering system for Textil, providing
 * a flexible and efficient way to render 2D graphics to terminal-based targets.
 * The system supports multiple primitive types, depth sorting, blending modes,
 * and filter pipelines for advanced visual effects.
 * 
 * The rendering architecture consists of:
 * - Primitive types for basic geometry (Vertex, Line, Ellipse, TriangleMesh)
 * - Draw call management with depth sorting
 * - Render targets for output abstraction
 * - Renderer class for primitive processing
 * - Filter pipeline integration for effects
 * 
 * The system is designed for high performance in terminal environments while
 * providing the flexibility needed for complex graphics applications. It supports
 * both immediate-mode and retained-mode rendering patterns.
 * 
 * @par Key Features:
 * - Multiple primitive types with automatic tessellation
 * - Depth-sorted rendering with transparency support
 * - Filter pipeline integration for visual effects
 * - Efficient draw call batching and sorting
 * - Memory-efficient mesh allocation system
 */

#ifndef TEXTIL_RENDER_HPP
#define TEXTIL_RENDER_HPP

#include "numeric_types.hpp"
#include "vector2.hpp"
#include <vector>
#include <variant>
#include <span>
#include "filters.hpp"
#include "transform.hpp"
#include "filter_pipeline.hpp"

namespace til
{
    class Renderer;

    /**
     * @brief Enumeration of supported primitive types for rendering
     * 
     * @details Defines the different types of geometric primitives that
     * can be rendered by the system. Each type corresponds to a specific
     * rendering path and data structure.
     */
    enum class DrawCallType : u8
    {
        Vertex,        ///< Single point/vertex primitive
        Line,          ///< Line segment between two points
        Ellipse,       ///< Elliptical/circular shape with automatic tessellation
        TriangleMesh   ///< Collection of triangles from vertex buffer
    };

    /**
     * @brief Geometric primitive definitions for rendering
     * 
     * @details This namespace contains the data structures that define
     * the various primitive types supported by the rendering system.
     * Each primitive contains the necessary geometric information for
     * rendering and tessellation.
     */
    namespace primitives
    {
        /**
         * @brief Basic vertex with position and UV coordinates
         * 
         * @details Represents a single point in 2D space with associated
         * texture coordinates. This is the fundamental building block
         * for all rendering operations.
         * 
         * The UV coordinates are used for texture sampling and procedural
         * effects during the rendering pipeline.
         */
        struct Vertex
        {
            Vector2<f32> position { 0.f, 0.f };  ///< World space position
            Vector2<f32> uv { 0.f, 0.f };        ///< Texture coordinates (0-1 range)
        };

        /**
         * @brief Line segment primitive defined by two vertices
         * 
         * @details Represents a line segment with start and end points.
         * Each endpoint includes position and UV coordinates for gradient
         * effects and texture mapping along the line.
         * 
         * Lines are rendered with proper interpolation between the
         * endpoints for smooth visual transitions.
         */
        struct Line
        {
            Vertex start;  ///< Starting vertex of the line
            Vertex end;    ///< Ending vertex of the line
        };

        /**
         * @brief Ellipse primitive with customizable radii
         * 
         * @details Represents an elliptical shape that can be stretched
         * independently along both axes. Includes UV mapping coordinates
         * for texture effects across the ellipse surface.
         * 
         * The ellipse is automatically tessellated during rendering to
         * provide smooth curves appropriate for the terminal resolution.
         */
        struct Ellipse
        {
            Vector2<f32> center;                          ///< Center point of the ellipse
            Vector2<f32> radii;                           ///< Horizontal and vertical radii
            Vector2<f32> uvTopLeft { 0.f, 0.f };         ///< UV coordinate for bounding box top-left
            Vector2<f32> uvBottomRight { 1.f, 1.f };     ///< UV coordinate for bounding box bottom-right
        };

        /**
         * @brief Triangle mesh primitive referencing vertex buffer
         * 
         * @details Represents a collection of triangles stored in the
         * renderer's vertex buffer. Provides efficient rendering of
         * complex geometry through indexed vertex access.
         * 
         * The mesh references a contiguous range of vertices that are
         * interpreted as triangles for rendering.
         */
        struct TriangleMesh
        {
            u32 firstVertex = 0;   ///< Index of the first vertex in the mesh
            u32 vertexCount = 0;   ///< Number of vertices in the mesh
        };
    }

    /**
     * @brief Variant type for primitive data payloads
     * 
     * @details Type-safe union that can hold any of the supported primitive
     * types. Used in draw call data structures to store geometry information
     * in a uniform way while maintaining type safety.
     */
    using DrawPayload = std::variant<
        primitives::Vertex,
        primitives::Line,
        primitives::Ellipse,
        primitives::TriangleMesh
    >;

    /**
     * @brief Complete data for a single draw call
     * 
     * @details Contains all the information needed to render a primitive,
     * including geometry, transformation, blending, and effects. This
     * structure is stored in the render target's draw call pool.
     * 
     * Draw call data is collected during the frame and processed during
     * the render phase, allowing for depth sorting and batching optimizations.
     */
    struct DrawCallData
    {
        DrawCallType type;                                                            ///< Type of primitive to render
        DrawPayload payload;                                                          ///< Geometry data for the primitive
        Transform transform;                                                          ///< Transformation matrix
        BlendMode blendMode;                                                          ///< How to blend with existing pixels
        FilterPipeline<filters::VertexData, filters::VertexData> *fragmentPipeline;  ///< Filter pipeline for visual effects
    };

    /**
     * @brief Lightweight draw call reference with depth information
     * 
     * @details A minimal structure that references draw call data and
     * includes depth information for sorting. This separation allows
     * efficient sorting without moving large data structures.
     * 
     * Draw calls are sorted by depth before rendering to ensure
     * proper visibility and transparency handling.
     */
    struct DrawCall
    {
        f32 depth = 0.f;        ///< Depth value used for sorting (higher values processed first during rendering)
        u32 data_index = 0;     ///< Index into the draw call data pool
    };

    /**
     * @brief Abstract render target for graphics output
     * 
     * @details Base class for all render targets that can receive graphics
     * output. Manages draw call collection, sorting, and rendering pipeline
     * execution. Provides a pixel buffer abstraction for various output types.
     * 
     * Render targets collect draw calls during the frame and process them
     * all at once during the render phase. This allows for depth sorting,
     * batching optimizations, and efficient memory usage.
     * 
     * The render target maintains a pixel buffer that can be accessed by
     * derived classes for outputting to specific devices or formats.
     * 
     * @par Usage Pattern:
     * 1. Clear the target at frame start
     * 2. Submit draw calls during the frame
     * 3. Call render() to process all draw calls
     * 4. Present the result to the display
     * 
     * @par Example Usage:
     * @code
     * class ConsoleRenderTarget : public RenderTarget {
     *     void present() {
     *         // Output pixel buffer to console
     *         for (u32 y = 0; y < getBufferSize().y; ++y) {
     *             for (u32 x = 0; x < getBufferSize().x; ++x) {
     *                 Color pixel = m_pixelBuffer[y * getBufferSize().x + x];
     *                 // Convert to terminal character and color
     *             }
     *         }
     *     }
     * };
     * @endcode
     */
    class RenderTarget
    {
    public:

        /**
         * @brief Virtual destructor for proper inheritance
         * 
         * @details Ensures proper cleanup when render targets are
         * destroyed through base class pointers.
         */
        virtual ~RenderTarget() = default;

        /**
         * @brief Process all submitted draw calls and render to pixel buffer
         * 
         * @details Executes the complete rendering pipeline:
         * 1. Sorts draw calls by depth
         * 2. Processes each draw call through the renderer
         * 3. Applies transformations and filter pipelines
         * 4. Writes results to the pixel buffer
         * 
         * This method should be called once per frame after all draw
         * calls have been submitted.
         * 
         * @note Clears the draw call list after rendering
         */
        void render();

        /**
         * @brief Assign a renderer for processing draw calls
         * 
         * @details Associates a renderer with this render target. The
         * renderer is used to process primitive geometry and apply
         * filter pipelines during rendering.
         * 
         * @param renderer Pointer to the renderer to use
         * 
         * @note The renderer must remain valid for the target's lifetime
         */
        void setRenderer(Renderer *renderer);

        /**
         * @brief Fill the entire pixel buffer with a solid color
         * 
         * @details Clears the render target by setting all pixels to
         * the specified color. This is typically used at the beginning
         * of each frame to provide a clean background.
         * 
         * @param color The color to fill the buffer with
         * 
         * @par Example Usage:
         * @code
         * renderTarget.fill(Color::Black);  // Clear to black
         * // Submit draw calls...
         * renderTarget.render();
         * @endcode
         */
        void fill(const Color &color);

    protected:

        /**
         * @brief Get the current pixel buffer dimensions
         * 
         * @details Returns the size of the pixel buffer in pixels.
         * This is used by derived classes to understand the output
         * resolution and for bounds checking.
         * 
         * @return Buffer dimensions as width and height
         */
        const Vector2<u32> &getBufferSize() const;
        
        /**
         * @brief Set the pixel buffer dimensions
         * 
         * @details Resizes the pixel buffer to the specified dimensions.
         * This should be called by derived classes when the output
         * resolution changes.
         * 
         * @param size New buffer dimensions as width and height
         * 
         * @note Triggers buffer reallocation and data loss
         */
        void setBufferSize(const Vector2<u32> &size);
        
        /**
         * @brief Get the current frame context data
         * 
         * @details Returns a reference to the base data structure that
         * provides timing and state information to filters during rendering.
         * 
         * @return Reference to the current frame data
         */
        filters::BaseData &getBaseData();

    protected:

        FilterableBuffer<Color> m_pixelBuffer {};  ///< Pixel data storage for the render target

    private:

        /**
         * @brief Set a pixel at the specified linear index
         * 
         * @details Directly sets a pixel value without blending.
         * Used internally for immediate pixel operations.
         * 
         * @param index Linear index into the pixel buffer
         * @param color Color value to set
         */
        void setPixel(u32 index, const Color &color);
        
        /**
         * @brief Set a pixel at the specified 2D position
         * 
         * @details Directly sets a pixel value without blending.
         * Converts 2D coordinates to linear index internally.
         * 
         * @param position 2D pixel coordinates
         * @param color Color value to set
         */
        void setPixel(const Vector2<u32> &position, const Color &color);
        
        /**
         * @brief Set a pixel with blending at linear index
         * 
         * @details Sets a pixel value using the specified blend mode
         * to combine with the existing pixel value.
         * 
         * @param index Linear index into the pixel buffer
         * @param color Color value to blend
         * @param blendMode How to combine with existing pixel
         */
        void setPixelWithBlend(u32 index, const Color &color, BlendMode blendMode);
        
        /**
         * @brief Set a pixel with blending at 2D position
         * 
         * @details Sets a pixel value using the specified blend mode
         * to combine with the existing pixel value.
         * 
         * @param position 2D pixel coordinates
         * @param color Color value to blend
         * @param blendMode How to combine with existing pixel
         */
        void setPixelWithBlend(const Vector2<u32> &position, const Color &color, BlendMode blendMode);

    /**
     * @brief Sort all draw calls by depth value
     * 
      * @details Sorts the draw call list in descending depth order. Higher
      * depth values are processed first, so draw calls with smaller depth
      * values are blended last and therefore appear in front.
     */
        void sortDrawCalls();

        /**
         * @brief Register a new draw call for rendering
         * 
         * @details Adds a draw call to the pending list for processing
         * during the next render() call. Used internally by the renderer.
         * 
         * @param drawCallData Complete draw call information
         * @param depth Depth value for sorting
         */
        void registerDrawCall(const DrawCallData &drawCallData, f32 depth);
        
        /**
         * @brief Clear all pending draw calls
         * 
         * @details Removes all draw calls from the pending list.
         * Called automatically after render() completes.
         */
        void clearDrawCalls();

    private:

        Vector2<u32> m_bufferSize { 0u, 0u };        ///< Current pixel buffer dimensions

        std::vector<DrawCall> m_drawCalls {};        ///< Pending draw calls for rendering
        std::vector<DrawCallData> m_drawCallDataPool {}; ///< Storage pool for draw call data

        filters::BaseData m_baseData;                ///< Current frame context data

        Renderer *m_renderer = nullptr;              ///< Associated renderer for processing

    friend class Renderer;
    friend class Framework;
    };

    /**
     * @brief Core renderer for processing geometric primitives
     * 
     * @details The Renderer class handles the conversion of geometric primitives
     * into pixel data through tessellation, transformation, and filter pipeline
     * application. It manages vertex buffers for complex geometry and provides
     * both deferred and immediate rendering modes.
     * 
     * The renderer supports multiple primitive types and automatically handles
     * tessellation for complex shapes like ellipses. It integrates tightly with
     * the filter pipeline system to enable advanced visual effects.
     * 
     * @par Rendering Modes:
     * - Deferred: Submit draw calls for later processing during render()
     * - Immediate: Process and render primitives immediately
     * 
     * @par Mesh Management:
     * The renderer maintains a vertex buffer for complex geometry and provides
     * allocation mechanisms for efficient mesh storage and reuse.
     * 
     * @par Example Usage:
     * @code
     * Renderer renderer;
     * RenderTarget target;
     * target.setRenderer(&renderer);
     * 
     * // Add some geometry
     * u32 meshId = renderer.addMesh(triangleVertices, 3);
     * primitives::TriangleMesh mesh{0, 3};
     * 
     * // Render with effects
     * FilterPipeline<filters::VertexData, filters::VertexData> pipeline;
     * renderer.draw(target, mesh, transform, pipeline);
     * 
     * target.render();  // Process all draw calls
     * @endcode
     */
    class Renderer
    {
    public:

        /**
         * @brief Mesh allocation result containing vertex access
         * 
         * @details Provides information about an allocated mesh region
         * including the starting vertex index and a span for direct
         * vertex data access. This allows efficient mesh building.
         */
        struct MeshAllocation {
            u32 firstVertex;                        ///< Index of first vertex in allocation
            std::span<primitives::Vertex> vertices; ///< Direct access to allocated vertex range
        };

        /**
         * @brief Submit a vertex primitive for deferred rendering
         * 
         * @details Adds a single vertex to the render queue for processing
         * during the next render() call. The vertex will be transformed
         * and processed through the filter pipeline.
         * 
         * @param renderTarget Target to render onto
         * @param vertex Vertex primitive to render
         * @param transform Transformation to apply
         * @param fragmentPipeline Filter pipeline for effects
         * @param depth Depth value for sorting (default: 0.0)
         * @param blendMode How to blend with existing pixels (default: Alpha)
         */
        void draw(RenderTarget &renderTarget, const primitives::Vertex &vertex, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, f32 depth = 0.f, BlendMode blendMode = BlendMode::Alpha);
        
        /**
         * @brief Submit a line primitive for deferred rendering
         * 
         * @details Adds a line segment to the render queue. The line will
         * be rasterized with proper interpolation between endpoints.
         * 
         * @param renderTarget Target to render onto
         * @param line Line primitive to render
         * @param transform Transformation to apply
         * @param fragmentPipeline Filter pipeline for effects
         * @param depth Depth value for sorting (default: 0.0)
         * @param blendMode How to blend with existing pixels (default: Alpha)
         */
        void draw(RenderTarget &renderTarget, const primitives::Line &line, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, f32 depth = 0.f, BlendMode blendMode = BlendMode::Alpha);
        
        /**
         * @brief Submit an ellipse primitive for deferred rendering
         * 
         * @details Adds an ellipse to the render queue. The ellipse will
         * be automatically tessellated for smooth rendering.
         * 
         * @param renderTarget Target to render onto
         * @param ellipse Ellipse primitive to render
         * @param transform Transformation to apply
         * @param fragmentPipeline Filter pipeline for effects
         * @param depth Depth value for sorting (default: 0.0)
         * @param blendMode How to blend with existing pixels (default: Alpha)
         */
        void draw(RenderTarget &renderTarget, const primitives::Ellipse &ellipse, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, f32 depth = 0.f, BlendMode blendMode = BlendMode::Alpha);
        
        /**
         * @brief Submit a triangle mesh for deferred rendering
         * 
         * @details Adds a triangle mesh from the vertex buffer to the render
         * queue. The mesh vertices are referenced by index for efficiency.
         * 
         * @param renderTarget Target to render onto
         * @param mesh Triangle mesh primitive to render
         * @param transform Transformation to apply
         * @param fragmentPipeline Filter pipeline for effects
         * @param depth Depth value for sorting (default: 0.0)
         * @param blendMode How to blend with existing pixels (default: Alpha)
         */
        void draw(RenderTarget &renderTarget, const primitives::TriangleMesh &mesh, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, f32 depth = 0.f, BlendMode blendMode = BlendMode::Alpha);

        /**
         * @brief Render a vertex primitive immediately
         * 
         * @details Processes and renders a vertex immediately without
         * queuing. Useful for debug rendering or simple cases.
         * 
         * @param renderTarget Target to render onto
         * @param vertex Vertex primitive to render
         * @param transform Transformation to apply
         * @param fragmentPipeline Filter pipeline for effects
         * @param blendMode How to blend with existing pixels (default: Alpha)
         */
        void drawImmediate(RenderTarget &renderTarget, const primitives::Vertex &vertex, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, BlendMode blendMode = BlendMode::Alpha);
        
        /**
         * @brief Render a line primitive immediately
         * 
         * @details Processes and renders a line immediately without queuing.
         * 
         * @param renderTarget Target to render onto
         * @param line Line primitive to render
         * @param transform Transformation to apply
         * @param fragmentPipeline Filter pipeline for effects
         * @param blendMode How to blend with existing pixels (default: Alpha)
         */
        void drawImmediate(RenderTarget &renderTarget, const primitives::Line &line, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, BlendMode blendMode = BlendMode::Alpha);
        
        /**
         * @brief Render an ellipse primitive immediately
         * 
         * @details Processes and renders an ellipse immediately without queuing.
         * 
         * @param renderTarget Target to render onto
         * @param ellipse Ellipse primitive to render
         * @param transform Transformation to apply
         * @param fragmentPipeline Filter pipeline for effects
         * @param blendMode How to blend with existing pixels (default: Alpha)
         */
        void drawImmediate(RenderTarget &renderTarget, const primitives::Ellipse &ellipse, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, BlendMode blendMode = BlendMode::Alpha);
        
        /**
         * @brief Render a triangle mesh immediately
         * 
         * @details Processes and renders a triangle mesh immediately without queuing.
         * 
         * @param renderTarget Target to render onto
         * @param mesh Triangle mesh primitive to render
         * @param transform Transformation to apply
         * @param fragmentPipeline Filter pipeline for effects
         * @param blendMode How to blend with existing pixels (default: Alpha)
         */
        void drawImmediate(RenderTarget &renderTarget, const primitives::TriangleMesh &mesh, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, BlendMode blendMode = BlendMode::Alpha);

        /**
         * @brief Draw a single pixel immediately
         * 
         * @details Directly sets a pixel color at the specified position
         * without going through the full rendering pipeline. Useful for
         * simple pixel operations and debug output.
         * 
         * @param renderTarget Target to render onto
         * @param position Pixel coordinates to set
         * @param color Color value to apply
         * @param blendMode How to blend with existing pixel (default: Alpha)
         */
        void drawImmediatePixel(RenderTarget &renderTarget, const Vector2<u32> &position, const Color &color, BlendMode blendMode = BlendMode::Alpha);
        
        /**
         * @brief Draw a simple line immediately
         * 
         * @details Draws a line between two pixel coordinates using a solid
         * color. This bypasses the full primitive pipeline for efficiency.
         * 
         * @param renderTarget Target to render onto
         * @param start Starting pixel coordinates
         * @param end Ending pixel coordinates
         * @param color Color for the line
         * @param blendMode How to blend with existing pixels (default: Alpha)
         */
        void drawImmediateLine(RenderTarget &renderTarget, const Vector2<u32> &start, const Vector2<u32> &end, const Color &color, BlendMode blendMode = BlendMode::Alpha);

        /**
         * @brief Add a triangle mesh to the vertex buffer
         * 
         * @details Copies vertex data into the managed vertex buffer and
         * returns the starting index. The mesh can then be referenced
         * by TriangleMesh primitives for efficient rendering.
         * 
         * @param vertices Pointer to vertex data array
         * @param vertexCount Number of vertices to copy
         * @return Starting index of the added vertices
         * 
         * @note Vertices are copied into internal storage
         * @warning Original vertex data can be freed after this call
         */
        u32 addMesh(primitives::Vertex *vertices, u32 vertexCount);

        /**
         * @brief Allocate space for a mesh in the vertex buffer
         * 
         * @details Reserves space in the vertex buffer and returns direct
         * access for efficient mesh building. This avoids copying vertex
         * data and allows in-place construction.
         * 
         * @param vertexCount Number of vertices to allocate
         * @return MeshAllocation with vertex span for direct access
         * 
         * @par Example Usage:
         * @code
         * auto allocation = renderer.allocateMesh(6);  // Two triangles
         * allocation.vertices[0] = {{0, 0}, {0, 0}};
         * allocation.vertices[1] = {{1, 0}, {1, 0}};
         * // ... fill remaining vertices
         * 
         * primitives::TriangleMesh mesh{allocation.firstVertex, 6};
         * renderer.draw(target, mesh, transform, pipeline);
         * @endcode
         */
        MeshAllocation allocateMesh(u32 vertexCount);

        /**
         * @brief Clear all mesh data from the vertex buffer
         * 
         * @details Removes all stored vertex data and resets the buffer.
         * This invalidates all existing TriangleMesh references and should
         * be used carefully.
         * 
         * @warning Invalidates all existing mesh references
         */
        void clearMeshes();

    private:

        /**
         * @brief Clip a line segment to fit within a rectangle
         * 
         * @details Uses Cohen-Sutherland line clipping algorithm to trim
         * line segments to the specified rectangular bounds. Used internally
         * for viewport clipping.
         * 
         * @param start Starting point of the line (modified in-place)
         * @param end Ending point of the line (modified in-place)
         * @param rectSize Rectangle bounds for clipping
         * @return True if any part of the line is visible after clipping
         */
        bool clipLineToRect(Vector2<f32>& start, Vector2<f32>& end, const Vector2<u32>& rectSize) const;

        std::vector<primitives::Vertex> m_meshVertices {};          ///< Vertex buffer for triangle meshes

        FilterableBuffer<filters::VertexData> m_fragmentInputBuffer {};  ///< Input buffer for filter pipelines
        FilterableBuffer<filters::VertexData> m_fragmentOutputBuffer {}; ///< Output buffer for filter pipelines
    };
}

#endif // TEXTIL_RENDER_HPP