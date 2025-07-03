#ifndef CGL_NUMERIC_TYPES_HPP
#define CGL_NUMERIC_TYPES_HPP

#include <cstdint>

namespace cgl
{
    using u8 = std::uint8_t;   ///< Unsigned 8-bit integer type.
    using u16 = std::uint16_t; ///< Unsigned 16-bit integer type.
    using u32 = std::uint32_t; ///< Unsigned 32-bit integer type.
    using u64 = std::uint64_t; ///< Unsigned 64-bit integer type.

    using i8 = std::int8_t;    ///< Signed 8-bit integer type.
    using i16 = std::int16_t;  ///< Signed 16-bit integer type.
    using i32 = std::int32_t;  ///< Signed 32-bit integer type.
    using i64 = std::int64_t;  ///< Signed 64-bit integer type.

    using f32 = float;         ///< 32-bit floating point type.
    using f64 = double;        ///< 64-bit floating point type.
}

#endif // CGL_NUMERIC_TYPES_HPP