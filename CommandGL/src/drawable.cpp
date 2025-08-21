#include "cgl.hpp"
#include <cmath>

namespace cgl
{
    std::shared_ptr<Drawable> Drawable::clone() const {
        return nullptr;
    }

    void drawables::Mesh::generateGeometry(std::vector<filters::GeometryElementData> &drawableBuffer, Transform &transform) {
        int triangleCount = static_cast<int>(points.size() / 3);

        if (triangleCount <= 0) {
            invokeError<InvalidArgumentError>("Not enough points to form a mesh.");
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

        std::vector<filters::GeometryElementData> triangleBuffer;

        for (int ti = 0; ti < triangleCount; ++ti) {
            const auto &triangle = triangles[ti];

            Vector2<i32> size = {
                static_cast<i32>(std::ceil(triangle.bottomRight.x - triangle.topLeft.x)),
                static_cast<i32>(std::ceil(triangle.bottomRight.y - triangle.topLeft.y))
            };

            triangleBuffer.resize((size.x + 1) * (size.y + 1));

            #pragma omp parallel for collapse(2) schedule(dynamic)
            for (int y = static_cast<int>(triangle.topLeft.y); y <= static_cast<int>(triangle.bottomRight.y); ++y)
            for (int x = static_cast<int>(triangle.topLeft.x); x <= static_cast<int>(triangle.bottomRight.x); ++x) {
                f32 e1 = triangle.e1a * x + triangle.e1b * y + triangle.e1c;
                f32 e2 = triangle.e2a * x + triangle.e2b * y + triangle.e2c;
                f32 e3 = triangle.e3a * x + triangle.e3b * y + triangle.e3c;
                
                bool inside = (
                    (e1 > 0 || (e1 == 0 && triangle.e1_topLeft)) &&
                    (e2 > 0 || (e2 == 0 && triangle.e2_topLeft)) &&
                    (e3 > 0 || (e3 == 0 && triangle.e3_topLeft))
                );
                
                int index = (y - static_cast<int>(triangle.topLeft.y)) * size.x + (x - static_cast<int>(triangle.topLeft.x));

                filters::GeometryElementData &pixelData = triangleBuffer[index];

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
                if (pixelData.size.x >= 0 && pixelData.size.y >= 0) {
                    drawableBuffer.push_back(pixelData);
                }
            }
        }
    }

    bool drawables::Mesh::isTopEdge(const Vector2<f32>& v1, const Vector2<f32>& v2) {
        return v1.y == v2.y && v2.x > v1.x;
    }

    bool drawables::Mesh::isLeftEdge(const Vector2<f32>& v1, const Vector2<f32>& v2) {
        return v2.y < v1.y;
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
            invokeError<InvalidArgumentError>("Index out of range");
        }

        points[index] = point;

        f32 signedArea = (points[1] - points[0]).x * (points[2].y - points[0].y) - (points[2].x - points[0].x) * (points[1].y - points[0].y);

        if (signedArea < 0) {
            std::swap(points[0], points[2]);
        }
    }

    Vector2<f32> drawables::Triangle::getPoint(u8 index) const {
        if (index >= 3) {
            invokeError<InvalidArgumentError>("Index out of range");
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

    drawables::Rectangle::Rectangle(const Vector2<f32> &size) {
        Vector2<f32> topLeft = { 0, 0 };
        Vector2<f32> topRight = { size.x, 0 };
        Vector2<f32> bottomRight = { size.x, size.y };
        Vector2<f32> bottomLeft = { 0, size.y };

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

    void drawables::Rectangle::setSize(const Vector2<f32> &size) {
        Vector2<f32> topLeft = { 0, 0 };
        Vector2<f32> topRight = { size.x, 0 };
        Vector2<f32> bottomRight = { size.x, size.y };
        Vector2<f32> bottomLeft = { 0, size.y };

        points[0] = topLeft; points[1] = topRight; points[2] = bottomRight;
        points[3] = topLeft; points[4] = bottomRight; points[5] = bottomLeft;

        m_size = size;
    }

    Vector2<f32> drawables::Rectangle::getSize() const {
        return m_size;
    }

    void drawables::Point::generateGeometry(std::vector<filters::GeometryElementData> &drawableBuffer, Transform &transform) {
        Vector2<f32> transformedPosition = transform.getMatrix() * position;

        filters::GeometryElementData pixelData;
        pixelData.position = transformedPosition;
        pixelData.uv = { 0.f, 0.f };
        pixelData.size = { 1.f, 1.f };
        pixelData.inverseSize = { 1.f, 1.f };

        drawableBuffer.push_back(pixelData);
    }

    void drawables::Ellipse::generateGeometry(std::vector<filters::GeometryElementData> &drawableBuffer, Transform &transform) {
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
            std::vector<filters::GeometryElementData> localBuffer;
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
                        filters::GeometryElementData pixelData;
                        
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

    void drawables::Line::generateGeometry(std::vector<filters::GeometryElementData> &drawableBuffer, Transform &transform) {
        Matrix3<f32> transformMatrix = transform.getMatrix();

        Vector2<f32> startTransformed = transformMatrix * start;
        Vector2<f32> endTransformed = transformMatrix * end;

        Vector2<f32> difference = endTransformed - startTransformed;
        Vector2<f32> inverseDifference = { 1.f / difference.x, 1.f / difference.y };
        float length = difference.magnitude();

        filters::GeometryElementData pixelData;

        if (length < 1e-6f) {
            pixelData.position = startTransformed;
            pixelData.uv = { 0.f, 0.f };
            pixelData.size = { 1.f, 1.f };
            pixelData.inverseSize = { 1.f, 1.f };

            drawableBuffer.push_back(pixelData);
            return;
        }

        u32 steps = static_cast<u32>(std::ceil(length));
        f32 inverseSteps = 1.f / static_cast<f32>(steps);
        Vector2<f32> step = difference / static_cast<f32>(steps);

        for (u32 i = 0; i <= steps; ++i) {
            Vector2<f32> position = startTransformed + step * static_cast<f32>(i);

            pixelData.position = position;
            pixelData.uv = { static_cast<f32>(i) * inverseSteps, 0.f };
            pixelData.size = difference;
            pixelData.inverseSize = inverseDifference;

            drawableBuffer.push_back(pixelData);
        }
    }

    drawables::Polygon::Polygon(const std::vector<Vector2<f32>> &points) {
        nonTriangulatedPoints = points;
        triangulated = false;
    }

    void drawables::Polygon::addPoint(const Vector2<f32> &point) {
        nonTriangulatedPoints.push_back(point);
        triangulated = false;
    }

    void drawables::Polygon::insertPoint(u32 index, const Vector2<f32> &point) {
        nonTriangulatedPoints.insert(std::next(nonTriangulatedPoints.begin(), index), point);
        triangulated = false;
    }

    void drawables::Polygon::removePoint(u32 index) {
        nonTriangulatedPoints.erase(std::next(nonTriangulatedPoints.begin(), index));
        triangulated = false;
    }

    Vector2<f32> drawables::Polygon::getPoint(u32 index) const {
        if (index >= nonTriangulatedPoints.size()) {
            invokeError<InvalidArgumentError>("Index out of range");
        }

        return nonTriangulatedPoints[index];
    }

    void drawables::Polygon::setPoint(u32 index, const Vector2<f32> &point) {
        if (index >= nonTriangulatedPoints.size()) {
            invokeError<InvalidArgumentError>("Index out of range");
        }

        nonTriangulatedPoints[index] = point;
        triangulated = false;
    }

    const std::vector<Vector2<f32>>& drawables::Polygon::getPoints() const {
        return nonTriangulatedPoints;
    }

    void drawables::Polygon::generateGeometry(std::vector<filters::GeometryElementData> &drawableBuffer, Transform &transform) {
        if (!triangulated) {
            triangulate();
        }

        Mesh::generateGeometry(drawableBuffer, transform);
    }

    void drawables::Polygon::triangulate() {
        if (nonTriangulatedPoints.size() < 3) {
            invokeError<InvalidArgumentError>("Not enough points to form a polygon.");
        }

        auto nonTriangulatedPointsList = std::list<Vector2<f32>>(nonTriangulatedPoints.begin(), nonTriangulatedPoints.end());

        if (calculateSignedArea() < 0) {
            std::reverse(nonTriangulatedPointsList.begin(), nonTriangulatedPointsList.end());
        }
        
        Vector2<f32> topleft = { std::numeric_limits<f32>::max(), std::numeric_limits<f32>::max() };
        Vector2<f32> bottomright = { std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::lowest() };

        for (const auto &point : nonTriangulatedPoints) {
            topleft.x = std::min(topleft.x, point.x);
            topleft.y = std::min(topleft.y, point.y);
            bottomright.x = std::max(bottomright.x, point.x);
            bottomright.y = std::max(bottomright.y, point.y);
        }
        
        Vector2<f32> size = bottomright - topleft;
        Vector2<f32> inverseSize = { 1.f / size.x, 1.f / size.y };

        points.clear();
        uvs.clear();

        auto p1 = nonTriangulatedPointsList.begin();
        auto p2 = std::next(p1);
        auto p3 = std::next(p2);

        auto advance = [&](auto it) {
            ++it;
            if (it == nonTriangulatedPointsList.end()) {
                it = nonTriangulatedPointsList.begin();
            }
            return it;
        };

        u32 count = 0u;

        while (nonTriangulatedPointsList.size() > 3) {
            if (count >= nonTriangulatedPointsList.size()) {
                invokeError<LogicError>("Couldn't triangulate polygon");
            }

            count++;

            f32 turn = (p2->x - p1->x) * (p3->y - p2->y) - (p3->x - p2->x) * (p2->y - p1->y);

            if (turn <= 0) {
                p1 = p2;
                p2 = p3;
                p3 = advance(p2);
                continue;
            }

            f32 e1a = p1->y - p2->y; f32 e1b = p2->x - p1->x; f32 e1c = p1->x * p2->y - p2->x * p1->y;
            f32 e2a = p2->y - p3->y; f32 e2b = p3->x - p2->x; f32 e2c = p2->x * p3->y - p3->x * p2->y;
            f32 e3a = p3->y - p1->y; f32 e3b = p1->x - p3->x; f32 e3c = p3->x * p1->y - p1->x * p3->y;

            bool found = false;
            for (auto it = nonTriangulatedPointsList.begin(); it != nonTriangulatedPointsList.end(); it++) {
                if (it == p1 || it == p2 || it == p3) {
                    continue;
                }

                f32 e1 = e1a * it->x + e1b * it->y + e1c;
                f32 e2 = e2a * it->x + e2b * it->y + e2c;
                f32 e3 = e3a * it->x + e3b * it->y + e3c;

                bool inside = (e1 > 0 && e2 > 0 && e3 > 0);

                if (inside) {
                    found = true;
                    break;
                }
            }

            if (found) {
                p1 = p2;
                p2 = p3;
                p3 = advance(p2);
                continue;
            }

            count = 0u;

            points.push_back(*p1); uvs.push_back({ (p1->x - topleft.x) * inverseSize.x, (p1->y - topleft.y) * inverseSize.y });
            points.push_back(*p2); uvs.push_back({ (p2->x - topleft.x) * inverseSize.x, (p2->y - topleft.y) * inverseSize.y });
            points.push_back(*p3); uvs.push_back({ (p3->x - topleft.x) * inverseSize.x, (p3->y - topleft.y) * inverseSize.y });

            nonTriangulatedPointsList.erase(p2);
            p2 = p3;
            p3 = advance(p2);
        }

        auto it = nonTriangulatedPointsList.begin();
        points.push_back(*it); uvs.push_back({ (it->x - topleft.x) * inverseSize.x, (it->y - topleft.y) * inverseSize.y }); it++;
        points.push_back(*it); uvs.push_back({ (it->x - topleft.x) * inverseSize.x, (it->y - topleft.y) * inverseSize.y }); it++;
        points.push_back(*it); uvs.push_back({ (it->x - topleft.x) * inverseSize.x, (it->y - topleft.y) * inverseSize.y }); it++;

        nonTriangulatedPointsList.clear();

		triangulated = true;
    }

    f32 drawables::Polygon::calculateSignedArea() {
        f32 area = 0.f;

        for (u32 i = 0; i < nonTriangulatedPoints.size(); ++i) {
            u32 next_x = (i + 1) % nonTriangulatedPoints.size();
            u32 next_y = (i + 1) % nonTriangulatedPoints.size();

            area += nonTriangulatedPoints[i].x * nonTriangulatedPoints[next_y].y - nonTriangulatedPoints[next_x].x * nonTriangulatedPoints[i].y;
        }

        return area * 0.5f;
    }
}