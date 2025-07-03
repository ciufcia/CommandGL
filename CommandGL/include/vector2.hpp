#ifndef CGL_VECTOR2_HPP
#define CGL_VECTOR2_HPP

#include <concepts>
#include <cmath>

namespace cgl
{
    /**
     * @brief Concept for arithmetic types (integral or floating point).
     */
    template<typename T>
    concept arithmetic = std::integral<T> || std::floating_point<T>;

    /**
     * @class Vector2
     * @brief A simple 2D vector class template.
     * @tparam T The type of the vector components (e.g., float, int).
     */
    template<arithmetic T>
    class Vector2
    {
    public:
        /**
         * @brief The x-coordinate of the vector.
         */
        T x;
        /**
         * @brief The y-coordinate of the vector.
         */
        T y;

        /**
         * @brief Default constructor initializes the vector to (0, 0).
         */
        Vector2();

        /**
         * @brief Constructor that initializes the vector with given x and y values.
         * @param x The x-coordinate.
         * @param y The y-coordinate.
         */
        Vector2(T x, T y);

        /**
         * @brief Adds two vectors.
         */
        Vector2<T> operator+(const Vector2<T>& other) const;

        /**
         * @brief Subtracts two vectors.
         */
        Vector2<T> operator-(const Vector2<T>& other) const;

        /**
         * @brief Multiplies the vector by a scalar.
         */
        Vector2<T> operator*(T scalar) const;

        /**
         * @brief Multiplies the vector by a scalar (friend function).
         * @param scalar The scalar value.
         * @param vec The vector to multiply.
         * @return The result of scalar * vector.
         */
        friend Vector2<T> operator*(T scalar, const Vector2<T>& vec) {
            return vec * scalar;
        }

        /**
         * @brief Divides the vector by a scalar.
         */
        Vector2<T> operator/(T scalar) const;

        /**
         * @brief Computes the dot product of two vectors.
         */
        T dot(const Vector2<T>& other) const;

        /**
         * @brief Checks if two vectors are equal.
         */
        bool operator==(const Vector2<T>& other) const;

        /**
         * @brief Checks if two vectors are not equal.
         */
        bool operator!=(const Vector2<T>& other) const;

        /**
         * @brief Returns the squared magnitude (length^2) of the vector.
         */
        T magnitude_squared() const;

        /**
         * @brief Returns the magnitude (length) of the vector.
         */
        T magnitude() const;

        /**
         * @brief Returns a normalized (unit length) vector.
         */
        Vector2<T> normalized() const;
    };

    template<arithmetic T>
    Vector2<T>::Vector2() : x(static_cast<T>(0)), y(static_cast<T>(0)) {}

    template<arithmetic T>
    Vector2<T>::Vector2(T x, T y) : x(x), y(y) {}

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
        return Vector2<T>(x / scalar, y / scalar);
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
    T Vector2<T>::magnitude_squared() const {
        return x * x + y * y;
    }

    template<arithmetic T>
    T Vector2<T>::magnitude() const {
        using std::sqrt;
        return sqrt(static_cast<double>(magnitude_squared()));
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