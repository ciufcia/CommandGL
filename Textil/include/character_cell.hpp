#ifndef TIL_CHARACTER_CELL_HPP
#define TIL_CHARACTER_CELL_HPP

#include "color.hpp"

namespace til
{
    struct CharacterCell
    {
        u32 codepoint = 35;
        Color color = Color{128, 0, 128};
    };
}

#endif // TIL_CHARACTER_CELL_HPP