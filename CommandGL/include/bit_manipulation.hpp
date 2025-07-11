#ifndef CGL_BIT_MANIPULATION_HPP
#define CGL_BIT_MANIPULATION_HPP

namespace cgl
{
    /**
     * @brief Sets a specific bit in an integer to 1.
     * @param value The integer value to modify.
     * @param bit The bit position to set (0-based).
     * @return The modified integer with the specified bit set to 1.
     */
    template<typename T>
    constexpr T setBit(T value, int bit) {
        return value | (static_cast<T>(1) << bit);
    }

    /**
     * @brief Clears a specific bit in an integer (sets it to 0).
     * @param value The integer value to modify.
     * @param bit The bit position to clear (0-based).
     * @return The modified integer with the specified bit cleared.
     */
    template<typename T>
    constexpr T clearBit(T value, int bit) {
        return value & ~(static_cast<T>(1) << bit);
    }

    /**
     * @brief Toggles a specific bit in an integer (flips its value).
     * @param value The integer value to modify.
     * @param bit The bit position to toggle (0-based).
     * @return The modified integer with the specified bit toggled.
     */
    template<typename T>
    constexpr T toggleBit(T value, int bit) {
        return value ^ (static_cast<T>(1) << bit);
    }

    /**
     * @brief Checks if a specific bit in an integer is set (1).
     * @param value The integer value to check.
     * @param bit The bit position to check (0-based).
     * @return True if the specified bit is set, false otherwise.
     */
    template<typename T>
    constexpr bool isBitSet(T value, int bit) {
        return (value & (static_cast<T>(1) << bit)) != 0;
    }

    /**
     * @brief Reverses the bit order of an integer value using all bits of T.
     *        For example, reverseBits(0b1101u) == 0b10110000...0u (for 32-bit unsigned).
     * @param value The integer value whose bits to reverse.
     * @return The integer with its bits reversed.
     */
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