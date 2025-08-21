#ifndef CGL_VECTOR2_HPP
#define CGL_VECTOR2_HPP

#include <concepts>
#include <cmath>
#include "numeric_types.hpp"
#include "color.hpp"
#include "errors.hpp"

namespace cgl
{
    template<typename T>
    concept arithmetic = std::integral<T> || std::floating_point<T>;
    template<arithmetic T>
    class Vector2
    {
    public:
        T x;
        T y;
        Vector2();
        Vector2(T x, T y);
        Vector2<T> operator+(const Vector2<T>& other) const;
        Vector2<T> operator-(const Vector2<T>& other) const;
        Vector2<T> operator*(T scalar) const;
        friend Vector2<T> operator*(T scalar, const Vector2<T>& vec) {
            return vec * scalar;
        }
        Vector2<T> operator/(T scalar) const;
        Vector2<T>& operator+=(const Vector2<T>& other);
        Vector2<T>& operator-=(const Vector2<T>& other);
        Vector2<T>& operator*=(T scalar);
        Vector2<T>& operator/=(T scalar);
        Vector2<T> operator-() const; // unary negation
        T dot(const Vector2<T>& other) const;
        bool operator==(const Vector2<T>& other) const;
        bool operator!=(const Vector2<T>& other) const;
        T magnitudeSquared() const;
        T magnitude() const;
        Vector2<T> normalized() const;
        template<arithmetic U>
        Vector2(const Vector2<U>& other);
    };

    Vector2<f32> normalizeUV(const Vector2<f32> &uv);
    Color sampleUVGradient(const Vector2<f32> &uv);

    template<arithmetic T>
    Vector2<T>::Vector2() : x(static_cast<T>(0)), y(static_cast<T>(0)) {}

    template<arithmetic T>
    Vector2<T>::Vector2(T x, T y) : x(x), y(y) {}

    template<arithmetic T>
    template<arithmetic U>
    Vector2<T>::Vector2(const Vector2<U>& other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}

    template<arithmetic T>
    Vector2<T> Vector2<T>::operator+(const Vector2<T>& other) const {
        return Vector2<T>(x + other.x, y + other.y);
    }

    template<arithmetic T>
    Vector2<T> Vector2<T>::operator-(const Vector2<T>& other) const {
        return Vector2<T>(x - other.x, y - other.y);
    }

    template<arithmetic T>
    Vector2<T> Vector2<T>::operator*(T scalar) const {
        return Vector2<T>(x * scalar, y * scalar);
    }

    template<arithmetic T>
    Vector2<T> Vector2<T>::operator/(T scalar) const {
        if (scalar == static_cast<T>(0)) {
            invokeError<InvalidArgumentError>("Cannot divide Vector2 by zero");
        }
        return Vector2<T>(x / scalar, y / scalar);
    }

    template<arithmetic T>
    Vector2<T>& Vector2<T>::operator+=(const Vector2<T>& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    template<arithmetic T>
    Vector2<T>& Vector2<T>::operator-=(const Vector2<T>& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    template<arithmetic T>
    Vector2<T>& Vector2<T>::operator*=(T scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    template<arithmetic T>
    Vector2<T>& Vector2<T>::operator/=(T scalar) {
        if (scalar == static_cast<T>(0)) {
            invokeError<InvalidArgumentError>("Cannot divide Vector2 by zero");
        }
        x /= scalar;
        y /= scalar;
        return *this;
    }

    template<arithmetic T>
    Vector2<T> Vector2<T>::operator-() const {
        return Vector2<T>(-x, -y);
    }

    template<arithmetic T>
    T Vector2<T>::dot(const Vector2<T>& other) const {
        return x * other.x + y * other.y;
    }

    template<arithmetic T>
    bool Vector2<T>::operator==(const Vector2<T>& other) const {
        return x == other.x && y == other.y;
    }

    template<arithmetic T>
    bool Vector2<T>::operator!=(const Vector2<T>& other) const {
        return !(*this == other);
    }

    template<arithmetic T>
    T Vector2<T>::magnitudeSquared() const {
        return x * x + y * y;
    }

    template<arithmetic T>
    T Vector2<T>::magnitude() const {
        using std::sqrt;
        return sqrt(static_cast<double>(magnitudeSquared()));
    }

    template<arithmetic T>
    Vector2<T> Vector2<T>::normalized() const {
        T mag = magnitude();
        if (mag == static_cast<T>(0)) {
            return Vector2<T>(static_cast<T>(0), static_cast<T>(0));
        }
        return *this / mag;
    }
}

#endif // CGL_VECTOR2_HPP