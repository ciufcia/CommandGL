#include "drawables.hpp"
#include <algorithm>
#include <limits>

namespace til
{
    static inline f32 crossZ(const Vector2<f32>& a, const Vector2<f32>& b, const Vector2<f32>& c) {
        return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    }

    f32 Polygon::signedArea(const std::vector<Vector2<f32>> &pts) {
        if (pts.size() < 3) return 0.f;
        f32 area = 0.f;
        for (size_t i = 0, n = pts.size(), j = n - 1; i < n; j = i++) {
            area += (pts[j].x * pts[i].y) - (pts[i].x * pts[j].y);
        }
        return area * 0.5f;
    }

    bool Polygon::pointInTriangle(const Vector2<f32> &p, const Vector2<f32> &a, const Vector2<f32> &b, const Vector2<f32> &c) {
        f32 c1 = crossZ(a, b, p);
        f32 c2 = crossZ(b, c, p);
        f32 c3 = crossZ(c, a, p);
        bool hasNeg = (c1 < 0) || (c2 < 0) || (c3 < 0);
        bool hasPos = (c1 > 0) || (c2 > 0) || (c3 > 0);
        return !(hasNeg && hasPos);
    }

    const std::vector<Vector2<f32>>& Polygon::getPoints() const { return m_points; }

    void Polygon::setPoints(const std::vector<Vector2<f32>> &points) {
        m_points = points;
        m_meshDirty = true;
    }

    void Polygon::addPoint(const Vector2<f32> &pt) {
        m_points.push_back(pt);
        m_meshDirty = true;
    }

    void Polygon::insertPoint(u32 index, const Vector2<f32> &pt) {
        if (index > static_cast<u32>(m_points.size())) {
            invokeError<InvalidArgumentError>("Point index out of range");
        }
        m_points.insert(m_points.begin() + index, pt);
        m_meshDirty = true;
    }

    void Polygon::removePoint(u32 index) {
        if (index >= static_cast<u32>(m_points.size())) {
            invokeError<InvalidArgumentError>("Point index out of range");
        }
        m_points.erase(m_points.begin() + index);
        m_meshDirty = true;
    }

    void Polygon::clearPoints() {
        m_points.clear();
        m_meshDirty = true;
    }

    const std::vector<Vector2<f32>>& Polygon::getUVs() const { return m_uvs; }

    void Polygon::setUVs(const std::vector<Vector2<f32>> &uvs) {
        if (uvs.size() != m_points.size()) {
            invokeError<InvalidArgumentError>("UVs size must match points size");
        }
        m_uvs = uvs;
        m_hasCustomUVs = true;
        m_meshDirty = true;
    }

    void Polygon::setPointUV(u32 index, const Vector2<f32> &uv) {
        if (index >= static_cast<u32>(m_points.size())) {
            invokeError<InvalidArgumentError>("UV index out of range");
        }
        if (!m_hasCustomUVs) {
            m_uvs = std::vector<Vector2<f32>>(m_points.size());
            m_hasCustomUVs = true;
        }
        m_uvs[index] = uv;
        m_meshDirty = true;
    }

    void Polygon::clearUVs() {
        m_uvs.clear();
        m_hasCustomUVs = false;
        m_meshDirty = true;
    }

    void Polygon::rebuildMesh() {
        m_builtVertices.clear();
        const size_t n = m_points.size();
        if (n < 3) return;

        std::vector<Vector2<f32>> pts = m_points;
        std::vector<Vector2<f32>> uvsLocal;
        if (m_hasCustomUVs) {
            uvsLocal = m_uvs;
        }

        if (signedArea(pts) < 0.f) {
            std::reverse(pts.begin(), pts.end());
            if (m_hasCustomUVs) std::reverse(uvsLocal.begin(), uvsLocal.end());
        }

        Vector2<f32> minPt { std::numeric_limits<f32>::max(), std::numeric_limits<f32>::max() };
        Vector2<f32> maxPt { std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::lowest() };
        for (const auto &p : pts) {
            minPt.x = std::min(minPt.x, p.x); minPt.y = std::min(minPt.y, p.y);
            maxPt.x = std::max(maxPt.x, p.x); maxPt.y = std::max(maxPt.y, p.y);
        }
        Vector2<f32> uvSize = { std::max(1e-6f, maxPt.x - minPt.x), std::max(1e-6f, maxPt.y - minPt.y) };
        auto uvOf = [&](int idx, const Vector2<f32> &p) -> Vector2<f32> {
            if (m_hasCustomUVs) return uvsLocal[idx];
            return { (p.x - minPt.x) / uvSize.x, (p.y - minPt.y) / uvSize.y };
        };

        std::vector<int> V(n);
        for (int i = 0; i < static_cast<int>(n); ++i) V[i] = i;

        auto isEar = [&](int i0, int i1, int i2) -> bool {
            const Vector2<f32> &a = pts[V[i0]];
            const Vector2<f32> &b = pts[V[i1]];
            const Vector2<f32> &c = pts[V[i2]];

            if (crossZ(a, b, c) <= 0.f) return false;
            for (int j = 0; j < static_cast<int>(V.size()); ++j) {
                if (j == i0 || j == i1 || j == i2) continue;
                if (pointInTriangle(pts[V[j]], a, b, c)) return false;
            }
            return true;
        };

        int guard = static_cast<int>(n) * static_cast<int>(n);
        int i = 0;
        while (V.size() > 3 && guard-- > 0) {
            int i0 = (i + static_cast<int>(V.size()) - 1) % static_cast<int>(V.size());
            int i1 = i;
            int i2 = (i + 1) % static_cast<int>(V.size());

            if (isEar(i0, i1, i2)) {
                const int ia = V[i0];
                const int ib = V[i1];
                const int ic = V[i2];
                const Vector2<f32> &a = pts[ia];
                const Vector2<f32> &b = pts[ib];
                const Vector2<f32> &c = pts[ic];

                m_builtVertices.push_back({ a, uvOf(ia, a) });
                m_builtVertices.push_back({ b, uvOf(ib, b) });
                m_builtVertices.push_back({ c, uvOf(ic, c) });

                V.erase(V.begin() + i1);
                i = 0;
            } else {
                i = (i + 1) % static_cast<int>(V.size());
            }
        }

        if (V.size() == 3) {
            const int ia = V[0];
            const int ib = V[1];
            const int ic = V[2];
            const Vector2<f32> &a = pts[ia];
            const Vector2<f32> &b = pts[ib];
            const Vector2<f32> &c = pts[ic];
            if (crossZ(a, b, c) < 0.f) {
                m_builtVertices.push_back({ a, uvOf(ia, a) });
                m_builtVertices.push_back({ c, uvOf(ic, c) });
                m_builtVertices.push_back({ b, uvOf(ib, b) });
            } else {
                m_builtVertices.push_back({ a, uvOf(ia, a) });
                m_builtVertices.push_back({ b, uvOf(ib, b) });
                m_builtVertices.push_back({ c, uvOf(ic, c) });
            }
        }
    }

    void Polygon::draw(Renderer &renderer, RenderTarget &target) {
        if (m_meshDirty) {
            rebuildMesh();
            m_meshDirty = false;
        }

        if (m_builtVertices.empty()) return;
        auto alloc = renderer.allocateMesh(static_cast<u32>(m_builtVertices.size()));
        auto out = alloc.vertices;
        std::copy(m_builtVertices.begin(), m_builtVertices.end(), out.begin());

        primitives::TriangleMesh mesh;
        mesh.firstVertex = alloc.firstVertex;
        mesh.vertexCount = static_cast<u32>(m_builtVertices.size());

        renderer.draw(target, mesh, transform, fragmentPipeline);
    }
    
    void Point::draw(Renderer &renderer, RenderTarget &target) {
        primitives::Vertex v{ position, uv };
        renderer.draw(target, v, transform, fragmentPipeline);
    }
    
    void LineDrawable::draw(Renderer &renderer, RenderTarget &target) {
        primitives::Line l{ { start, uvStart }, { end, uvEnd } };
        renderer.draw(target, l, transform, fragmentPipeline);
    }
    
    void EllipseDrawable::draw(Renderer &renderer, RenderTarget &target) {
        primitives::Ellipse e{ center, radii, uvTopLeft, uvBottomRight };
        renderer.draw(target, e, transform, fragmentPipeline);
    }

    void Rectangle::draw(Renderer &renderer, RenderTarget &target) {
        auto alloc = renderer.allocateMesh(6);
        auto &v = alloc.vertices;
        v[0] = { { topLeft.x,         topLeft.y          }, { 0.f, 0.f } };
        v[1] = { { topLeft.x+size.x,  topLeft.y          }, { 1.f, 0.f } };
        v[2] = { { topLeft.x+size.x,  topLeft.y+size.y   }, { 1.f, 1.f } };
        v[3] = { { topLeft.x,         topLeft.y          }, { 0.f, 0.f } };
        v[4] = { { topLeft.x+size.x,  topLeft.y+size.y   }, { 1.f, 1.f } };
        v[5] = { { topLeft.x,         topLeft.y+size.y   }, { 0.f, 1.f } };

        primitives::TriangleMesh mesh;
        mesh.firstVertex = alloc.firstVertex;
        mesh.vertexCount = 6;

        renderer.draw(target, mesh, transform, fragmentPipeline);
    }

    Sprite::Sprite() : m_texture(nullptr) {
        m_textureSampler.data.texture = m_texture;
        m_fragmentPipeline.addFilter(&m_textureSampler);
    }

    Sprite::Sprite(Texture *texture) : m_texture(texture) {
        if (!m_texture) {
            invokeError<InvalidArgumentError>("Texture pointer cannot be null");
        }

        m_textureSampler.data.texture = m_texture;
        m_fragmentPipeline.addFilter(&m_textureSampler);
    }

    void Sprite::setTexture(Texture *texture) {
        if (!texture) {
            invokeError<InvalidArgumentError>("Texture pointer cannot be null");
        }

        m_texture = texture;
        m_textureSampler.data.texture = m_texture;
    }

    Texture* Sprite::getTexture() const {
        return m_texture;
    }

    void Sprite::draw(Renderer &renderer, RenderTarget &target) {
        if (!m_texture) {
            invokeError<LogicError>("Cannot draw sprite without a texture");
            return;
        }

        if (m_texture->getSize().x == 0 || m_texture->getSize().y == 0) {
            invokeError<LogicError>("Cannot draw sprite with zero-sized texture");
            return;
        }

        auto alloc = renderer.allocateMesh(6);
        auto &v = alloc.vertices;
        v[0] = { { 0.f,     0.f      }, { 0.f, 0.f } };
        v[1] = { { size.x,  0.f      }, { 1.f, 0.f } };
        v[2] = { { size.x,  size.y   }, { 1.f, 1.f } };
        v[3] = { { 0.f,     0.f      }, { 0.f, 0.f } };
        v[4] = { { size.x,  size.y   }, { 1.f, 1.f } };
        v[5] = { { 0.f,     size.y   }, { 0.f, 1.f } };

        primitives::TriangleMesh mesh;
        mesh.firstVertex = alloc.firstVertex;
        mesh.vertexCount = 6;

        renderer.draw(target, mesh, transform, m_fragmentPipeline);
    }

    void Sprite::addFilter(BaseFilter *filter) {
        m_fragmentPipeline.addFilter(filter);
    }

    void Sprite::insertFilter(u32 index, BaseFilter *filter) {
        u32 realIndex = index + 1;
        m_fragmentPipeline.insertFilter(realIndex, filter);
    }

    void Sprite::removeFilter(u32 index) {
        u32 realIndex = index + 1;
        m_fragmentPipeline.removeFilter(realIndex);
    }

    void Sprite::clearFilters() {
        m_fragmentPipeline.clearFilters();
        m_fragmentPipeline.addFilter(&m_textureSampler);
    }
}