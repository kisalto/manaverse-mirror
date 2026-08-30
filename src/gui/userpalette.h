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

#ifndef GUI_USERPALETTE_H
#define GUI_USERPALETTE_H

#include "logger.h"

#include "enums/gui/usercolorid.h"

#include "gui/palette.h"

#include "gui/models/listmodel.h"

/**
 * Class controlling the game's color palette.
 */
class UserPalette final : public Palette, public ListModel
{
    public:
        /**
         * Constructor
         */
        UserPalette();

        A_DELETE_COPY(UserPalette)

        /**
         * Destructor
         */
        ~UserPalette() override final;

        /**
         * Gets the color channel settings associated with the specified type.
         *
         * @param type the color type requested
         *
         * @return channel settings color
         */
        const Color &getColorChannelSettings(const UserColorIdT type)
                                             const A_WARN_UNUSED;

        /**
         * Sets the color channel settings associated with the specified type.
         *
         * @param type the color type requested
         * @param color the new channel settings
         */
        void setColorChannelSettings(const UserColorIdT type,
                                     const Color &color);

        /**
         * Sets the gradient type for the specified color.
         *
         * @param grad gradient type to set
         */
        void setGradient(const UserColorIdT type,
                         const GradientTypeT grad);

        void setGradientDelay(const UserColorIdT type,
                              const int delay)
        { mColors[CAST_SIZE(type)].delay = delay; }

        /**
         * Returns the number of colors known.
         *
         * @return the number of colors known
         */
        inline int getNumberOfElements() override final A_WARN_UNUSED
        { return CAST_S32(mColors.size()); }

        /**
         * Returns the name of the ith color.
         *
         * @param i index of color interested in
         *
         * @return the name of the color
         */
        std::string getElementAt(int i) override final A_WARN_UNUSED;

        /**
         * Commit the current animated color settings
         */
        void commit();

        /**
         * Rollback to the previous animated color settings
         */
        void rollback();

        /**
         * Gets the ColorType used by the color for the element at index i in
         * the current color model.
         *
         * @param i the index of the color
         *
         * @return the color type of the color with the given index
         */
        int getColorTypeAt(const int i) A_WARN_UNUSED;

        /**
         * Gets the color associated with the type. Sets the alpha channel
         * before returning.
         *
         * @param type the color type requested
         * @param alpha alpha channel to use
         *
         * @return the requested color
         */
        inline const Color &getColor(UserColorIdT type,
                                     const unsigned int alpha)
                                     A_WARN_UNUSED
        {
            if (CAST_SIZE(type) >= mColors.size())
            {
                logger->log("incorrect color request type: %d from %u",
                    CAST_S32(type),
                    CAST_U32(mColors.size()));
                type = UserColorId::BEING;
            }
            Color* col = &mColors[CAST_SIZE(type)].color;
            col->a = alpha;
            return *col;
        }

        int getIdByChar(const signed char c, bool &valid) const A_WARN_UNUSED;

        /**
         * Gets the GradientType associated with the specified type.
         *
         * @param type the color type of the color
         *
         * @return the gradient type of the color with the given index
         */
        inline GradientTypeT getGradientType(const UserColorIdT type)
                                             const A_WARN_UNUSED
        { return mColors[CAST_SIZE(type)].grad; }

        /**
         * Gets the gradient delay for the specified type.
         *
         * @param type the color type of the color
         *
         * @return the gradient delay of the color with the given index
         */
        inline int getGradientDelay(const UserColorIdT type)
                                    const A_WARN_UNUSED
        { return mColors[CAST_SIZE(type)].delay; }

        inline const Color &getColorWithAlpha(const UserColorIdT type)
                                              A_WARN_UNUSED
        {
            auto &elem = mColors[CAST_SIZE(type)];
            Color &color = elem.color;
            color.a = CAST_U32(elem.delay);
            return color;
        }

    private:
        /**
         * Define a color replacement.
         *
         * @param i the index of the color to replace
         * @param r red component
         * @param g green component
         * @param b blue component
         */
        void setColorAt(int i, int r, int g, int b);

        void addColor(const UserColorIdT type,
                      const unsigned rgb,
                      GradientTypeT grad,
                      const std::string &text,
                      int delay);

        void addLabel(const UserColorIdT type,
                      const std::string &text);
};

extern UserPalette *userPalette;

#endif  // GUI_USERPALETTE_H
