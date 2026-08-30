/*
 *  The ManaVerse Client
 *  Copyright (C) 2008  Douglas Boffey <dougaboffey@netscape.net>
 *  Copyright (C) 2009  The Mana World Development Team
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

#include "gui/palette.h"

#include "utils/foreach.h"
#include "utils/timer.h"

#ifndef USE_SDL2
#include <cmath>
#endif  // USE_SDL2

#include "debug.h"

// required by MSYS2 / Windows
#ifndef M_PI
#define M_PI    3.1415926535897932384626433832795
#endif

const Color Palette::BLACK = Color(0, 0, 0, 255);
Palette::Palettes Palette::mInstances;

const Color Palette::RAINBOW_COLORS[] =
{
    Color(255, 0, 0, 255),
    Color(255, 153, 0, 255),
    Color(255, 255, 0, 255),
    Color(0, 153, 0, 255),
    Color(0, 204, 204, 255),
    Color(51, 0, 153, 255),
    Color(153, 0, 153, 255)
};

Palette::Palette(const int size) :
    mRainbowTime(tick_time),
    mColors(Colors(size)),
    mCharColors(),
    mGradVector()
{
    mInstances.insert(this);
}

Palette::~Palette()
{
    mInstances.erase(this);
}

const Color& Palette::getCharColor(const signed char c, bool &valid) const
{
    const CharColors::const_iterator it = mCharColors.find(c);
    if (it != mCharColors.end())
    {
        valid = true;
        return mColors[(*it).second].color;
    }

    valid = false;
    return BLACK;
}

void Palette::advanceGradients()
{
    FOR_EACH (Palettes::const_iterator, it, mInstances)
        (*it)->advanceGradient();
}

void Palette::advanceGradient()
{
    const int time = get_elapsed_time(mRainbowTime);
    if (time > 5)
    {
        // For slower systems, advance can be greater than one (advance > 1
        // skips advance-1 steps). Should make gradient look the same
        // independent of the framerate.
        const int advance = time / 5;

        for (size_t i = 0, sz = mGradVector.size(); i < sz; i++)
        {
            ColorElem *const elem A_NONNULLPOINTER = mGradVector[i];
            if (elem == nullptr)
                continue;

            int delay = elem->delay;
            const GradientTypeT &grad = elem->grad;

            if (grad == GradientType::PULSE)
                delay = delay / 20;

            const int numOfColors
                = grad == GradientType::SPECTRUM ? 6
                : grad == GradientType::PULSE ? 127
                : NUM_ELEMENTS(RAINBOW_COLORS);

            // We cannot losslessly decrease advance in integers, as it is
            // a small number, but we can "stretch" the duration of each
            // colour by multiplying the number of colours with the delay:
            //
            // Imagine a ruler that goes from 0 to N for each of the N
            // colours. If you zoom in, you will see *delay* subdivisions
            // between two units of this ruler. gradientIndex counts these
            // subdivisions from the start of ruler. To get the unit number
            // (color index) we have to divide this count with *delay*
            // and round it down (integer division does this
            // for us). Local subdivision number is then the remainder.
            elem->gradientIndex = (elem->gradientIndex + advance)
                % (delay * numOfColors);

            const int gradIndex = elem->gradientIndex;
            // Index of animated color (if applicable)
            const int colorIndex = gradIndex / delay;
            const int subdivision = gradIndex % delay;

            Color &color = elem->color;

            if (grad == GradientType::PULSE)
            {
                // pulsating from 0 to channel settings and back
                const int amplitude
                    = CAST_S32(255.0 * sin(M_PI * colorIndex / numOfColors));

                const Color &settings = elem->colorChannelSettings;

                color.r = ((amplitude * settings.r) / 255);
                color.g = ((amplitude * settings.g) / 255);
                color.b = ((amplitude * settings.b) / 255);
            }
            else if (grad == GradientType::SPECTRUM)
            {
                // On every even colorIndex, one color fades in
                // and on every odd one, another colour fades out:
                const int relSub // relative subdivision
                    = ((colorIndex % 2) == 0)
                    ? (delay - subdivision)  // falling curve
                    : subdivision;           // rising curve
                const int amplitude
                    = CAST_S32(255.0 * (cos(M_PI * relSub / delay) + 1) / 2);

                /*
                 * colorIndex:  0   1   2   3   4   5
                 * red:        max  \   0   0   /  max
                 * green:       /  max max  \   0   0
                 * blue:        0   0   /  max max  \
                */
                color.r = (colorIndex == 0 || colorIndex == 5) ? 255
                        : (colorIndex == 1 || colorIndex == 4) ? amplitude
                        : 0;
                color.g = (colorIndex == 1 || colorIndex == 2) ? 255
                        : (colorIndex == 0 || colorIndex == 3) ? amplitude
                        : 0;
                color.b = (colorIndex == 3 || colorIndex == 4) ? 255
                        : (colorIndex == 2 || colorIndex == 5) ? amplitude
                        : 0;
            }
            else if (grad == GradientType::RAINBOW)
            {
                const Color &startColor = RAINBOW_COLORS[colorIndex];
                const Color &destColor = RAINBOW_COLORS
                    [(colorIndex + 1) % NUM_ELEMENTS(RAINBOW_COLORS)];

                // use a 0..1 wave for smooth transition between two
                // colours in the list. (start/dest amplitude)
                const double sAmp = (cos(M_PI * subdivision / delay) + 1) / 2;
                const double dAmp = 1 - sAmp;

                color.r = CAST_S32(sAmp * startColor.r + dAmp * destColor.r);
                color.g = CAST_S32(sAmp * startColor.g + dAmp * destColor.g);
                color.b = CAST_S32(sAmp * startColor.b + dAmp * destColor.b);
            }
        }

        mRainbowTime = tick_time;
    }
}
