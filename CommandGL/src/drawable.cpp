#include "cgl.hpp"
#include <cmath>

namespace cgl
{
    void Drawable::applyFragmentPipeline(std::vector<filter_pass_data::PixelPass> &drawableBuffer, f32 time) {
        fragmentPipeline.start();

        while (fragmentPipeline.step()) {
            auto currentFilter = fragmentPipeline.getCurrentFilter();

            if (!currentFilter->isEnabled)
                continue;

            if (currentFilter->type == FilterType::SinglePass) {
                filter_pass_data::PixelSinglePass passData;
                passData.pixelBuffer = &drawableBuffer;
                passData.time = time;

                currentFilter->function(currentFilter->data.get(), &passData);
            } else if (currentFilter->type == FilterType::Sequential) {
                filter_pass_data::PixelPass passData;

                for (u32 i = 0; i < drawableBuffer.size(); ++i) {
                    passData = drawableBuffer[i];
                    passData.time = time;
                    currentFilter->function(currentFilter->data.get(), &passData);
                    drawableBuffer[i] = passData;
                }
            } else if (currentFilter->type == FilterType::Parallel) {
                #pragma omp parallel for
                for (int i = 0; i < static_cast<int>(drawableBuffer.size()); ++i) {
                    filter_pass_data::PixelPass passData = drawableBuffer[i];
                    passData.time = time;
                    currentFilter->function(currentFilter->data.get(), &passData);
                    drawableBuffer[i] = passData;
                }
            }
        }
    }

    void drawables::Mesh::generateGeometry(std::vector<filter_pass_data::PixelPass> &drawableBuffer, Transform &transform) {
        int triangleCount = static_cast<int>(points.size() / 3);

        if (triangleCount <= 0) {
            throw std::runtime_error("Not enough points to form a mesh.");
        }

        std::vector<Vector2<f32>> transformedPoints(points.size());
        Matrix3<f32> transformMatrix = transform.getMatrix();
        for (size_t i = 0; i < points.size(); ++i) {
            transformedPoints[i] = transformMatrix * points[i];
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

        std::vector<TriangleParams> triangles(triangleCount);
        
        #pragma omp parallel for schedule(static)
        for (int i = 0; i < triangleCount; ++i) {
            auto p1 = transformedPoints[i * 3];
            auto p2 = transformedPoints[i * 3 + 1];
            auto p3 = transformedPoints[i * 3 + 2];

            auto &triangle = triangles[i];

            triangle.uv1 = uvs[i * 3]; triangle.uv2 = uvs[i * 3 + 1]; triangle.uv3 = uvs[i * 3 + 2];

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

        #pragma omp parallel
        {
            std::vector<filter_pass_data::PixelPass> localBuffer;
            localBuffer.reserve(256);

            #pragma omp for collapse(3) schedule(dynamic)
            for (int ti = 0; ti < triangleCount; ++ti)
            for (int y = static_cast<int>(triangles[ti].topLeft.y); y <= static_cast<int>(triangles[ti].bottomRight.y); ++y)
            for (int x = static_cast<int>(triangles[ti].topLeft.x); x <= static_cast<int>(triangles[ti].bottomRight.x); ++x) {
                auto &triangle = triangles[ti];
                
                f32 e1 = triangle.e1a * x + triangle.e1b * y + triangle.e1c;
                f32 e2 = triangle.e2a * x + triangle.e2b * y + triangle.e2c;
                f32 e3 = triangle.e3a * x + triangle.e3b * y + triangle.e3c;
                
                bool inside = (
                    (e1 > 0 || (e1 == 0 && triangle.e1_topLeft)) &&
                    (e2 > 0 || (e2 == 0 && triangle.e2_topLeft)) &&
                    (e3 > 0 || (e3 == 0 && triangle.e3_topLeft))
                );
                
                if (inside) {
                    f32 w1 = e2 * triangle.inverseArea;
                    f32 w2 = e3 * triangle.inverseArea;
                    f32 w3 = e1 * triangle.inverseArea;

                    filter_pass_data::PixelPass pixelData;
                    pixelData.position = { static_cast<f32>(x), static_cast<f32>(y) };
                    pixelData.uv = { triangle.uv1.x * w1 + triangle.uv2.x * w2 + triangle.uv3.x * w3,
                                     triangle.uv1.y * w1 + triangle.uv2.y * w2 + triangle.uv3.y * w3 };
                    pixelData.size = triangle.size;
                    pixelData.inverseSize = triangle.inverseSize;

                    localBuffer.push_back(pixelData);
                }
            }

            #pragma omp critical
            drawableBuffer.insert(drawableBuffer.end(), localBuffer.begin(), localBuffer.end());
        }
    }

    bool drawables::Mesh::isTopEdge(const Vector2<f32>& v1, const Vector2<f32>& v2) {
        return v1.y == v2.y && v1.x > v2.x;
    }

    bool drawables::Mesh::isLeftEdge(const Vector2<f32>& v1, const Vector2<f32>& v2) {
        return v1.y < v2.y;
    }

    bool drawables::Mesh::isTopOrLeftEdge(const Vector2<f32>& v1, const Vector2<f32>& v2) {
        return isTopEdge(v1, v2) || isLeftEdge(v1, v2);
    }

    drawables::Triangle::Triangle() {
        points.resize(3, { 0.f, 0.f });
        uvs = { { 0.f, 0.f }, { 1.f, 0.f }, { 0.f, 1.f } };
    }

    drawables::Triangle::Triangle(const Vector2<f32> &p1, const Vector2<f32> &p2, const Vector2<f32> &p3) {
		points.resize(3);

        f32 signedArea = (p2.x - p1.x) * (p3.y - p1.y) - (p3.x - p1.x) * (p2.y - p1.y);

        if (signedArea >= 0) {
            points[0] = p1;
            points[1] = p2;
            points[2] = p3;
        } else {
            points[0] = p1;
            points[1] = p3;
            points[2] = p2;
        }

        uvs = { { 0.f, 0.f }, { 1.f, 0.f }, { 0.f, 1.f } };
    }

    void drawables::Triangle::setPoint(u8 index, const Vector2<f32> &point) {
        if (index >= 3) {
            throw std::out_of_range("Index out of range");
        }

        points[index] = point;

        f32 signedArea = (points[1] - points[0]).x * (points[2].y - points[0].y) - (points[2].x - points[0].x) * (points[1].y - points[0].y);

        if (signedArea < 0) {
            std::swap(points[0], points[2]);
        }
    }

    Vector2<f32> drawables::Triangle::getPoint(u8 index) const {
        if (index >= 3) {
            throw std::out_of_range("Index out of range");
        }

        return points[index];
    }

    drawables::Rectangle::Rectangle() {
        points = {
            { 0.f, 0.f }, { 1.f, 0.f }, { 1.f, 1.f },
            { 0.f, 0.f }, { 1.f, 1.f }, { 0.f, 1.f }
        };
        uvs = {
            { 0.f, 0.f }, { 1.f, 1.f }, { 1.f, 0.f },
            { 0.f, 0.f }, { 0.f, 1.f }, { 1.f, 1.f }
        };

        m_size = { 1.f, 1.f };
    }

    drawables::Rectangle::Rectangle(const Vector2<f32> &topLeft, const Vector2<f32> &size) {
        Vector2<f32> topRight = { topLeft.x + size.x, topLeft.y };
        Vector2<f32> bottomRight = { topLeft.x + size.x, topLeft.y + size.y };
        Vector2<f32> bottomLeft = { topLeft.x, topLeft.y + size.y };

        points = {
            topLeft, topRight, bottomRight,
            topLeft, bottomRight, bottomLeft
        };
        uvs = {
            { 0.f, 0.f }, { 1.f, 0.f }, { 1.f, 1.f },
            { 0.f, 0.f }, { 1.f, 1.f }, { 0.f, 1.f }
        };

        m_size = size;
    }

    void drawables::Rectangle::setTopLeft(const Vector2<f32> &topLeft) {
        Vector2<f32> topRight = { topLeft.x + m_size.x, topLeft.y };
        Vector2<f32> bottomRight = { topLeft.x + m_size.x, topLeft.y + m_size.y };
        Vector2<f32> bottomLeft = { topLeft.x, topLeft.y + m_size.y };

        points[0] = topLeft; points[1] = topRight; points[2] = bottomRight;
        points[3] = topLeft; points[4] = bottomRight; points[5] = bottomLeft;
    }

    Vector2<f32> drawables::Rectangle::getTopLeft() const {
        return points[0];
    }

    void drawables::Rectangle::setBottomRight(const Vector2<f32> &bottomRight) {
        Vector2<f32> topLeft = bottomRight - m_size;
        Vector2<f32> topRight = { bottomRight.x, topLeft.y };
        Vector2<f32> bottomLeft = { topLeft.x, bottomRight.y };

        points[0] = topLeft; points[1] = topRight; points[2] = bottomRight;
        points[3] = topLeft; points[4] = bottomRight; points[5] = bottomLeft;
    }

    Vector2<f32> drawables::Rectangle::getBottomRight() const {
        return points[1];
    }

    void drawables::Rectangle::setSize(const Vector2<f32> &size) {
        Vector2<f32> topLeft = getTopLeft();
        Vector2<f32> topRight = { topLeft.x + size.x, topLeft.y };
        Vector2<f32> bottomRight = { topLeft.x + size.x, topLeft.y + size.y };
        Vector2<f32> bottomLeft = { topLeft.x, topLeft.y + size.y };

        points[0] = topLeft; points[1] = topRight; points[2] = bottomRight;
        points[3] = topLeft; points[4] = bottomRight; points[5] = bottomLeft;
    }

    Vector2<f32> drawables::Rectangle::getSize() const {
        return m_size;
    }

    void drawables::Point::generateGeometry(std::vector<filter_pass_data::PixelPass> &drawableBuffer, Transform &transform) {
        Vector2<f32> transformedPosition = transform.getMatrix() * position;

        filter_pass_data::PixelPass pixelData;
        pixelData.position = transformedPosition;
        pixelData.uv = { 0.f, 0.f };
        pixelData.size = { 1.f, 1.f };
        pixelData.inverseSize = { 1.f, 1.f };

        drawableBuffer.push_back(pixelData);
    }

    void drawables::Ellipse::generateGeometry(std::vector<filter_pass_data::PixelPass> &drawableBuffer, Transform &transform) {
        Matrix3<f32> transformMatrix = transform.getMatrix();
        Matrix3<f32> inverseMatrix = transformMatrix.inverse();

        Vector2<f32> corners[4] = {
            { center.x - radius.x, center.y - radius.y },
            { center.x + radius.x, center.y - radius.y },
            { center.x + radius.x, center.y + radius.y },
            { center.x - radius.x, center.y + radius.y }
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
        Vector2<f32> inverseDiameter = { 1.f / (2.f * radius.x), 1.f / (2.f * radius.y) };

        #pragma omp parallel
        {
            std::vector<filter_pass_data::PixelPass> localBuffer;
            localBuffer.reserve(256);

            #pragma omp for collapse(2) schedule(dynamic)
            for (int y = static_cast<int>(topLeft.y); y <= static_cast<int>(bottomRight.y); ++y) {
                for (int x = static_cast<int>(topLeft.x); x <= static_cast<int>(bottomRight.x); ++x) {
                    Vector2<f32> pixelPos = { static_cast<f32>(x), static_cast<f32>(y) };
                    Vector2<f32> localPos = inverseMatrix * pixelPos;

                    f32 dx = localPos.x - center.x;
                    f32 dy = localPos.y - center.y;
                    f32 distanceSquared = (dx * dx) / (radius.x * radius.x) + (dy * dy) / (radius.y * radius.y);

                    if (distanceSquared <= 1.f) {
                        filter_pass_data::PixelPass pixelData;
                        
                        pixelData.position = pixelPos;
                        pixelData.uv = {
                            (localPos.x - center.x) * inverseDiameter.x + 0.5f,
                            (localPos.y - center.y) * inverseDiameter.y + 0.5f
                        };
                        pixelData.size = size;
                        pixelData.inverseSize = inverseSize;

                        localBuffer.push_back(pixelData);
                    }
                }
            }

            #pragma omp critical
            drawableBuffer.insert(drawableBuffer.end(), localBuffer.begin(), localBuffer.end());
        }
    }
}