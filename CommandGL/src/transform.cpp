#include "cgl.hpp"
#include <cmath>

namespace cgl
{
    Transform::Transform()
        : m_position({0.0f, 0.0f}),
        m_scale   ({1.0f, 1.0f}),
        m_rotation(0.0f),
        m_origin  ({0.0f, 0.0f}) {}

    Transform::Transform(const Vector2<f32>& position,
                        const Vector2<f32>& scale,
                        f32 rotation,
                        const Vector2<f32>& origin)
        : m_position(position),
        m_scale   (scale),
        m_rotation(rotation),
        m_origin(origin) {}

    Matrix3<f32> Transform::getMatrix() const {
        Matrix3<f32> translation = Matrix3<f32>::Translate(m_position);
        Matrix3<f32> rotation = Matrix3<f32>::Rotate(m_rotation);
        Matrix3<f32> scale = Matrix3<f32>::Scale(m_scale);
        Matrix3<f32> inverseOrigin = Matrix3<f32>::Translate({-m_origin.x, -m_origin.y});
        Matrix3<f32> postScale = Matrix3<f32>::Scale(m_postScale);

        return postScale * translation * rotation * scale * inverseOrigin;
    }

    Vector2<f32> Transform::getPosition() const {
        return m_position;
    }

    Vector2<f32> Transform::getScale() const {
        return m_scale;
    }

    f32 Transform::getRotation() const {
        return m_rotation;
    }

    Vector2<f32> Transform::getOrigin() const {
        return m_origin;
    }

    void Transform::setPosition(const Vector2<f32>& position) {
        m_position = position;
    }

    void Transform::setScale(const Vector2<f32>& scale) {
        m_scale = scale;
    }

    void Transform::setRotation(f32 rotation) {
        m_rotation = degreesToRadians(rotation);
    }

    void Transform::setOrigin(const Vector2<f32>& origin) {
        m_origin = origin;
    }

    void Transform::rotate(f32 degrees) {
        m_rotation += degreesToRadians(degrees);
    }

    void Transform::move(const Vector2<f32>& delta) {
        m_position.x += delta.x;
        m_position.y += delta.y;
    }

    float Transform::degreesToRadians(float degrees) {
        constexpr float conversionFactor = std::numbers::pi / 180.0f;
        return degrees * conversionFactor;
    }

    float Transform::radiansToDegrees(float radians) {
        constexpr float conversionFactor = 180.0f / std::numbers::pi;
        return radians * conversionFactor;
    }
}
