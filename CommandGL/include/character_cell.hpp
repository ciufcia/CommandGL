#ifndef CGL_CHARACTER_CELL_HPP
#define CGL_CHARACTER_CELL_HPP

#include "color.hpp"

namespace cgl
{
    struct CharacterCell
    {
        u32 codepoint = 35;
        Color color = Color{128, 0, 128};
    };
}

#endif // CGL_CHARACTER_CELL_HPP