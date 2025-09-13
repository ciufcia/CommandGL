/**
 * @file vector2.hpp
 * @brief 2D vector mathematics and utilities for Textil library
 * @details Provides a templated Vector2 class supporting arithmetic operations on 2D vectors,
 *          along with utility functions for UV coordinate manipulation and gradient sampling.
 */

#ifndef TIL_VECTOR2_HPP
#define TIL_VECTOR2_HPP

#include <concepts>
#include <cmath>
#include "numeric_types.hpp"
#include "color.hpp"
#include "errors.hpp"

namespace til
{
    /**
     * @brief Concept constraining template types to arithmetic types
     * @details Ensures that Vector2 can only be instantiated with integral or floating-point types,
     *          providing compile-time safety for mathematical operations.
     */
    template<typename T>
    concept arithmetic = std::integral<T> || std::floating_point<T>;
    
    /**
     * @brief A 2D vector class template supporting various arithmetic operations
     * @tparam T The numeric type for vector components (must satisfy arithmetic concept)
     * @details Vector2 represents a point or direction in 2D space with x and y components.
     *          It supports all standard vector operations including addition, subtraction,
     *          scalar multiplication, dot product, normalization, and magnitude calculations.
     *          The class is designed to work efficiently with both integer and floating-point types.
     */
    template<arithmetic T>
    class Vector2
    {
    public:
        T x; ///< X component of the vector
        T y; ///< Y component of the vector
        
        /**
         * @brief Default constructor initializing vector to (0, 0)
         */
        Vector2();
        
        /**
         * @brief Construct vector with specified x and y components
         * @param x X component value
         * @param y Y component value
         */
        Vector2(T x, T y);
        
        /**
         * @brief Vector addition operator
         * @param other Vector to add
         * @return New vector representing the sum of this vector and other
         * @details Performs component-wise addition: result = (x1 + x2, y1 + y2)
         */
        Vector2<T> operator+(const Vector2<T>& other) const;
        
        /**
         * @brief Vector subtraction operator
         * @param other Vector to subtract
         * @return New vector representing the difference of this vector and other
         * @details Performs component-wise subtraction: result = (x1 - x2, y1 - y2)
         */
        Vector2<T> operator-(const Vector2<T>& other) const;
        
        /**
         * @brief Scalar multiplication operator
         * @param scalar Value to multiply each component by
         * @return New vector with each component multiplied by scalar
         * @details Scales the vector by the given scalar value
         */
        Vector2<T> operator*(T scalar) const;
        
        /**
         * @brief Friend function for scalar multiplication with scalar on the left
         * @param scalar Value to multiply each component by
         * @param vec Vector to multiply
         * @return New vector with each component multiplied by scalar
         * @details Allows syntax: scalar * vector in addition to vector * scalar
         */
        friend Vector2<T> operator*(T scalar, const Vector2<T>& vec) {
            return vec * scalar;
        }
        
        /**
         * @brief Scalar division operator
         * @param scalar Value to divide each component by
         * @return New vector with each component divided by scalar
         * @throws InvalidArgumentError if scalar is zero
         * @details Divides the vector by the given scalar value, with zero-division protection
         */
        Vector2<T> operator/(T scalar) const;
        
        /**
         * @brief Compound addition assignment operator
         * @param other Vector to add to this vector
         * @return Reference to this vector after addition
         * @details Modifies this vector by adding other to it
         */
        Vector2<T>& operator+=(const Vector2<T>& other);
        
        /**
         * @brief Compound subtraction assignment operator
         * @param other Vector to subtract from this vector
         * @return Reference to this vector after subtraction
         * @details Modifies this vector by subtracting other from it
         */
        Vector2<T>& operator-=(const Vector2<T>& other);
        
        /**
         * @brief Compound scalar multiplication assignment operator
         * @param scalar Value to multiply each component by
         * @return Reference to this vector after scaling
         * @details Modifies this vector by scaling it by the given scalar
         */
        Vector2<T>& operator*=(T scalar);
        
        /**
         * @brief Compound scalar division assignment operator
         * @param scalar Value to divide each component by
         * @return Reference to this vector after division
         * @throws InvalidArgumentError if scalar is zero
         * @details Modifies this vector by dividing it by the given scalar
         */
        Vector2<T>& operator/=(T scalar);
        
        /**
         * @brief Unary negation operator
         * @return New vector with both components negated
         * @details Returns the vector pointing in the opposite direction: (-x, -y)
         */
        Vector2<T> operator-() const;
        
        /**
         * @brief Calculate dot product with another vector
         * @param other Vector to compute dot product with
         * @return Scalar result of the dot product
         * @details Computes x1*x2 + y1*y2. Result is positive when vectors point in similar directions,
         *          zero when perpendicular, and negative when pointing in opposite directions.
         */
        T dot(const Vector2<T>& other) const;
        
        /**
         * @brief Equality comparison operator
         * @param other Vector to compare with
         * @return True if both components are exactly equal
         * @details Performs exact comparison of floating-point values, which may have precision issues
         */
        bool operator==(const Vector2<T>& other) const;
        
        /**
         * @brief Inequality comparison operator
         * @param other Vector to compare with
         * @return True if any component differs
         */
        bool operator!=(const Vector2<T>& other) const;
        
        /**
         * @brief Calculate squared magnitude of the vector
         * @return Squared length of the vector (x² + y²)
         * @details More efficient than magnitude() when only relative lengths are needed,
         *          as it avoids the expensive square root operation.
         */
        T magnitudeSquared() const;
        
        /**
         * @brief Calculate magnitude (length) of the vector
         * @return Length of the vector (√(x² + y²))
         * @details Computes the Euclidean length of the vector. For performance-critical code
         *          where only relative magnitudes matter, consider using magnitudeSquared().
         */
        T magnitude() const;
        
        /**
         * @brief Get normalized version of this vector
         * @return Unit vector in the same direction, or zero vector if magnitude is zero
         * @details Returns a vector with magnitude 1.0 pointing in the same direction.
         *          If the original vector has zero magnitude, returns (0, 0) to avoid division by zero.
         */
        Vector2<T> normalized() const;
        
        /**
         * @brief Type conversion constructor
         * @tparam U Source arithmetic type to convert from
         * @param other Vector of different type to convert
         * @details Performs static_cast conversion between different arithmetic types.
         *          Useful for converting between integer and floating-point vectors.
         */
        template<arithmetic U>
        Vector2(const Vector2<U>& other);
    };

    /**
     * @brief Normalize UV coordinates to [0,1] range
     * @param uv UV coordinates that may be outside [0,1] range
     * @return UV coordinates clamped/wrapped to valid [0,1] range
     * @details Ensures UV coordinates are valid for texture sampling operations
     */
    Vector2<f32> normalizeUV(const Vector2<f32> &uv);
    
    /**
     * @brief Sample a UV gradient color
     * @param uv UV coordinates in [0,1] range
     * @return Color sampled from the UV gradient
     * @details Generates colors based on UV coordinates, typically creating rainbow or gradient effects.
     *          U coordinate controls one color component, V coordinate controls another.
     */
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

#endif // TIL_VECTOR2_HPP