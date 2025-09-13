#include "render.hpp"
#include "texture.hpp"
#include "transform.hpp"

namespace til
{
    class Drawable
    {
    public:

        virtual ~Drawable() = default;

        virtual void draw(Renderer &renderer, RenderTarget &target) = 0;
    };

    class Sprite : public Drawable
    {
    public:

        Transform transform;

        Vector2<f32> size { 10.f, 10.f };

    public:

        Sprite();
        Sprite(Texture *texture);

        void setTexture(Texture *texture);
        Texture* getTexture() const;

        void draw(Renderer &renderer, RenderTarget &target) override;

        void addFilter(BaseFilter *filter);
        void insertFilter(u32 index, BaseFilter *filter);
        void removeFilter(u32 index);
        void clearFilters();

    private:

        Texture *m_texture = nullptr;

        FilterPipeline<filters::VertexData, filters::VertexData> m_fragmentPipeline {};
        filters::TextureSampler m_textureSampler { nullptr };

    };

    class Rectangle : public Drawable
    {
    public:

        Transform transform;

        Vector2<f32> topLeft { 0.f, 0.f };
        Vector2<f32> size { 10.f, 10.f };

        FilterPipeline<filters::VertexData, filters::VertexData> fragmentPipeline {};

    public:

        Rectangle() = default;

        void draw(Renderer &renderer, RenderTarget &target) override;
    };

    class Polygon : public Drawable
    {
    public:

        Transform transform;

        FilterPipeline<filters::VertexData, filters::VertexData> fragmentPipeline {};

        const std::vector<Vector2<f32>>& getPoints() const;
        void setPoints(const std::vector<Vector2<f32>> &points);
        void addPoint(const Vector2<f32> &pt);
        void insertPoint(u32 index, const Vector2<f32> &pt);
        void removePoint(u32 index);
        void clearPoints();

        const std::vector<Vector2<f32>>& getUVs() const;
        void setUVs(const std::vector<Vector2<f32>> &uvs);
        void setPointUV(u32 index, const Vector2<f32> &uv);
        void clearUVs();

        void draw(Renderer &renderer, RenderTarget &target) override;

    private:
        void rebuildMesh();
        static f32 signedArea(const std::vector<Vector2<f32>> &pts);
        static bool pointInTriangle(const Vector2<f32> &p, const Vector2<f32> &a, const Vector2<f32> &b, const Vector2<f32> &c);

    private:
    
        std::vector<Vector2<f32>> m_points;
        std::vector<Vector2<f32>> m_uvs;
        bool m_hasCustomUVs = false;

        std::vector<primitives::Vertex> m_builtVertices;
        bool m_meshDirty = true;
    };

    class Point : public Drawable
    {
    public:
    
        Transform transform;
        FilterPipeline<filters::VertexData, filters::VertexData> fragmentPipeline {};

        Vector2<f32> position { 0.f, 0.f };
        Vector2<f32> uv { 0.f, 0.f };

        void draw(Renderer &renderer, RenderTarget &target) override;
    };

    class LineDrawable : public Drawable
    {
    public:

        Transform transform;
        FilterPipeline<filters::VertexData, filters::VertexData> fragmentPipeline {};

        Vector2<f32> start { 0.f, 0.f };
        Vector2<f32> end { 1.f, 1.f };
        Vector2<f32> uvStart { 0.f, 0.f };
        Vector2<f32> uvEnd { 1.f, 0.f };

        void draw(Renderer &renderer, RenderTarget &target) override;
    };

    class EllipseDrawable : public Drawable
    {
    public:

        Transform transform;
        FilterPipeline<filters::VertexData, filters::VertexData> fragmentPipeline {};

        Vector2<f32> center { 0.f, 0.f };
        Vector2<f32> radii { 10.f, 10.f };
        Vector2<f32> uvTopLeft { 0.f, 0.f };
        Vector2<f32> uvBottomRight { 1.f, 1.f };

        void draw(Renderer &renderer, RenderTarget &target) override;
    };
}