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
}

#endif // CGL_BIT_MANIPULATION_HPP