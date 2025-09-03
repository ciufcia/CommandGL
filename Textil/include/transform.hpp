#ifndef TIL_TRANSFORM_HPP
#define TIL_TRANSFORM_HPP

#include "matrix3.hpp"
#include "vector2.hpp"
#include "numeric_types.hpp"
#include <cmath>
#include <numbers>

namespace til
{
    class Transform
    {
    public:

        Transform();
        Transform(const Vector2<f32>& position, const Vector2<f32>& scale, f32 rotation, const Vector2<f32>& origin);

        Matrix3<f32> getMatrix() const;

        Vector2<f32> getPosition() const;
        Vector2<f32> getScale() const;
        f32          getRotation() const;
        Vector2<f32> getOrigin() const;

        void setPosition(const Vector2<f32>& position);
        void setScale(const Vector2<f32>& scale);
        void setRotation(f32 rotation);
        void setOrigin(const Vector2<f32>& origin);
        
        void rotate(f32 degrees);
        void move(const Vector2<f32>& delta);

        static float degreesToRadians(float degrees);
        static float radiansToDegrees(float radians);

    private:

        Vector2<f32> m_position { 0.f, 0.f };
        Vector2<f32> m_scale { 1.f, 1.f };
        f32          m_rotation { 0.f };
        Vector2<f32> m_origin { 0.f, 0.f };

    friend class Framework;
    };
}

#endif // TIL_TRANSFORM_HPP