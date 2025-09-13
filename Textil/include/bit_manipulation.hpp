/**
 * @file bit_manipulation.hpp
 * @brief Bit manipulation utility functions for Textil library
 * @details Provides templated functions for common bit-level operations including
 *          setting, clearing, toggling, and testing individual bits in integer types.
 *          All functions are constexpr for compile-time evaluation when possible.
 */

#ifndef TIL_BIT_MANIPULATION_HPP
#define TIL_BIT_MANIPULATION_HPP

namespace til
{
    /**
     * @brief Set a specific bit to 1 in an integer value
     * @tparam T Integer type to operate on
     * @param value Original value to modify
     * @param bit Bit position to set (0-based, 0 = least significant bit)
     * @return New value with specified bit set to 1
     * @details Performs bitwise OR operation to set the target bit while preserving other bits.
     *          The operation is: result = value | (1 << bit)
     * 
     *          Example:
     *          ```cpp
     *          uint8_t flags = 0b00000000;
     *          flags = setBit(flags, 3);  // Result: 0b00001000
     *          ```
     */
    template<typename T>
    constexpr T setBit(T value, int bit) {
        return value | (static_cast<T>(1) << bit);
    }
    
    /**
     * @brief Clear a specific bit to 0 in an integer value
     * @tparam T Integer type to operate on
     * @param value Original value to modify
     * @param bit Bit position to clear (0-based, 0 = least significant bit)
     * @return New value with specified bit set to 0
     * @details Performs bitwise AND with inverted mask to clear the target bit while preserving other bits.
     *          The operation is: result = value & ~(1 << bit)
     * 
     *          Example:
     *          ```cpp
     *          uint8_t flags = 0b11111111;
     *          flags = clearBit(flags, 3);  // Result: 0b11110111
     *          ```
     */
    template<typename T>
    constexpr T clearBit(T value, int bit) {
        return value & ~(static_cast<T>(1) << bit);
    }

    /**
     * @brief Toggle a specific bit in an integer value
     * @tparam T Integer type to operate on
     * @param value Original value to modify
     * @param bit Bit position to toggle (0-based, 0 = least significant bit)
     * @return New value with specified bit flipped (0→1 or 1→0)
     * @details Performs bitwise XOR operation to flip the target bit while preserving other bits.
     *          The operation is: result = value ^ (1 << bit)
     * 
     *          Example:
     *          ```cpp
     *          uint8_t flags = 0b00001000;
     *          flags = toggleBit(flags, 3);  // Result: 0b00000000
     *          flags = toggleBit(flags, 5);  // Result: 0b00100000
     *          ```
     */
    template<typename T>
    constexpr T toggleBit(T value, int bit) {
        return value ^ (static_cast<T>(1) << bit);
    }

    /**
     * @brief Test if a specific bit is set (equals 1)
     * @tparam T Integer type to operate on
     * @param value Value to test
     * @param bit Bit position to test (0-based, 0 = least significant bit)
     * @return True if the bit is set (1), false if clear (0)
     * @details Performs bitwise AND with bit mask and tests for non-zero result.
     *          The operation is: result = (value & (1 << bit)) != 0
     * 
     *          Example:
     *          ```cpp
     *          uint8_t flags = 0b00001000;
     *          bool bit3Set = isBitSet(flags, 3);  // Result: true
     *          bool bit2Set = isBitSet(flags, 2);  // Result: false
     *          ```
     */
    template<typename T>
    constexpr bool isBitSet(T value, int bit) {
        return (value & (static_cast<T>(1) << bit)) != 0;
    }
    
    /**
     * @brief Reverse the bit order in an integer value
     * @tparam T Integer type to operate on
     * @param value Value to reverse
     * @return New value with bits in reverse order
     * @details Creates a bit-reversed copy where the most significant bit becomes
     *          the least significant bit and vice versa. Useful for certain algorithms
     *          and data processing operations that require bit-order manipulation.
     * 
     *          Example:
     *          ```cpp
     *          uint8_t value = 0b10110001;
     *          uint8_t reversed = reverseBits(value);  // Result: 0b10001101
     *          ```
     * 
     *          Performance note: This function has O(n) complexity where n is the bit width.
     *          For frequently used values, consider pre-computing and caching results.
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

#endif // TIL_BIT_MANIPULATION_HPP