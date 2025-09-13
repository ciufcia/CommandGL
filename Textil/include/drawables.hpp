/**
 * @file drawables.hpp
 * @brief Drawable objects and graphics primitives for terminal graphics
 * 
 * @details This file contains the drawable object system for Textil, providing
 * a collection of graphics primitives that can be rendered to the terminal.
 * The system is built around a base Drawable interface with concrete
 * implementations for common graphics shapes and sprites.
 * 
 * All drawable objects support:
 * - 2D transformations through the base class `transform` member (position, rotation, scale)
 * - Filter pipelines for custom graphics effects
 * - UV coordinate mapping for texture sampling
 * - Integration with the rendering system
 * 
 * The drawable system is designed for efficiency in terminal environments
 * while providing flexibility for complex graphics applications.
 * 
 * @par Transformation System:
 * The base Drawable class provides a public `Transform transform` member that
 * controls the object's position, rotation, and scale in 2D space. All derived
 * classes inherit this functionality, ensuring consistent transformation
 * behavior across all drawable types.
 * 
 * @note All drawable objects work with the coordinate system where (0,0)
 * represents the top-left corner of the terminal
 */

#include "render.hpp"
#include "texture.hpp"
#include "transform.hpp"

namespace til
{
    /**
     * @brief Abstract base class for all drawable objects
     * 
     * @details The Drawable class defines the interface that all renderable
     * objects must implement. It provides a common abstraction for different
     * types of graphics primitives and ensures consistent rendering behavior
     * across all drawable types.
     * 
     * The base class provides a Transform member that handles positioning,
     * rotation, and scaling for all drawable objects. Derived classes must
     * implement the draw() method to define their specific rendering behavior.
     * 
     * @par Design Philosophy:
     * The drawable system follows a component-based approach where the base
     * class provides common transformation functionality, while derived classes
     * focus on their specific rendering implementation. This allows for
     * flexible composition and easy extension while maintaining consistency.
     * 
     * @par Example Usage:
     * @code
     * MyDrawable drawable;
     * drawable.transform.position = {100, 50};
     * drawable.transform.rotation = 45.0f;
     * drawable.transform.scale = {2.0f, 2.0f};
     * drawable.draw(renderer, target);
     * @endcode
     */
    class Drawable
    {
    public:

        Transform transform;  ///< Transformation matrix for position, rotation, and scale

        /**
         * @brief Virtual destructor for proper polymorphic destruction
         * 
         * @details Ensures that derived class destructors are called
         * correctly when objects are destroyed through base class pointers.
         * This is essential for proper resource cleanup in polymorphic
         * inheritance hierarchies.
         */
        virtual ~Drawable() = default;

        /**
         * @brief Render the drawable object to a target
         * 
         * @details Pure virtual method that must be implemented by all
         * drawable types. This method defines how the object should be
         * rendered using the provided renderer and target.
         * 
         * Implementations should:
         * - Apply any transformations
         * - Set up appropriate shaders/filters
         * - Generate and submit geometry
         * - Handle texture sampling if applicable
         * 
         * @param renderer The renderer to use for drawing operations
         * @param target The render target to draw onto
         * 
         * @note This method should not modify the renderer's global state
         * @warning Implementations must handle error conditions gracefully
         */
        virtual void draw(Renderer &renderer, RenderTarget &target) = 0;
    };

    /**
     * @brief A drawable sprite with texture support and filtering capabilities
     * 
     * @details The Sprite class represents a textured quad that can be positioned,
     * scaled, and rotated in 2D space. It supports texture mapping with UV
     * coordinates and provides a complete filter pipeline for applying visual
     * effects during rendering.
     * 
     * Sprites are one of the most commonly used drawable objects, suitable for:
     * - Character graphics and animations
     * - UI elements and icons
     * - Textured backgrounds
     * - Particle effects
     * 
     * The sprite automatically handles texture sampling and provides methods
     * for managing filter effects that can be applied during rendering.
     * 
     * @par Performance Notes:
     * - Sprites with no filters render faster than filtered sprites
     * - Texture changes trigger internal state updates
     * - Filter modifications require pipeline rebuilding
     * 
     * @par Example Usage:
     * @code
     * // Create a sprite with a texture
     * Sprite playerSprite(&playerTexture);
     * playerSprite.transform.position = {100, 50};
     * playerSprite.transform.scale = {2.0f, 2.0f};
     * playerSprite.size = {32, 32};
     * 
     * // Add a color filter
     * ColorFilter colorFilter;
     * playerSprite.addFilter(&colorFilter);
     * 
     * // Render the sprite
     * playerSprite.draw(renderer, renderTarget);
     * @endcode
     */
    class Sprite : public Drawable
    {
    public:

        Vector2<f32> size { 10.f, 10.f };  ///< Dimensions of the sprite in world units

    public:

        /**
         * @brief Default constructor creating an empty sprite
         * 
         * @details Creates a sprite with no associated texture. The sprite
         * will not be visible until a texture is assigned using setTexture().
         * Default size is set to 10x10 units.
         */
        Sprite();
        
        /**
         * @brief Constructor with texture assignment
         * 
         * @details Creates a sprite and immediately associates it with the
         * provided texture. The sprite will be ready for rendering after
         * construction.
         * 
         * @param texture Pointer to the texture to use for this sprite
         * 
         * @note The texture pointer is stored but not owned by the sprite
         * @warning The texture must remain valid for the sprite's lifetime
         */
        Sprite(Texture *texture);

        /**
         * @brief Set the texture used by this sprite
         * 
         * @details Associates a new texture with this sprite. The texture
         * will be used for sampling during rendering. Setting a null texture
         * will make the sprite invisible.
         * 
         * @param texture Pointer to the new texture, or nullptr to clear
         * 
         * @note Changing textures may trigger internal state updates
         * @warning The texture must remain valid until changed or sprite is destroyed
         */
        void setTexture(Texture *texture);
        
        /**
         * @brief Get the currently assigned texture
         * 
         * @details Returns a pointer to the texture currently associated
         * with this sprite. May return nullptr if no texture is assigned.
         * 
         * @return Pointer to the current texture, or nullptr if none assigned
         */
        Texture* getTexture() const;

        /**
         * @brief Render the sprite to the specified target
         * 
         * @details Draws the sprite as a textured quad using the current
         * transformation matrix and applied filters. The sprite geometry
         * is generated based on the size and transform properties.
         * 
         * @param renderer The renderer to use for drawing operations
         * @param target The render target to draw onto
         * 
         * @note Does nothing if no texture is assigned
         */
        void draw(Renderer &renderer, RenderTarget &target) override;

        /**
         * @brief Add a filter to the end of the filter pipeline
         * 
         * @details Appends a new filter to the sprite's effect pipeline.
         * Filters are applied in the order they are added, allowing for
         * complex layered effects.
         * 
         * @param filter Pointer to the filter to add
         * 
         * @note The filter must remain valid for the sprite's lifetime
         * @warning Adding filters requires pipeline rebuilding
         */
        void addFilter(BaseFilter *filter);
        
        /**
         * @brief Insert a filter at a specific position in the pipeline
         * 
         * @details Inserts a filter at the specified index, shifting
         * subsequent filters to later positions. Allows precise control
         * over filter execution order.
         * 
         * @param index Position to insert the filter (0-based)
         * @param filter Pointer to the filter to insert
         * 
         * @note If index exceeds pipeline size, filter is appended
         */
        void insertFilter(u32 index, BaseFilter *filter);
        
        /**
         * @brief Remove a filter from the pipeline
         * 
         * @details Removes the filter at the specified index from the
         * pipeline. All subsequent filters shift to earlier positions.
         * 
         * @param index Index of the filter to remove (0-based)
         * 
         * @throws InvalidArgumentError If index is out of range
         */
        void removeFilter(u32 index);
        
        /**
         * @brief Clear all filters from the pipeline
         * 
         * @details Removes all filters from the sprite's effect pipeline,
         * restoring default rendering behavior. This is useful for
         * resetting visual effects or changing the entire filter setup.
         */
        void clearFilters();

    private:

        Texture *m_texture = nullptr;  ///< Pointer to the assigned texture

        FilterPipeline<filters::VertexData, filters::VertexData> m_fragmentPipeline {};  ///< Filter pipeline for visual effects
        filters::TextureSampler m_textureSampler { nullptr };                           ///< Texture sampling filter

    };

    /**
     * @brief A drawable rectangle primitive with customizable appearance
     * 
     * @details The Rectangle class provides a simple filled rectangle that
     * can be positioned, scaled, and filtered. Unlike sprites, rectangles
     * don't require textures and are useful for solid colored shapes,
     * backgrounds, and UI elements.
     * 
     * Rectangles support the full filter pipeline system, allowing for
     * complex visual effects like gradients, patterns, and color modifications.
     * They provide a lightweight alternative to textured sprites when only
     * solid colors or generated patterns are needed.
     * 
     * @par Use Cases:
     * - Solid colored backgrounds
     * - UI panels and borders
     * - Debug visualization
     * - Procedural patterns via filters
     * 
     * @par Example Usage:
     * @code
     * Rectangle panel;
     * panel.transform.position = {50, 100};
     * panel.topLeft = {0, 0};
     * panel.size = {200, 150};
     * 
     * // Add a color filter for solid color
     * SolidColorFilter colorFilter(Color::Blue);
     * panel.fragmentPipeline.addFilter(&colorFilter);
     * panel.fragmentPipeline.build();
     * 
     * panel.draw(renderer, target);
     * @endcode
     */
    class Rectangle : public Drawable
    {
    public:

        Vector2<f32> topLeft { 0.f, 0.f };          ///< Top-left corner in local coordinates
        Vector2<f32> size { 10.f, 10.f };           ///< Dimensions of the rectangle

        FilterPipeline<filters::VertexData, filters::VertexData> fragmentPipeline {};  ///< Filter pipeline for visual effects

    public:

        /**
         * @brief Default constructor creating a 10x10 rectangle
         * 
         * @details Creates a rectangle with default size and position.
         * The rectangle will be invisible until filters are added to
         * provide color or pattern generation.
         */
        Rectangle() = default;

        /**
         * @brief Render the rectangle to the specified target
         * 
         * @details Draws the rectangle as a filled quad using the current
         * transformation matrix and applied filters. The rectangle geometry
         * is generated based on the topLeft and size properties.
         * 
         * @param renderer The renderer to use for drawing operations
         * @param target The render target to draw onto
         * 
         * @note Rectangles without filters may not be visible
         */
        void draw(Renderer &renderer, RenderTarget &target) override;
    };

    /**
     * @brief A drawable polygon with custom vertices and UV mapping
     * 
     * @details The Polygon class provides a flexible way to render custom
     * shapes with arbitrary vertices. It supports automatic triangulation
     * of complex polygons and provides UV coordinate mapping for texture
     * sampling or procedural effects.
     * 
     * The polygon system automatically handles:
     * - Concave and convex polygon rendering
     * - Automatic triangulation using ear clipping
     * - Custom UV coordinate assignment
     * - Mesh rebuilding when vertices change
     * 
     * Polygons are ideal for:
     * - Custom shapes and logos
     * - Complex UI elements
     * - Terrain and level geometry
     * - Vector graphics rendering
     * 
     * @par Performance Notes:
     * - Mesh rebuilding occurs when vertices change
     * - Complex polygons with many vertices may impact performance
     * - UV coordinates are optional but recommended for textured polygons
     * 
     * @par Example Usage:
     * @code
     * Polygon triangle;
     * triangle.addPoint({0, 0});
     * triangle.addPoint({50, 0});
     * triangle.addPoint({25, 50});
     * 
     * // Optional UV coordinates
     * triangle.setUVs({{0, 0}, {1, 0}, {0.5f, 1}});
     * 
     * triangle.draw(renderer, target);
     * @endcode
     * 
     * @warning Polygons must have at least 3 vertices to be rendered
     * @note Self-intersecting polygons may not render correctly
     */
    class Polygon : public Drawable
    {
    public:

        FilterPipeline<filters::VertexData, filters::VertexData> fragmentPipeline {};  ///< Filter pipeline for visual effects

        /**
         * @brief Get the polygon's vertex points
         * 
         * @details Returns a constant reference to the vector containing
         * all vertex positions in local coordinates. Points are ordered
         * counter-clockwise for proper triangulation.
         * 
         * @return Constant reference to the vertex points vector
         */
        const std::vector<Vector2<f32>>& getPoints() const;
        
        /**
         * @brief Set all vertex points at once
         * 
         * @details Replaces the entire vertex list with a new set of points.
         * This triggers mesh rebuilding on the next draw call. Points should
         * be ordered counter-clockwise for proper rendering.
         * 
         * @param points Vector of vertex positions in local coordinates
         * 
         * @note Triggers mesh rebuilding
         * @warning Requires at least 3 points for valid polygon
         */
        void setPoints(const std::vector<Vector2<f32>> &points);
        
        /**
         * @brief Add a single vertex point to the polygon
         * 
         * @details Appends a new vertex to the end of the point list.
         * This is useful for building polygons incrementally or adding
         * points based on user input.
         * 
         * @param pt The vertex position to add in local coordinates
         * 
         * @note Triggers mesh rebuilding on next draw
         */
        void addPoint(const Vector2<f32> &pt);
        
        /**
         * @brief Insert a vertex at a specific position
         * 
         * @details Inserts a new vertex at the specified index, shifting
         * subsequent vertices to later positions. Useful for polygon
         * editing and refinement.
         * 
         * @param index Position to insert the vertex (0-based)
         * @param pt The vertex position to insert
         * 
         * @note If index exceeds size, point is appended
         */
        void insertPoint(u32 index, const Vector2<f32> &pt);
        
        /**
         * @brief Remove a vertex from the polygon
         * 
         * @details Removes the vertex at the specified index. All subsequent
         * vertices shift to earlier positions. Triggers mesh rebuilding.
         * 
         * @param index Index of the vertex to remove (0-based)
         * 
         * @throws InvalidArgumentError If index is out of range
         * @warning Removing vertices may invalidate the polygon shape
         */
        void removePoint(u32 index);
        
        /**
         * @brief Clear all vertices from the polygon
         * 
         * @details Removes all vertices, resulting in an empty polygon
         * that will not be rendered. This is useful for resetting the
         * polygon or preparing for a new shape.
         */
        void clearPoints();

        /**
         * @brief Get the polygon's UV coordinates
         * 
         * @details Returns a constant reference to the vector containing
         * UV coordinates for each vertex. UV coordinates are used for
         * texture sampling and procedural effects.
         * 
         * @return Constant reference to the UV coordinates vector
         */
        const std::vector<Vector2<f32>>& getUVs() const;
        
        /**
         * @brief Set UV coordinates for all vertices
         * 
         * @details Assigns UV coordinates to match the vertex positions.
         * The UV vector should have the same size as the points vector
         * for proper mapping.
         * 
         * @param uvs Vector of UV coordinates (0-1 range typical)
         * 
         * @note UV count should match vertex count
         */
        void setUVs(const std::vector<Vector2<f32>> &uvs);
        
        /**
         * @brief Set UV coordinate for a specific vertex
         * 
         * @details Updates the UV coordinate for a single vertex identified
         * by its index. Useful for fine-tuning texture mapping on specific
         * parts of the polygon.
         * 
         * @param index Index of the vertex to update (0-based)
         * @param uv New UV coordinate for the vertex
         * 
         * @throws InvalidArgumentError If index is out of range
         */
        void setPointUV(u32 index, const Vector2<f32> &uv);
        
        /**
         * @brief Clear all UV coordinates
         * 
         * @details Removes all custom UV coordinates, reverting to
         * automatic UV generation based on vertex positions. This
         * can be useful when switching from textured to solid rendering.
         */
        void clearUVs();

        /**
         * @brief Render the polygon to the specified target
         * 
         * @details Draws the polygon using automatic triangulation and
         * the current transformation matrix. Rebuilds the mesh if vertices
         * have changed since the last draw call.
         * 
         * @param renderer The renderer to use for drawing operations
         * @param target The render target to draw onto
         * 
         * @note Polygons with fewer than 3 vertices are not rendered
         */
        void draw(Renderer &renderer, RenderTarget &target) override;

    private:
        /**
         * @brief Rebuild the internal triangle mesh
         * 
         * @details Performs automatic triangulation of the polygon vertices
         * using ear clipping algorithm. Also generates UV coordinates if
         * none are provided. Called automatically when vertices change.
         */
        void rebuildMesh();
        
        /**
         * @brief Calculate signed area of a polygon
         * 
         * @details Computes the signed area to determine polygon winding
         * order. Used internally for triangulation algorithm.
         * 
         * @param pts Vector of polygon vertices
         * @return Signed area (positive for counter-clockwise)
         */
        static f32 signedArea(const std::vector<Vector2<f32>> &pts);
        
        /**
         * @brief Test if a point is inside a triangle
         * 
         * @details Uses barycentric coordinates to test point containment.
         * Used internally by the triangulation algorithm.
         * 
         * @param p Point to test
         * @param a First triangle vertex
         * @param b Second triangle vertex
         * @param c Third triangle vertex
         * @return True if point is inside triangle
         */
        static bool pointInTriangle(const Vector2<f32> &p, const Vector2<f32> &a, const Vector2<f32> &b, const Vector2<f32> &c);

    private:
    
        std::vector<Vector2<f32>> m_points;           ///< Polygon vertex positions
        std::vector<Vector2<f32>> m_uvs;              ///< UV coordinates for vertices
        bool m_hasCustomUVs = false;                  ///< Whether custom UVs are assigned

        std::vector<primitives::Vertex> m_builtVertices;  ///< Triangulated mesh vertices
        bool m_meshDirty = true;                          ///< Whether mesh needs rebuilding
    };

    /**
     * @brief A drawable single point primitive
     * 
     * @details The Point class represents a single pixel or point that can
     * be positioned and filtered. While simple, points are useful for
     * particle effects, debug visualization, and pixel-perfect graphics.
     * 
     * Points support the full filter pipeline, allowing for color changes,
     * effects, and procedural modifications. They provide the most lightweight
     * drawable primitive for applications that need to render many individual
     * pixels efficiently.
     * 
     * @par Use Cases:
     * - Particle systems
     * - Debug markers and visualization
     * - Pixel art and low-resolution graphics
     * - Mathematical plotting and data visualization
     * 
     * @par Example Usage:
     * @code
     * Point marker;
     * marker.transform.position = {150, 75};
     * marker.position = {0, 0};  // Local offset
     * marker.uv = {0.5f, 0.5f};  // Center UV
     * 
     * // Add color filter
     * SolidColorFilter redColor(Color::Red);
     * marker.fragmentPipeline.addFilter(&redColor);
     * marker.fragmentPipeline.build();
     * 
     * marker.draw(renderer, target);
     * @endcode
     */
    class Point : public Drawable
    {
    public:
    
        FilterPipeline<filters::VertexData, filters::VertexData> fragmentPipeline {};  ///< Filter pipeline for visual effects

        Vector2<f32> position { 0.f, 0.f };  ///< Local position offset
        Vector2<f32> uv { 0.f, 0.f };         ///< UV coordinate for texture sampling

        /**
         * @brief Render the point to the specified target
         * 
         * @details Draws a single point at the transformed position.
         * The point uses the assigned UV coordinate for any texture
         * sampling performed by filters.
         * 
         * @param renderer The renderer to use for drawing operations
         * @param target The render target to draw onto
         */
        void draw(Renderer &renderer, RenderTarget &target) override;
    };

    /**
     * @brief A drawable line segment primitive
     * 
     * @details The LineDrawable class represents a line segment between two
     * points with customizable endpoints and UV mapping. Lines are useful
     * for wireframe graphics, UI elements, and geometric visualization.
     * 
     * Lines support gradient effects through UV coordinate mapping and
     * can be styled using the filter pipeline. The line is rendered as
     * a connection between the start and end points with proper UV
     * interpolation along its length.
     * 
     * @par Use Cases:
     * - Wireframe rendering
     * - UI borders and dividers
     * - Mathematical graphs and plots
     * - Debug ray visualization
     * 
     * @par Example Usage:
     * @code
     * LineDrawable border;
     * border.transform.position = {10, 10};
     * border.start = {0, 0};
     * border.end = {100, 0};
     * border.uvStart = {0, 0};
     * border.uvEnd = {1, 0};
     * 
     * // Add gradient filter
     * GradientFilter gradient(Color::Blue, Color::Red);
     * border.fragmentPipeline.addFilter(&gradient);
     * border.fragmentPipeline.build();
     * 
     * border.draw(renderer, target);
     * @endcode
     */
    class LineDrawable : public Drawable
    {
    public:

        FilterPipeline<filters::VertexData, filters::VertexData> fragmentPipeline {};  ///< Filter pipeline for visual effects

        Vector2<f32> start { 0.f, 0.f };      ///< Starting point of the line
        Vector2<f32> end { 1.f, 1.f };        ///< Ending point of the line
        Vector2<f32> uvStart { 0.f, 0.f };    ///< UV coordinate at line start
        Vector2<f32> uvEnd { 1.f, 0.f };      ///< UV coordinate at line end

        /**
         * @brief Render the line to the specified target
         * 
         * @details Draws a line segment from start to end points with
         * UV coordinate interpolation. The line uses the transformation
         * matrix for positioning and scaling.
         * 
         * @param renderer The renderer to use for drawing operations
         * @param target The render target to draw onto
         */
        void draw(Renderer &renderer, RenderTarget &target) override;
    };

    /**
     * @brief A drawable ellipse primitive with customizable radii
     * 
     * @details The EllipseDrawable class represents an elliptical shape
     * that can be stretched independently along both axes. It supports
     * UV mapping across the ellipse surface and provides full filter
     * pipeline support for visual effects.
     * 
     * Ellipses are rendered as filled shapes and can represent:
     * - Circles (when radii are equal)
     * - Ovals and ellipses
     * - UI buttons and badges
     * - Selection indicators
     * 
     * The ellipse is automatically tessellated for smooth rendering
     * and supports proper UV coordinate mapping for texture effects.
     * 
     * @par Use Cases:
     * - Circular UI elements
     * - Selection markers
     * - Particle effects
     * - Geometric shapes in diagrams
     * 
     * @par Example Usage:
     * @code
     * EllipseDrawable button;
     * button.transform.position = {200, 100};
     * button.center = {0, 0};
     * button.radii = {30, 20};  // Width=60, Height=40
     * button.uvTopLeft = {0, 0};
     * button.uvBottomRight = {1, 1};
     * 
     * // Add texture filter
     * TextureFilter buttonTexture(&buttonImage);
     * button.fragmentPipeline.addFilter(&buttonTexture);
     * button.fragmentPipeline.build();
     * 
     * button.draw(renderer, target);
     * @endcode
     */
    class EllipseDrawable : public Drawable
    {
    public:

        FilterPipeline<filters::VertexData, filters::VertexData> fragmentPipeline {};  ///< Filter pipeline for visual effects

        Vector2<f32> center { 0.f, 0.f };          ///< Center point of the ellipse
        Vector2<f32> radii { 10.f, 10.f };         ///< Horizontal and vertical radii
        Vector2<f32> uvTopLeft { 0.f, 0.f };       ///< UV coordinate for top-left corner
        Vector2<f32> uvBottomRight { 1.f, 1.f };   ///< UV coordinate for bottom-right corner

        /**
         * @brief Render the ellipse to the specified target
         * 
         * @details Draws a filled ellipse with automatic tessellation
         * for smooth curves. Uses the specified UV coordinates for
         * texture mapping across the ellipse surface.
         * 
         * @param renderer The renderer to use for drawing operations
         * @param target The render target to draw onto
         */
        void draw(Renderer &renderer, RenderTarget &target) override;
    };
}