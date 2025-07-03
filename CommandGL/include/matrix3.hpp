#ifndef CGL_MATRIX3_HPP
#define CGL_MATRIX3_HPP

#include "vector2.hpp"
#include "numeric_types.hpp"

namespace cgl
{
    template<typename T>
    class Matrix3
    {
    public:

        Matrix3() = default;
        Matrix3(T value);
        Matrix3(const T values[9]);

        Vector2<T> operator*(const Vector2<T>& vec) const;
        Matrix3<T> operator*(const Matrix3<T>& other) const;

        static Matrix3<T> Translate(const Vector2<T>& v);
        static Matrix3<T> Rotate(T radians);
        static Matrix3<T> Scale(const Vector2<T>& v);

        T* operator[](std::size_t row) { return m[row]; }
        const T* operator[](std::size_t row) const { return m[row]; }

    private:

        T m[3][3];
    };

    template<typename T>
    Matrix3<T>::Matrix3(T value)
    {
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                m[i][j] = value;
    }

    template<typename T>
    Matrix3<T>::Matrix3(const T values[9])
    {
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                m[i][j] = values[i * 3 + j];
    }

    template<typename T>
    Vector2<T> Matrix3<T>::operator*(const Vector2<T>& vec) const
    {
        return Vector2<T>(
            m[0][0] * vec.x + m[0][1] * vec.y + m[0][2],
            m[1][0] * vec.x + m[1][1] * vec.y + m[1][2]
        );
    }

    template<typename T>
    Matrix3<T> Matrix3<T>::Translate(const Vector2<T>& v)
    {
        Matrix3<T> result((T)0);
        result[0][0] = (T)1; result[0][1] = (T)0; result[0][2] = v.x;
        result[1][0] = (T)0; result[1][1] = (T)1; result[1][2] = v.y;
        result[2][0] = (T)0; result[2][1] = (T)0; result[2][2] = (T)1;
        return result;
    }

    template<typename T>
    Matrix3<T> Matrix3<T>::Rotate(T radians)
    {
        Matrix3<T> result((T)0);
        T c = static_cast<T>(std::cos(radians));
        T s = static_cast<T>(std::sin(radians));
        result[0][0] =   c;  result[0][1] = -s;  result[0][2] = (T)0;
        result[1][0] =   s;  result[1][1] =  c;  result[1][2] = (T)0;
        result[2][0] = (T)0; result[2][1] = (T)0; result[2][2] = (T)1;
        return result;
    }

    template<typename T>
    Matrix3<T> Matrix3<T>::Scale(const Vector2<T>& v)
    {
        Matrix3<T> result((T)0);
        result[0][0] = v.x;  result[0][1] = (T)0; result[0][2] = (T)0;
        result[1][0] = (T)0; result[1][1] = v.y;  result[1][2] = (T)0;
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

#endif // CGL_MATRIX3_HPP