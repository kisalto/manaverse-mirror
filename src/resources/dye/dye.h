/*
 *  The ManaVerse Client
 *  Copyright (C) 2007-2009  The Mana World Development Team
 *  Copyright (C) 2009-2010  The Mana Developers
 *  Copyright (C) 2011-2020  The ManaPlus Developers
 *  Copyright (C) 2020-2025  The ManaVerse Developers
 *
 *  This file is part of The ManaVerse Client.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RESOURCES_DYE_DYE_H
#define RESOURCES_DYE_DYE_H

#include <string>

#include "localconsts.h"

class DyePalette;

const int dyePalateSize = 9;
const int sPaletteIndex = 7;
const int aPaletteIndex = 8;

/**
 * Class for dispatching pixel-recoloring amongst several palettes.
 */
class Dye final
{
    public:
        /**
         * Creates a set of palettes based on the given string.
         *
         * The parts of string are separated by semi-colons. Each part starts
         * by an uppercase letter, followed by a colon and then a palette name.
         */
        explicit Dye(const std::string &restrict dye);

        A_DELETE_COPY(Dye)

        /**
         * Destroys the associated palettes.
         */
        ~Dye();

        /**
         * Fills the blank in a dye placeholder with some palette names.
         */
        static void instantiate(std::string &restrict target,
                                const std::string &restrict palettes);

        /**
         * Return special dye palette (S)
         */
        const DyePalette *getSPalette() const restrict2 noexcept2 A_WARN_UNUSED
        { return mDyePalettes[sPaletteIndex]; }

        /**
         * Return special dye palette (A)
         */
        const DyePalette *getAPalette() const restrict2 noexcept2 A_WARN_UNUSED
        { return mDyePalettes[aPaletteIndex]; }

        /**
         * Return dye type for S - 1, for A - 2, 0 for other
         */
        int getType() const restrict2 noexcept2 A_WARN_UNUSED;

        void normalDye(uint32_t *restrict pixels,
                       const int bufSize) const restrict2;

        void normalOGLDye(uint32_t *restrict pixels,
                          const int bufSize) const restrict2;

    private:
        /**
         * The order of the palettes, as well as their uppercase letter, is:
         *
         * Red, Green, Yellow, Blue, Magenta, White (or rather gray), Simple.
         */
        DyePalette *restrict mDyePalettes[dyePalateSize];
};

#endif  // RESOURCES_DYE_DYE_H
