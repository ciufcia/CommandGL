/**
 * @file matrix3.hpp
 * @brief 3x3 matrix class for 2D transformations in Textil library
 * @details Provides a templated 3x3 matrix implementation optimized for 2D graphics transformations.
 *          Supports standard matrix operations and transformation generation (translate, rotate, scale).
 *          Uses homogeneous coordinates to represent 2D transformations in 3D matrix space.
 */

#ifndef TIL_MATRIX3_HPP
#define TIL_MATRIX3_HPP

#include "vector2.hpp"
#include "numeric_types.hpp"

namespace til
{
    /**
     * @brief 3x3 matrix class template for 2D transformations
     * @tparam T Numeric type for matrix elements (typically f32 or f64)
     * @details Represents a 3x3 matrix using homogeneous coordinates for 2D transformations.
     *          The matrix layout is row-major, where matrix[row][column] accesses elements.
     *          Commonly used for combining translation, rotation, and scaling operations
     *          in a single transformation matrix that can be applied to Vector2 points.
     * 
     * Matrix layout for 2D transformations:
     * ```
     * | sx  shx  tx |   where: sx/sy = scale, shx/shy = shear/rotation,
     * | shy sy   ty |          tx/ty = translation
     * | 0   0    1  |
     * ```
     */
    template<typename T>
    class Matrix3
    {
    public:
        /**
         * @brief Default constructor creating uninitialized matrix
         * @details Matrix elements are not initialized and contain garbage values.
         *          Use identity() or explicit initialization for predictable behavior.
         */
        Matrix3() = default;
        
        /**
         * @brief Constructor initializing all elements to a single value
         * @param value Value to set for all matrix elements
         * @details Useful for creating zero matrices: Matrix3<f32>(0.0f)
         */
        Matrix3(T value);
        
        /**
         * @brief Constructor from array of 9 values in row-major order
         * @param values Array of 9 values: [m00, m01, m02, m10, m11, m12, m20, m21, m22]
         * @details Values are arranged in row-major order where index i*3+j corresponds to matrix[i][j].
         */
        Matrix3(const T values[9]);

        /**
         * @brief Transform a 2D vector using this matrix
         * @param vec 2D vector to transform (treated as homogeneous coordinate [x, y, 1])
         * @return Transformed 2D vector
         * @details Multiplies the matrix by the vector in homogeneous coordinates.
         *          Only returns the x,y components of the result, effectively performing:
         *          result.x = m[0][0]*vec.x + m[0][1]*vec.y + m[0][2]
         *          result.y = m[1][0]*vec.x + m[1][1]*vec.y + m[1][2]
         */
        Vector2<T> operator*(const Vector2<T>& vec) const;
        
        /**
         * @brief Matrix multiplication operator
         * @param other Matrix to multiply with (on the right)
         * @return New matrix representing the composition of transformations
         * @details Performs standard matrix multiplication. Order matters: A*B ≠ B*A in general.
         *          The result represents applying transformation 'other' first, then 'this'.
         */
        Matrix3<T> operator*(const Matrix3<T>& other) const;
        
        /**
         * @brief Calculate matrix inverse
         * @return Inverse matrix that undoes this transformation
         * @throws LogicError if matrix is singular (determinant is zero)
         * @details Computes the mathematical inverse using cofactor expansion.
         *          The inverse matrix satisfies: M * M^(-1) = Identity.
         *          Useful for converting from transformed coordinates back to original space.
         */
        Matrix3<T> inverse() const;

        /**
         * @brief Create translation transformation matrix
         * @param v Translation vector (offset to apply)
         * @return Translation matrix that moves points by vector v
         * @details Creates matrix that translates points: new_point = old_point + v
         *          Matrix form:
         *          ```
         *          | 1  0  v.x |
         *          | 0  1  v.y |
         *          | 0  0   1  |
         *          ```
         */
        static Matrix3<T> translate(const Vector2<T>& v);
        
        /**
         * @brief Create rotation transformation matrix
         * @param radians Rotation angle in radians (positive = counterclockwise)
         * @return Rotation matrix that rotates points around origin
         * @details Creates matrix that rotates points around the origin (0,0).
         *          For rotation around different points, combine with translation matrices.
         *          Matrix form:
         *          ```
         *          |  cos(θ)  -sin(θ)  0 |
         *          |  sin(θ)   cos(θ)  0 |
         *          |    0        0     1 |
         *          ```
         */
        static Matrix3<T> rotate(T radians);
        
        /**
         * @brief Create scaling transformation matrix
         * @param v Scale factors for x and y axes
         * @return Scale matrix that resizes objects by specified factors
         * @details Creates matrix that scales points: new_point = old_point * scale_factors
         *          Scaling is applied relative to the origin (0,0).
         *          Matrix form:
         *          ```
         *          | v.x  0   0 |
         *          |  0  v.y  0 |
         *          |  0   0   1 |
         *          ```
         */
        static Matrix3<T> scale(const Vector2<T>& v);
        
        /**
         * @brief Create identity transformation matrix
         * @return Identity matrix (no transformation applied)
         * @details Creates the multiplicative identity matrix where A * I = I * A = A.
         *          Points transformed by identity matrix remain unchanged.
         *          Matrix form:
         *          ```
         *          | 1  0  0 |
         *          | 0  1  0 |
         *          | 0  0  1 |
         *          ```
         */
        static Matrix3<T> identity();

        /**
         * @brief Access matrix row for modification
         * @param row Row index (0-2)
         * @return Pointer to the first element of the specified row
         * @details Allows direct modification of matrix elements: matrix[row][col] = value
         */
        T* operator[](std::size_t row) { return m[row]; }
        
        /**
         * @brief Access matrix row for reading (const version)
         * @param row Row index (0-2) 
         * @return Const pointer to the first element of the specified row
         * @details Allows read-only access to matrix elements: value = matrix[row][col]
         */
        const T* operator[](std::size_t row) const { return m[row]; }

    private:
        T m[3][3]; ///< Matrix data stored in row-major order [row][column]
    };

    template<typename T>
    Matrix3<T>::Matrix3(T value) {
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                m[i][j] = value;
    }

    template<typename T>
    Matrix3<T>::Matrix3(const T values[9]) {
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                m[i][j] = values[i * 3 + j];
    }

    template<typename T>
    Vector2<T> Matrix3<T>::operator*(const Vector2<T>& vec) const {
        return Vector2<T>(
            m[0][0] * vec.x + m[0][1] * vec.y + m[0][2],
            m[1][0] * vec.x + m[1][1] * vec.y + m[1][2]
        );
    }

    template<typename T>
    Matrix3<T> Matrix3<T>::inverse() const {
        T det = m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
                m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
                m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);

        if (det == 0) {
            invokeError<LogicError>("Matrix is singular and cannot be inverted.");
        }

        Matrix3<T> result;
        result.m[0][0] = (m[1][1] * m[2][2] - m[1][2] * m[2][1]) / det;
        result.m[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) / det;
        result.m[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) / det;

        result.m[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) / det;
        result.m[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) / det;
        result.m[1][2] = (m[0][2] * m[1][0] - m[0][0] * m[1][2]) / det;

        result.m[2][0] = (m[1][0] * m[2][1] - m[1][1] * m[2][0]) / det;
        result.m[2][1] = (m[0][1] * m[2][0] - m[0][0] * m[2][1]) / det;
        result.m[2][2] = (m[0][0] * m[1][1] - m[0][1] * m[1][0]) / det;
        
        return result;
    }

    template<typename T>
    Matrix3<T> Matrix3<T>::translate(const Vector2<T>& v) {
        Matrix3<T> result((T)0);
        result[0][0] = (T)1; result[0][1] = (T)0; result[0][2] = v.x;
        result[1][0] = (T)0; result[1][1] = (T)1; result[1][2] = v.y;
        result[2][0] = (T)0; result[2][1] = (T)0; result[2][2] = (T)1;
        return result;
    }

    template<typename T>
    Matrix3<T> Matrix3<T>::rotate(T radians) {
        Matrix3<T> result((T)0);
        T c = static_cast<T>(std::cos(radians));
        T s = static_cast<T>(std::sin(radians));
        result[0][0] =   c;  result[0][1] = -s;  result[0][2] = (T)0;
        result[1][0] =   s;  result[1][1] =  c;  result[1][2] = (T)0;
        result[2][0] = (T)0; result[2][1] = (T)0; result[2][2] = (T)1;
        return result;
    }

    template<typename T>
    Matrix3<T> Matrix3<T>::scale(const Vector2<T>& v) {
        Matrix3<T> result((T)0);
        result[0][0] = v.x;  result[0][1] = (T)0; result[0][2] = (T)0;
        result[1][0] = (T)0; result[1][1] = v.y;  result[1][2] = (T)0;
        result[2][0] = (T)0; result[2][1] = (T)0; result[2][2] = (T)1;
        return result;
    }

    template<typename T>
    Matrix3<T> Matrix3<T>::identity() {
        Matrix3<T> result((T)0);
        result[0][0] = (T)1; result[0][1] = (T)0; result[0][2] = (T)0;
        result[1][0] = (T)0; result[1][1] = (T)1; result[1][2] = (T)0;
        result[2][0] = (T)0; result[2][1] = (T)0; result[2][2] = (T)1;
        return result;
    }

    template<typename T>
    Matrix3<T> Matrix3<T>::operator*(const Matrix3<T>& other) const {
        Matrix3<T> result;
        result[0][0] = m[0][0]*other.m[0][0] + m[0][1]*other.m[1][0] + m[0][2]*other.m[2][0];
        result[0][1] = m[0][0]*other.m[0][1] + m[0][1]*other.m[1][1] + m[0][2]*other.m[2][1];
        result[0][2] = m[0][0]*other.m[0][2] + m[0][1]*other.m[1][2] + m[0][2]*other.m[2][2];

        result[1][0] = m[1][0]*other.m[0][0] + m[1][1]*other.m[1][0] + m[1][2]*other.m[2][0];
        result[1][1] = m[1][0]*other.m[0][1] + m[1][1]*other.m[1][1] + m[1][2]*other.m[2][1];
        result[1][2] = m[1][0]*other.m[0][2] + m[1][1]*other.m[1][2] + m[1][2]*other.m[2][2];

        result[2][0] = m[2][0]*other.m[0][0] + m[2][1]*other.m[1][0] + m[2][2]*other.m[2][0];
        result[2][1] = m[2][0]*other.m[0][1] + m[2][1]*other.m[1][1] + m[2][2]*other.m[2][1];
        result[2][2] = m[2][0]*other.m[0][2] + m[2][1]*other.m[1][2] + m[2][2]*other.m[2][2];

        return result;
    }
}

#endif // TIL_MATRIX3_HPP