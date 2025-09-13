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

    enum class DrawCallType : u8
    {
        Vertex,
        Line,
        Ellipse,
        TriangleMesh
    };

    namespace primitives
    {
        struct Vertex
        {
            Vector2<f32> position { 0.f, 0.f };
            Vector2<f32> uv { 0.f, 0.f };
        };

        struct Line
        {
            Vertex start;
            Vertex end;
        };

        struct Ellipse
        {
            Vector2<f32> center;
            Vector2<f32> radii;
            Vector2<f32> uvTopLeft { 0.f, 0.f };
            Vector2<f32> uvBottomRight { 1.f, 1.f };
        };

        struct TriangleMesh
        {
            u32 firstVertex = 0;
            u32 vertexCount = 0;
        };
    }

    using DrawPayload = std::variant<
        primitives::Vertex,
        primitives::Line,
        primitives::Ellipse,
        primitives::TriangleMesh
    >;

    struct DrawCallData
    {
        DrawCallType type;
        DrawPayload payload;
        Transform transform;
        BlendMode blendMode;
        FilterPipeline<filters::VertexData, filters::VertexData> *fragmentPipeline;
    };

    struct DrawCall
    {
        f32 depth = 0.f;
        u32 data_index = 0;
    };

    class RenderTarget
    {
    public:

        virtual ~RenderTarget() = default;

        void render();

        void setRenderer(Renderer *renderer);

        void fill(const Color &color);

    protected:

        const Vector2<u32> &getBufferSize() const;
        void setBufferSize(const Vector2<u32> &size);
        filters::BaseData &getBaseData();

    protected:

        FilterableBuffer<Color> m_pixelBuffer {};

    private:

        void setPixel(u32 index, const Color &color);
        void setPixel(const Vector2<u32> &position, const Color &color);
        void setPixelWithBlend(u32 index, const Color &color, BlendMode blendMode);
        void setPixelWithBlend(const Vector2<u32> &position, const Color &color, BlendMode blendMode);

        void sortDrawCalls();

        void registerDrawCall(const DrawCallData &drawCallData, f32 depth);
        void clearDrawCalls();

    private:

        Vector2<u32> m_bufferSize { 0u, 0u };

        std::vector<DrawCall> m_drawCalls {};
        std::vector<DrawCallData> m_drawCallDataPool {};

        filters::BaseData m_baseData;

        Renderer *m_renderer = nullptr;

    friend class Renderer;
    friend class Framework;
    };

    class Renderer
    {
    public:

        struct MeshAllocation {
            u32 firstVertex;
            std::span<primitives::Vertex> vertices;
        };

        void draw(RenderTarget &renderTarget, const primitives::Vertex &vertex, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, f32 depth = 0.f, BlendMode blendMode = BlendMode::Alpha);
        void draw(RenderTarget &renderTarget, const primitives::Line &line, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, f32 depth = 0.f, BlendMode blendMode = BlendMode::Alpha);
        void draw(RenderTarget &renderTarget, const primitives::Ellipse &ellipse, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, f32 depth = 0.f, BlendMode blendMode = BlendMode::Alpha);
        void draw(RenderTarget &renderTarget, const primitives::TriangleMesh &mesh, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, f32 depth = 0.f, BlendMode blendMode = BlendMode::Alpha);

        void drawImmediate(RenderTarget &renderTarget, const primitives::Vertex &vertex, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, BlendMode blendMode = BlendMode::Alpha);
        void drawImmediate(RenderTarget &renderTarget, const primitives::Line &line, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, BlendMode blendMode = BlendMode::Alpha);
        void drawImmediate(RenderTarget &renderTarget, const primitives::Ellipse &ellipse, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, BlendMode blendMode = BlendMode::Alpha);
        void drawImmediate(RenderTarget &renderTarget, const primitives::TriangleMesh &mesh, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, BlendMode blendMode = BlendMode::Alpha);

        void drawImmediatePixel(RenderTarget &renderTarget, const Vector2<u32> &position, const Color &color, BlendMode blendMode = BlendMode::Alpha);
        void drawImmediateLine(RenderTarget &renderTarget, const Vector2<u32> &start, const Vector2<u32> &end, const Color &color, BlendMode blendMode = BlendMode::Alpha);

        u32 addMesh(primitives::Vertex *vertices, u32 vertexCount);

        MeshAllocation allocateMesh(u32 vertexCount);

        void clearMeshes();

    private:

        bool clipLineToRect(Vector2<f32>& start, Vector2<f32>& end, const Vector2<u32>& rectSize) const;

        std::vector<primitives::Vertex> m_meshVertices {};

        FilterableBuffer<filters::VertexData> m_fragmentInputBuffer {};
        FilterableBuffer<filters::VertexData> m_fragmentOutputBuffer {};
    };
}

#endif // TEXTIL_RENDER_HPP