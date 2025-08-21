#ifndef CGL_DRAWABLE_HPP
#define CGL_DRAWABLE_HPP

#include "filters.hpp"
#include "filter_pipeline.hpp"
#include "transform.hpp"
#include <list>

#define ENABLE_DRAWABLE_CLONING(T) std::shared_ptr<Drawable> clone() const override { return std::make_shared<T>(*this); }

namespace cgl
{
    class Drawable : public Transformable
    {
    public:

        virtual ~Drawable() = default;

        template<typename T, typename... Args>
        static std::shared_ptr<T> create(Args&&... args);

    private:

        virtual void generateGeometry(std::vector<filters::GeometryElementData> &drawableBuffer, Transform &transform) = 0;

        virtual std::shared_ptr<Drawable> clone() const;

    public:

        BlendMode blendMode = BlendMode::Alpha;

        float depth = 0.f;

        FilterPipeline<filters::GeometryElementData, filters::GeometryElementData> fragmentPipeline;

        bool cloneOnDraw = false;

    friend class Framework;
    };

    namespace drawables
    {
        class Mesh : public Drawable
        {
        public:

            ENABLE_DRAWABLE_CLONING(Mesh)

            Mesh() = default;

            std::vector<Vector2<f32>> points;

            std::vector<Vector2<f32>> uvs;
        
        protected:

            static bool isTopEdge(const Vector2<f32>& v1, const Vector2<f32>& v2);

            static bool isLeftEdge(const Vector2<f32>& v1, const Vector2<f32>& v2);

            static bool isTopOrLeftEdge(const Vector2<f32>& v1, const Vector2<f32>& v2);

            void generateGeometry(std::vector<filters::GeometryElementData> &drawableBuffer, Transform &transform) override;
        };

        class Triangle : public Mesh
        {
        public:

            ENABLE_DRAWABLE_CLONING(Triangle)

            Triangle();
            
            Triangle(const Vector2<f32> &p1, const Vector2<f32> &p2, const Vector2<f32> &p3);

            void setPoint(u8 index, const Vector2<f32> &point);

            Vector2<f32> getPoint(u8 index) const;
        };

        class Rectangle : public Mesh
        {
        public:
    
            ENABLE_DRAWABLE_CLONING(Rectangle)

            Rectangle();

            Rectangle(const Vector2<f32> &size);

            void setSize(const Vector2<f32> &size);

            Vector2<f32> getSize() const;

        private:

            Vector2<f32> m_size;
        };

        class Point : public Drawable
        {
        public:

            ENABLE_DRAWABLE_CLONING(Point)

            Point() = default;

            Point(const Vector2<f32> &position) : position(position) {}

        public:

            Vector2<f32> position { 0.f, 0.f };

        private:

            void generateGeometry(std::vector<filters::GeometryElementData> &drawableBuffer, Transform &transform) override;
        };

        class Ellipse : public Drawable
        {
        public:

            ENABLE_DRAWABLE_CLONING(Ellipse)

            Ellipse() = default;

            Ellipse(const Vector2<f32> &center, const Vector2<f32> &radius) : center(center), radius(radius) {}

        public:

            Vector2<f32> center { 0.f, 0.f };
            Vector2<f32> radius { 1.f, 1.f };

        private:

            void generateGeometry(std::vector<filters::GeometryElementData> &drawableBuffer, Transform &transform) override;
        };

        class Line : public Drawable
        {
        public:

            ENABLE_DRAWABLE_CLONING(Line)

            Line() = default;

            Line(const Vector2<f32> &start, const Vector2<f32> &end) : start(start), end(end) {}

        public:

            Vector2<f32> start { 0.f, 0.f };
            Vector2<f32> end { 1.f, 1.f };

        private:

            void generateGeometry(std::vector<filters::GeometryElementData> &drawableBuffer, Transform &transform) override;
        };

        class Polygon : public Mesh
        {
        public:

            ENABLE_DRAWABLE_CLONING(Polygon)

            Polygon() = default;

            Polygon(const std::vector<Vector2<f32>> &points);

            void addPoint(const Vector2<f32> &point);

            void insertPoint(u32 index, const Vector2<f32> &point);

            void removePoint(u32 index);

            Vector2<f32> getPoint(u32 index) const;

            void setPoint(u32 index, const Vector2<f32> &point);

            const std::vector<Vector2<f32>>& getPoints() const;

        private:

            void generateGeometry(std::vector<filters::GeometryElementData> &drawableBuffer, Transform &transform) override;

            void triangulate();

            f32 calculateSignedArea();

        private:

            bool triangulated = false;
            std::vector<Vector2<f32>> nonTriangulatedPoints;
        };
    }

    template<typename T, typename... Args>
    std::shared_ptr<T> Drawable::create(Args&&... args) {
        static_assert(std::is_base_of_v<Drawable, T>, "T must derive from Drawable");
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}

#endif // CGL_DRAWABLE_HPP