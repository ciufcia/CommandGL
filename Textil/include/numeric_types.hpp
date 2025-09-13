/**
 * @file numeric_types.hpp
 * @brief Type aliases for common numeric types used throughout the Textil library
 * @details This header provides convenient type aliases for standard integer and floating-point types,
 *          following a consistent naming convention that improves code readability and portability.
 */

#ifndef TIL_NUMERIC_TYPES_HPP
#define TIL_NUMERIC_TYPES_HPP

#include <cstdint>

namespace til
{
    /**
     * @brief Unsigned 8-bit integer type
     * @details Alias for std::uint8_t. Commonly used for byte values, color components (0-255), 
     *          and small non-negative integers where memory efficiency is important.
     */
    using u8 = std::uint8_t;
    
    /**
     * @brief Unsigned 16-bit integer type
     * @details Alias for std::uint16_t. Suitable for larger non-negative values up to 65,535,
     *          often used for dimensions, indices, and counters.
     */
    using u16 = std::uint16_t;
    
    /**
     * @brief Unsigned 32-bit integer type
     * @details Alias for std::uint32_t. The most commonly used unsigned integer type for
     *          general-purpose non-negative values, array sizes, and pixel coordinates.
     */
    using u32 = std::uint32_t;
    
    /**
     * @brief Unsigned 64-bit integer type
     * @details Alias for std::uint64_t. Used for very large non-negative values,
     *          memory addresses, and high-precision timing operations.
     */
    using u64 = std::uint64_t;

    /**
     * @brief Signed 8-bit integer type
     * @details Alias for std::int8_t. Range from -128 to 127. Used for small signed values
     *          where memory efficiency is critical.
     */
    using i8 = std::int8_t;
    
    /**
     * @brief Signed 16-bit integer type
     * @details Alias for std::int16_t. Range from -32,768 to 32,767. Suitable for 
     *          medium-range signed values and coordinates.
     */
    using i16 = std::int16_t;
    
    /**
     * @brief Signed 32-bit integer type
     * @details Alias for std::int32_t. The most commonly used signed integer type for
     *          general-purpose values, coordinates, and calculations requiring negative values.
     */
    using i32 = std::int32_t;
    
    /**
     * @brief Signed 64-bit integer type
     * @details Alias for std::int64_t. Used for very large signed values,
     *          high-precision calculations, and time measurements.
     */
    using i64 = std::int64_t;

    /**
     * @brief Single-precision floating-point type
     * @details Alias for float. 32-bit IEEE 754 floating-point number with approximately
     *          7 decimal digits of precision. The standard choice for graphics coordinates,
     *          transformations, and general mathematical calculations in Textil.
     */
    using f32 = float;
    
    /**
     * @brief Double-precision floating-point type
     * @details Alias for double. 64-bit IEEE 754 floating-point number with approximately
     *          15-17 decimal digits of precision. Used when higher precision is required
     *          for mathematical operations or accumulated calculations.
     */
    using f64 = double;
}

#endif // TIL_NUMERIC_TYPES_HPP