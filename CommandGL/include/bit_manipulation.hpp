#ifndef CGL_BIT_MANIPULATION_HPP
#define CGL_BIT_MANIPULATION_HPP

namespace cgl
{
    template<typename T>
    constexpr T setBit(T value, int bit) {
        return value | (static_cast<T>(1) << bit);
    }
    template<typename T>
    constexpr T clearBit(T value, int bit) {
        return value & ~(static_cast<T>(1) << bit);
    }
    template<typename T>
    constexpr T toggleBit(T value, int bit) {
        return value ^ (static_cast<T>(1) << bit);
    }
    template<typename T>
    constexpr bool isBitSet(T value, int bit) {
        return (value & (static_cast<T>(1) << bit)) != 0;
    }
    template<typename T>
    constexpr T reverseBits(T value) {
        T result = 0;
        constexpr int numBits = sizeof(T) * 8;
        for (int i = 0; i < numBits; ++i) {
            if ((value >> i) & 1)
                result |= (static_cast<T>(1) << (numBits - 1 - i));
        }
        return result;
    }
}

#endif // CGL_BIT_MANIPULATION_HPP