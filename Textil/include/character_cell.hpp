/**
 * @file character_cell.hpp
 * @brief Character cell structure for terminal text rendering
 * @details Defines the basic unit of terminal display - a character cell containing
 *          a Unicode codepoint and associated color information. This represents
 *          a single character position in the terminal grid.
 */

#ifndef TIL_CHARACTER_CELL_HPP
#define TIL_CHARACTER_CELL_HPP

#include "color.hpp"

namespace til
{
    /**
     * @brief Structure representing a single character cell in terminal display
     * @details A character cell is the fundamental unit of terminal text display,
     *          containing both the character to display and its visual properties.
     *          Terminal displays are organized as a 2D grid of these cells.
     * 
     *          The structure uses Unicode codepoints to support international
     *          text and symbols beyond basic ASCII. Color information allows
     *          for rich visual presentation within terminal constraints.
     * 
     *          Default values create a visible placeholder (hash symbol '#')
     *          in magenta color for easy identification of uninitialized cells.
     */
    struct CharacterCell
    {
        /**
         * @brief Unicode codepoint of the character to display
         * @details Represents the character using Unicode codepoint values.
         *          Defaults to 35 (ASCII '#' - hash/pound symbol) for easy
         *          identification of default/uninitialized cells.
         * 
         *          Common values:
         *          - 32: Space character (invisible)
         *          - 35: Hash symbol '#' (default)
         *          - 65-90: Uppercase letters A-Z
         *          - 97-122: Lowercase letters a-z
         *          - Values > 127: Extended Unicode characters
         */
        u32 codepoint = 35;
        
        /**
         * @brief Color information for character rendering
         * @details Specifies the visual appearance of the character including
         *          foreground color and transparency. The exact interpretation
         *          depends on terminal capabilities and rendering mode.
         * 
         *          Defaults to Color{128, 0, 128} - a medium magenta color
         *          that stands out for debugging uninitialized cells.
         */
        Color color = Color{128, 0, 128};
    };
}

#endif // TIL_CHARACTER_CELL_HPP