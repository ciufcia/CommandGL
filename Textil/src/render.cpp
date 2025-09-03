#include "til.hpp"

namespace til
{
    void RenderTarget::render() {
        if (!m_renderer) {
            invokeError<LogicError>("No renderer set");
        }

        sortDrawCalls();

        for (const auto &drawCall : m_drawCalls) {
            DrawCallData &data = m_drawCallDataPool[drawCall.data_index];
            switch (data.type) {
                case DrawCallType::Vertex:
                    m_renderer->drawImmediate(*this, std::get<primitives::Vertex>(data.payload), data.transform, *data.fragmentPipeline);
                    break;
                case DrawCallType::Line:
                    m_renderer->drawImmediate(*this, std::get<primitives::Line>(data.payload), data.transform, *data.fragmentPipeline);
                    break;
                case DrawCallType::Ellipse:
                    m_renderer->drawImmediate(*this, std::get<primitives::Ellipse>(data.payload), data.transform, *data.fragmentPipeline);
                    break;
                case DrawCallType::TriangleMesh:
                    m_renderer->drawImmediate(*this, std::get<primitives::TriangleMesh>(data.payload), data.transform, *data.fragmentPipeline);
                    break;
                default:
                    invokeError<LogicError>("Unknown draw call type");
            }
        }
        
        clearDrawCalls();
    }

    void RenderTarget::fill(const Color &color) {
        i32 size = m_bufferSize.x * m_bufferSize.y;

        #pragma omp parallel for
        for (i32 i = 0; i < size; ++i) {
            m_pixelBuffer[i] = color;
        }
    }

    void RenderTarget::registerDrawCall(const DrawCallData &drawCallData, f32 depth) {
        m_drawCallDataPool.push_back(drawCallData);
        m_drawCalls.push_back({ depth, static_cast<u32>(m_drawCallDataPool.size() - 1) });
    }

    void RenderTarget::clearDrawCalls() {
        m_drawCalls.clear();
        m_drawCallDataPool.clear();
    }

    const Vector2<u32> &RenderTarget::getBufferSize() const {
        return m_bufferSize;
    }

    void RenderTarget::setBufferSize(const Vector2<u32> &size) {
        m_bufferSize = size;
        m_pixelBuffer.getBuffer().resize(size.x * size.y);
    }

    void RenderTarget::setPixel(u32 index, const Color &color) {
        m_pixelBuffer[index] = color;
    }

    void RenderTarget::setPixel(const Vector2<u32> &position, const Color &color) {
        u32 index = position.y * m_bufferSize.x + position.x;
        setPixel(index, color);
    }

    void RenderTarget::setPixelWithBlend(u32 index, const Color &color, BlendMode blendMode) {
        Color destinationColor = m_pixelBuffer[index];

        m_pixelBuffer[index] = Color::applyBlend(destinationColor, color, blendMode);
    }

    void RenderTarget::setPixelWithBlend(const Vector2<u32> &position, const Color &color, BlendMode blendMode) {
        u32 index = position.y * m_bufferSize.x + position.x;
        setPixelWithBlend(index, color, blendMode);
    }

    filters::BaseData &RenderTarget::getBaseData() {
        return m_baseData;
    }

    void RenderTarget::setRenderer(Renderer *renderer) {
        m_renderer = renderer;
    }

    void RenderTarget::sortDrawCalls() {
        std::sort(m_drawCalls.begin(), m_drawCalls.end(), [](const DrawCall &a, const DrawCall &b) {
            return a.depth > b.depth;
        });
    }

    void Renderer::drawImmediatePixel(RenderTarget &renderTarget, const Vector2<u32> &position, const Color &color, BlendMode blendMode) {
        if (position.x > renderTarget.getBufferSize().x || position.y > renderTarget.getBufferSize().y) {
            return;
        }
        renderTarget.setPixelWithBlend(position, color, blendMode);
    }

    void Renderer::drawImmediateLine(RenderTarget &renderTarget, const Vector2<u32> &start, const Vector2<u32> &end, const Color &color, BlendMode blendMode) {
        const Vector2<u32> size = renderTarget.getBufferSize();
        if (start.x >= size.x || start.y >= size.y || end.x >= size.x || end.y >= size.y) {
            return;
        }

        i32 x0 = static_cast<i32>(start.x);
        i32 y0 = static_cast<i32>(start.y);
        i32 x1 = static_cast<i32>(end.x);
        i32 y1 = static_cast<i32>(end.y);

        bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);
        if (steep) {
            std::swap(x0, y0);
            std::swap(x1, y1);
        }

        if (x0 > x1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }

        i32 dx = x1 - x0;
        i32 dy = std::abs(y1 - y0);
        i32 err = dx / 2;
        i32 ystep = (y0 < y1) ? 1 : -1;
        i32 y = y0;

        i32 width = static_cast<i32>(size.x);
        i32 height = static_cast<i32>(size.y);

        for (i32 x = x0; x <= x1; ++x) {
            i32 plotx = steep ? y : x;
            i32 ploty = steep ? x : y;

            if (plotx >= 0 && plotx < width && ploty >= 0 && ploty < height) {
                renderTarget.setPixelWithBlend({ static_cast<u32>(plotx), static_cast<u32>(ploty) }, color, blendMode);
            }

            err -= dy;
            if (err < 0) {
                y += ystep;
                err += dx;
            }
        }
    }

    void Renderer::drawImmediate(RenderTarget &renderTarget, const primitives::Vertex &vertex, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, BlendMode blendMode) {
        Vector2<f32> transformedPosition = transform.getMatrix() * vertex.position;

        if (
            transformedPosition.x < 0.f ||
            transformedPosition.y < 0.f ||
            transformedPosition.x >= static_cast<f32>(renderTarget.getBufferSize().x) ||
            transformedPosition.y >= static_cast<f32>(renderTarget.getBufferSize().y)
        ) {
            return;
        }

        filters::VertexData vertexData;
        vertexData.position = transformedPosition;
        vertexData.uv = vertex.uv;
        vertexData.size = { 1.f, 1.f };
        vertexData.inverseSize = { 1.f, 1.f };

        m_fragmentInputBuffer.getBuffer().resize(1);
        m_fragmentOutputBuffer.getBuffer().resize(1);

        m_fragmentInputBuffer[0] = vertexData;

        fragmentPipeline.run(&m_fragmentInputBuffer, &m_fragmentOutputBuffer, renderTarget.getBaseData());

        renderTarget.setPixelWithBlend({ static_cast<u32>(transformedPosition.x), static_cast<u32>(transformedPosition.y) }, m_fragmentOutputBuffer[0].color, blendMode);
    }

    void Renderer::drawImmediate(RenderTarget &renderTarget, const primitives::Line &line, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, BlendMode blendMode) {
        Matrix3<f32> transformMatrix = transform.getMatrix();

        Vector2<f32> startTransformed = transformMatrix * line.start.position;
        Vector2<f32> endTransformed = transformMatrix * line.end.position;

        if (!clipLineToRect(startTransformed, endTransformed, renderTarget.getBufferSize())) {
            return;
        }

        Vector2<f32> difference = endTransformed - startTransformed;
        Vector2<f32> inverseDifference = { 1.f / difference.x, 1.f / difference.y };
        float length = difference.magnitude();

        filters::VertexData pixelData;

        m_fragmentInputBuffer.clear();

        if (length < 1e-6f) {
            pixelData.position = startTransformed;
            pixelData.uv = { 0.f, 0.f };
            pixelData.size = { 1.f, 1.f };
            pixelData.inverseSize = { 1.f, 1.f };

            m_fragmentInputBuffer.getBuffer().push_back(pixelData);
        } else {
            u32 steps = static_cast<u32>(std::ceil(length));
            f32 inverseSteps = 1.f / static_cast<f32>(steps);
            Vector2<f32> step = difference / static_cast<f32>(steps);

            for (u32 i = 0; i <= steps; ++i) {
                Vector2<f32> position = startTransformed + step * static_cast<f32>(i);

                pixelData.position = position;
                pixelData.uv = { static_cast<f32>(i) * inverseSteps, 0.f };
                pixelData.size = difference;
                pixelData.inverseSize = inverseDifference;

                m_fragmentInputBuffer.getBuffer().push_back(pixelData);
            }
        }

        m_fragmentOutputBuffer.setSize(m_fragmentInputBuffer.getSize());

        fragmentPipeline.run(&m_fragmentInputBuffer, &m_fragmentOutputBuffer, renderTarget.getBaseData());

        for (i32 i = 0; i < m_fragmentOutputBuffer.getSize(); ++i) {
            renderTarget.setPixelWithBlend({ static_cast<u32>(m_fragmentInputBuffer[i].position.x), static_cast<u32>(m_fragmentInputBuffer[i].position.y) }, m_fragmentOutputBuffer[i].color, blendMode);
        }
    }

    void Renderer::drawImmediate(RenderTarget &renderTarget, const primitives::Ellipse &ellipse, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, BlendMode blendMode) {
        Matrix3<f32> transformMatrix = transform.getMatrix();
        Matrix3<f32> inverseMatrix = transformMatrix.inverse();

        Vector2<f32> corners[4] = {
            { ellipse.center.x - ellipse.radii.x, ellipse.center.y - ellipse.radii.y },
            { ellipse.center.x + ellipse.radii.x, ellipse.center.y - ellipse.radii.y },
            { ellipse.center.x + ellipse.radii.x, ellipse.center.y + ellipse.radii.y },
            { ellipse.center.x - ellipse.radii.x, ellipse.center.y + ellipse.radii.y }
        };

        Vector2<f32> topLeft = transformMatrix * corners[0];
        Vector2<f32> bottomRight = topLeft;

        for (u32 i = 1; i < 4; ++i) {
            Vector2<f32> transformedCorner = transformMatrix * corners[i];

            topLeft.x = std::min(topLeft.x, transformedCorner.x);
            topLeft.y = std::min(topLeft.y, transformedCorner.y);
            bottomRight.x = std::max(bottomRight.x, transformedCorner.x);
            bottomRight.y = std::max(bottomRight.y, transformedCorner.y);
        }

        Vector2<f32> size = bottomRight - topLeft;
        Vector2<f32> inverseSize = { 1.f / size.x, 1.f / size.y };
        Vector2<f32> inverseDiameter = { 1.f / (2.f * ellipse.radii.x), 1.f / (2.f * ellipse.radii.y) };

        Vector2<u32> renderTargetSize = renderTarget.getBufferSize();
        
        i32 clippedLeft = std::max(static_cast<i32>(topLeft.x), 0);
        i32 clippedTop = std::max(static_cast<i32>(topLeft.y), 0);
        i32 clippedRight = std::min(static_cast<i32>(bottomRight.x), static_cast<i32>(renderTargetSize.x - 1));
        i32 clippedBottom = std::min(static_cast<i32>(bottomRight.y), static_cast<i32>(renderTargetSize.y - 1));

        if (clippedLeft > clippedRight || clippedTop > clippedBottom) {
            return;
        }

        m_fragmentInputBuffer.clear();

        #pragma omp parallel
        {
            std::vector<filters::VertexData> localBuffer;
            localBuffer.reserve(256);

            #pragma omp for collapse(2) schedule(dynamic)
            for (int y = clippedTop; y <= clippedBottom; ++y) {
                for (int x = clippedLeft; x <= clippedRight; ++x) {
                    Vector2<f32> pixelPos = { static_cast<f32>(x), static_cast<f32>(y) };
                    Vector2<f32> localPos = inverseMatrix * pixelPos;

                    f32 dx = localPos.x - ellipse.center.x;
                    f32 dy = localPos.y - ellipse.center.y;
                    f32 distanceSquared = (dx * dx) / (ellipse.radii.x * ellipse.radii.x) + (dy * dy) / (ellipse.radii.y * ellipse.radii.y);

                    if (distanceSquared <= 1.f) {
                        filters::VertexData pixelData;
                        
                        pixelData.position = pixelPos;
                        pixelData.uv = {
                            (localPos.x - ellipse.center.x) * inverseDiameter.x + 0.5f,
                            (localPos.y - ellipse.center.y) * inverseDiameter.y + 0.5f
                        };
                        pixelData.size = size;
                        pixelData.inverseSize = inverseSize;

                        localBuffer.push_back(pixelData);
                    }
                }
            }

            #pragma omp critical
            m_fragmentInputBuffer.getBuffer().insert(m_fragmentInputBuffer.getBuffer().end(), localBuffer.begin(), localBuffer.end());
        }

        m_fragmentOutputBuffer.setSize(m_fragmentInputBuffer.getSize());

        fragmentPipeline.run(&m_fragmentInputBuffer, &m_fragmentOutputBuffer, renderTarget.getBaseData());

        for (i32 i = 0; i < m_fragmentOutputBuffer.getSize(); ++i) {
            renderTarget.setPixelWithBlend({ static_cast<u32>(m_fragmentInputBuffer[i].position.x), static_cast<u32>(m_fragmentInputBuffer[i].position.y) }, m_fragmentOutputBuffer[i].color, blendMode);
        }
    }

    void Renderer::drawImmediate(RenderTarget &renderTarget, const primitives::TriangleMesh &mesh, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, BlendMode blendMode) {
        u32 meshStart = mesh.firstVertex;
        u32 meshEnd = mesh.firstVertex + mesh.vertexCount;

        if (meshEnd > m_meshVertices.size()) {
            invokeError<InvalidArgumentError>("Renderer couldn't find the specified mesh vertices");
        }

        if (mesh.vertexCount % 3 != 0) {
            invokeError<InvalidArgumentError>("Not enough vertices in mesh. Must be a multiple of 3.");
        }

        i32 triangleCount = static_cast<i32>(mesh.vertexCount / 3);

        if (triangleCount <= 0) {
            invokeError<InvalidArgumentError>("Not enough vertices to form a mesh.");
        }

        Matrix3<f32> transformMatrix = transform.getMatrix();

        #pragma omp parallel for
        for (u32 i = meshStart; i < meshEnd; ++i) {
            m_meshVertices[i].position = transformMatrix * m_meshVertices[i].position;
        }

        struct TriangleParams {
            Vector2<f32> uv1, uv2, uv3;
            Vector2<f32> topLeft, bottomRight;
            Vector2<f32> size, inverseSize;
            f32 inverseArea;

            f32 e1a, e1b, e1c;
            f32 e2a, e2b, e2c;
            f32 e3a, e3b, e3c;
            
            bool e1_topLeft, e2_topLeft, e3_topLeft;
        };

        auto isTopOrLeftEdge = [](const Vector2<f32> &p1, const Vector2<f32> &p2) {
            return (p1.y == p2.y) ? (p1.x < p2.x) : (p1.y > p2.y);
        };

        std::vector<TriangleParams> triangles(triangleCount);
        
        #pragma omp parallel for
        for (int i = 0; i < triangleCount; ++i) {
            auto p1 = m_meshVertices[meshStart + i * 3].position;
            auto p2 = m_meshVertices[meshStart + i * 3 + 1].position;
            auto p3 = m_meshVertices[meshStart + i * 3 + 2].position;

            auto &triangle = triangles[i];

            triangle.uv1 = m_meshVertices[meshStart + i * 3].uv;
            triangle.uv2 = m_meshVertices[meshStart + i * 3 + 1].uv;
            triangle.uv3 = m_meshVertices[meshStart + i * 3 + 2].uv;

            triangle.topLeft = { std::min({ p1.x, p2.x, p3.x }), std::min({ p1.y, p2.y, p3.y }) };
            triangle.bottomRight = { std::max({ p1.x, p2.x, p3.x }), std::max({ p1.y, p2.y, p3.y }) };

            triangle.size = triangle.bottomRight - triangle.topLeft;
            triangle.inverseSize = { 1.f / triangle.size.x, 1.f / triangle.size.y };

            triangle.e1a = p1.y - p2.y; triangle.e1b = p2.x - p1.x; triangle.e1c = p1.x * p2.y - p2.x * p1.y;
            triangle.e2a = p2.y - p3.y; triangle.e2b = p3.x - p2.x; triangle.e2c = p2.x * p3.y - p3.x * p2.y;
            triangle.e3a = p3.y - p1.y; triangle.e3b = p1.x - p3.x; triangle.e3c = p3.x * p1.y - p1.x * p3.y;
            
            triangle.e1_topLeft = isTopOrLeftEdge(p1, p2);
            triangle.e2_topLeft = isTopOrLeftEdge(p2, p3);
            triangle.e3_topLeft = isTopOrLeftEdge(p3, p1);

            f32 area2 = (p2.x - p1.x) * (p3.y - p1.y) - (p3.x - p1.x) * (p2.y - p1.y);
            triangle.inverseArea = (std::abs(area2) > 1e-6f) ? 1.f / area2 : 0.f;
        }

        m_fragmentInputBuffer.clear();

        Vector2<u32> renderTargetSize = renderTarget.getBufferSize();
        std::vector<filters::VertexData> triangleBuffer;

        for (int ti = 0; ti < triangleCount; ++ti) {
            const auto &triangle = triangles[ti];

            i32 clippedLeft = std::max(static_cast<i32>(std::floor(triangle.topLeft.x)), 0);
            i32 clippedTop = std::max(static_cast<i32>(std::floor(triangle.topLeft.y)), 0);
            i32 clippedRight = std::min(static_cast<i32>(std::ceil(triangle.bottomRight.x)), static_cast<i32>(renderTargetSize.x) - 1);
            i32 clippedBottom = std::min(static_cast<i32>(std::ceil(triangle.bottomRight.y)), static_cast<i32>(renderTargetSize.y) - 1);

            if (clippedLeft > clippedRight || clippedTop > clippedBottom) {
                continue;
            }

            Vector2<i32> size = {
                clippedRight - clippedLeft + 1,
                clippedBottom - clippedTop + 1
            };

            triangleBuffer.resize(size.x * size.y);

            #pragma omp parallel for collapse(2) schedule(dynamic)
            for (int y = clippedTop; y <= clippedBottom; ++y)
            for (int x = clippedLeft; x <= clippedRight; ++x) {
                f32 e1 = triangle.e1a * x + triangle.e1b * y + triangle.e1c;
                f32 e2 = triangle.e2a * x + triangle.e2b * y + triangle.e2c;
                f32 e3 = triangle.e3a * x + triangle.e3b * y + triangle.e3c;
                
                bool inside = (
                    (e1 > 0 || (e1 == 0 && triangle.e1_topLeft)) &&
                    (e2 > 0 || (e2 == 0 && triangle.e2_topLeft)) &&
                    (e3 > 0 || (e3 == 0 && triangle.e3_topLeft))
                );
                
                int index = (y - clippedTop) * size.x + (x - clippedLeft);

                filters::VertexData &pixelData = triangleBuffer[index];

                if (inside) {
                    f32 w1 = e2 * triangle.inverseArea;
                    f32 w2 = e3 * triangle.inverseArea;
                    f32 w3 = e1 * triangle.inverseArea;

                    pixelData.position = { static_cast<f32>(x), static_cast<f32>(y) };
                    pixelData.uv = { triangle.uv1.x * w1 + triangle.uv2.x * w2 + triangle.uv3.x * w3,
                                    triangle.uv1.y * w1 + triangle.uv2.y * w2 + triangle.uv3.y * w3 };
                    pixelData.size = triangle.size;
                    pixelData.inverseSize = triangle.inverseSize;
                } else {
                    pixelData.size = { -1.f, -1.f };
                }
            }

            for (const auto &pixelData : triangleBuffer) {
                if (pixelData.size.x >= 0.f && pixelData.size.y >= 0.f) {
                    m_fragmentInputBuffer.getBuffer().push_back(pixelData);
                }
            }

            triangleBuffer.clear();
        }

        m_fragmentOutputBuffer.setSize(m_fragmentInputBuffer.getSize());

        fragmentPipeline.run(&m_fragmentInputBuffer, &m_fragmentOutputBuffer, renderTarget.getBaseData());

        for (i32 i = 0; i < m_fragmentOutputBuffer.getSize(); ++i) {
            renderTarget.setPixelWithBlend({ static_cast<u32>(m_fragmentInputBuffer[i].position.x), static_cast<u32>(m_fragmentInputBuffer[i].position.y) }, m_fragmentOutputBuffer[i].color, blendMode);
        }
    }

    void Renderer::draw(RenderTarget &renderTarget, const primitives::Vertex &vertex, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, f32 depth, BlendMode blendMode) {
        DrawCallData drawCallData;
        drawCallData.type = DrawCallType::Vertex;
        drawCallData.payload = vertex;
        drawCallData.transform = transform;
        drawCallData.fragmentPipeline = &fragmentPipeline;
        drawCallData.blendMode = blendMode;

        renderTarget.registerDrawCall(drawCallData, depth);
    }

    void Renderer::draw(RenderTarget &renderTarget, const primitives::Line &line, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, f32 depth, BlendMode blendMode) {
        DrawCallData drawCallData;
        drawCallData.type = DrawCallType::Line;
        drawCallData.payload = line;
        drawCallData.transform = transform;
        drawCallData.fragmentPipeline = &fragmentPipeline;
        drawCallData.blendMode = blendMode;

        renderTarget.registerDrawCall(drawCallData, depth);
    }

    void Renderer::draw(RenderTarget &renderTarget, const primitives::Ellipse &ellipse, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, f32 depth, BlendMode blendMode) {
        DrawCallData drawCallData;
        drawCallData.type = DrawCallType::Ellipse;
        drawCallData.payload = ellipse;
        drawCallData.transform = transform;
        drawCallData.fragmentPipeline = &fragmentPipeline;
        drawCallData.blendMode = blendMode;

        renderTarget.registerDrawCall(drawCallData, depth);
    }

    void Renderer::draw(RenderTarget &renderTarget, const primitives::TriangleMesh &mesh, const Transform &transform, FilterPipeline<filters::VertexData, filters::VertexData> &fragmentPipeline, f32 depth, BlendMode blendMode) {
        DrawCallData drawCallData;
        drawCallData.type = DrawCallType::TriangleMesh;
        drawCallData.payload = mesh;
        drawCallData.transform = transform;
        drawCallData.fragmentPipeline = &fragmentPipeline;
        drawCallData.blendMode = blendMode;

        renderTarget.registerDrawCall(drawCallData, depth);
    }

    bool Renderer::clipLineToRect(Vector2<f32>& start, Vector2<f32>& end, const Vector2<u32>& rectSize) const {
        const f32 minX = 0.0f;
        const f32 minY = 0.0f;
        const f32 maxX = static_cast<f32>(rectSize.x - 1);
        const f32 maxY = static_cast<f32>(rectSize.y - 1);

        auto computeOutCode = [&](const Vector2<f32>& point) -> u8 {
            u8 code = 0;
            if (point.x < minX) code |= 1;
            if (point.x > maxX) code |= 2;
            if (point.y < minY) code |= 4;
            if (point.y > maxY) code |= 8;
            return code;
        };

        u8 outcode0 = computeOutCode(start);
        u8 outcode1 = computeOutCode(end);

        while (true) {
            if (!(outcode0 | outcode1)) {
                return true;
            } else if (outcode0 & outcode1) {
                return false;
            } else {
                u8 outcodeOut = outcode0 ? outcode0 : outcode1;
                Vector2<f32> intersection;

                if (outcodeOut & 8) { // Top
                    intersection.x = start.x + (end.x - start.x) * (maxY - start.y) / (end.y - start.y);
                    intersection.y = maxY;
                } else if (outcodeOut & 4) { // Bottom
                    intersection.x = start.x + (end.x - start.x) * (minY - start.y) / (end.y - start.y);
                    intersection.y = minY;
                } else if (outcodeOut & 2) { // Right
                    intersection.y = start.y + (end.y - start.y) * (maxX - start.x) / (end.x - start.x);
                    intersection.x = maxX;
                } else if (outcodeOut & 1) { // Left
                    intersection.y = start.y + (end.y - start.y) * (minX - start.x) / (end.x - start.x);
                    intersection.x = minX;
                }

                if (outcodeOut == outcode0) {
                    start = intersection;
                    outcode0 = computeOutCode(start);
                } else {
                    end = intersection;
                    outcode1 = computeOutCode(end);
                }
            }
        }
    }

    u32 Renderer::addMesh(primitives::Vertex *vertices, u32 vertexCount) {
        u32 firstVertex = static_cast<u32>(m_meshVertices.size());
        m_meshVertices.reserve(m_meshVertices.size() + vertexCount);
        for (u32 i = 0; i < vertexCount; ++i) {
            m_meshVertices.push_back(vertices[i]);
        }
        return firstVertex;
    }

    void Renderer::clearMeshes() {
        m_meshVertices.clear();
    }
}