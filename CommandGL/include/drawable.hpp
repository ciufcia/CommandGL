#ifndef CGL_DRAWABLE_HPP
#define CGL_DRAWABLE_HPP

#include "filters.hpp"
#include "transform.hpp"

namespace cgl
{
    /**
     * @class Drawable
     * @brief Abstract base class for all drawable objects in the graphics framework.
     * 
     * Drawable objects can be rendered to the screen and support transformation,
     * fragment shaders (filters), depth sorting, and blending modes.
     */
    class Drawable : public Transformable
    {
    public:

        virtual ~Drawable() = default;

        /**
         * @brief Factory method to create shared pointers to drawable objects.
         * @tparam T The type of drawable to create (must derive from Drawable).
         * @tparam Args Constructor argument types.
         * @param args Arguments to forward to the constructor of T.
         * @return A shared pointer to the created drawable object.
         */
        template<typename T, typename... Args>
        static std::shared_ptr<T> create(Args&&... args);

    private:

        /**
         * @brief Pure virtual method to generate geometry for rendering.
         * @param drawableBuffer Buffer to fill with pixel data for rendering.
         * 
         * This method must be implemented by derived classes to define
         * how the drawable's geometry is rasterized into pixels.
         */
        virtual void generateGeometry(std::vector<filter_pass_data::PixelPass> &drawableBuffer, Transform &transform) = 0;

        void applyFragmentPipeline(std::vector<filter_pass_data::PixelPass> &drawableBuffer, f32 time);

    public:

        /**
         * @brief The blending mode to use when compositing this drawable.
         */
        BlendMode blendMode = BlendMode::Alpha;

        /**
         * @brief Depth value for sorting (higher values are rendered earlier).
         */
        float depth = 0.f;

        /**
         * @brief Pipeline of fragment shaders (filters) to apply to pixels.
         */
        FilterPipeline fragmentPipeline;

    friend class Framework;
    };

    namespace drawables
    {
        /**
         * @class Mesh
         * @brief Base class for drawable objects defined by vertex geometry.
         * 
         * Mesh objects are defined by a collection of 2D points and UV coordinates,
         * and use triangle rasterization with a top-left fill rule.
         */
        class Mesh : public Drawable
        {
        public:

            Mesh() = default;

            /**
             * @brief Collection of 2D vertices defining the mesh geometry.
             * 
             * Points are grouped in sets of 3 to form triangles.
             */
            std::vector<Vector2<f32>> points;
            
            /**
             * @brief UV texture coordinates corresponding to each vertex.
             * 
             * Must have the same size as the points vector.
             */
            std::vector<Vector2<f32>> uvs;
        
        protected:

            /**
             * @brief Determines if an edge is a "top edge" for the top-left fill rule.
             * @param v1 First vertex of the edge.
             * @param v2 Second vertex of the edge.
             * @return True if this is a top edge (horizontal, left-to-right).
             */
            static bool isTopEdge(const Vector2<f32>& v1, const Vector2<f32>& v2);
            
            /**
             * @brief Determines if an edge is a "left edge" for the top-left fill rule.
             * @param v1 First vertex of the edge.
             * @param v2 Second vertex of the edge.
             * @return True if this is a left edge (going downward).
             */
            static bool isLeftEdge(const Vector2<f32>& v1, const Vector2<f32>& v2);
            
            /**
             * @brief Determines if an edge should be filled according to the top-left rule.
             * @param v1 First vertex of the edge.
             * @param v2 Second vertex of the edge.
             * @return True if pixels on this edge should be filled.
             */
            static bool isTopOrLeftEdge(const Vector2<f32>& v1, const Vector2<f32>& v2);

        private:

            void generateGeometry(std::vector<filter_pass_data::PixelPass> &drawableBuffer, Transform &transform) override;
        };

        /**
         * @class Triangle
         * @brief A drawable triangle defined by three vertices.
         * 
         * Triangles are automatically oriented to use counter-clockwise winding
         * for consistent rendering behavior.
         */
        class Triangle : public Mesh
        {
        public:

            Triangle();
            
            /**
             * @brief Constructor that creates a triangle from three points.
             * @param p1 First vertex of the triangle.
             * @param p2 Second vertex of the triangle.
             * @param p3 Third vertex of the triangle.
             * 
             * The vertices will be automatically reordered to ensure counter-clockwise winding.
             */
            Triangle(const Vector2<f32> &p1, const Vector2<f32> &p2, const Vector2<f32> &p3);

            /**
             * @brief Sets the position of a specific vertex.
             * @param index The vertex index (0, 1, or 2).
             * @param point The new position for the vertex.
             * @throws std::out_of_range if index is not 0, 1, or 2.
             * 
             * After setting a vertex, the triangle's winding order is automatically
             * corrected to maintain counter-clockwise orientation.
             */
            void setPoint(u8 index, const Vector2<f32> &point);
            
            /**
             * @brief Gets the position of a specific vertex.
             * @param index The vertex index (0, 1, or 2).
             * @return The position of the specified vertex.
             * @throws std::out_of_range if index is not 0, 1, or 2.
             */
            Vector2<f32> getPoint(u8 index) const;
        };

        /**
         * @class Rectangle
         * @brief A drawable rectangle composed of two triangles.
         * 
         * Rectangles are axis-aligned and defined by a top-left corner and size.
         * They are internally represented as two triangles with counter-clockwise winding.
         */
        class Rectangle : public Mesh
        {
        public:
        
            /**
             * @brief Default constructor creates a unit rectangle (1x1) at origin.
             */
            Rectangle();
            
            /**
             * @brief Constructor that creates a rectangle with specified position and size.
             * @param topLeft The top-left corner position.
             * @param size The width and height of the rectangle.
             */
            Rectangle(const Vector2<f32> &topLeft, const Vector2<f32> &size);

            /**
             * @brief Sets the top-left corner position while maintaining size.
             * @param topLeft The new top-left corner position.
             */
            void setTopLeft(const Vector2<f32> &topLeft);
            
            /**
             * @brief Gets the current top-left corner position.
             * @return The top-left corner position.
             */
            Vector2<f32> getTopLeft() const;

            /**
             * @brief Sets the bottom-right corner position, adjusting size accordingly.
             * @param bottomRight The new bottom-right corner position.
             */
            void setBottomRight(const Vector2<f32> &bottomRight);
            
            /**
             * @brief Gets the current bottom-right corner position.
             * @return The bottom-right corner position.
             */
            Vector2<f32> getBottomRight() const;

            /**
             * @brief Sets the rectangle size while maintaining top-left position.
             * @param size The new width and height.
             */
            void setSize(const Vector2<f32> &size);
            
            /**
             * @brief Gets the current rectangle size.
             * @return The width and height as a Vector2.
             */
            Vector2<f32> getSize() const;

        private:

            Vector2<f32> m_size;
        };

        class Point : public Drawable
        {
        public:

            Point() = default;

            /**
             * @brief Constructor that creates a point at a specific position.
             * @param position The position of the point in 2D space.
             */
            Point(const Vector2<f32> &position) : position(position) {}

        public:

            Vector2<f32> position { 0.f, 0.f };  ///< Position of the point in 2D space.

        private:

            void generateGeometry(std::vector<filter_pass_data::PixelPass> &drawableBuffer, Transform &transform) override;
        };
    }

    template<typename T, typename... Args>
    static std::shared_ptr<T> Drawable::create(Args&&... args) {
        static_assert(std::is_base_of_v<Drawable, T>, "T must derive from Drawable");
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}

#endif // CGL_DRAWABLE_HPP