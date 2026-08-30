/*
 *  The ManaVerse Client
 *  Copyright (C) 2004-2009  The Mana World Development Team
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

#ifndef GUI_MODELS_SORTLISTMODELBUY_H
#define GUI_MODELS_SORTLISTMODELBUY_H

#include "gui/models/listmodel.h"

#include "utils/gettext.h"

static const char *const SORT_NAME_BUY[] =
{
    // TRANSLATORS: buy dialog sort type.
    N_("unsorted"),
    // TRANSLATORS: buy dialog sort type.
    N_("by price"),
    // TRANSLATORS: buy dialog sort type.
    N_("by name"),
    // TRANSLATORS: buy dialog sort type.
    N_("by id"),
    // TRANSLATORS: buy dialog sort type.
    N_("by weight"),
    // TRANSLATORS: inventory sort mode
    N_("by total weight"),
    // TRANSLATORS: buy dialog sort type.
    N_("by amount"),
    // TRANSLATORS: buy dialog sort type.
    N_("by type & slot"),
};

class SortListModelBuy final : public ListModel
{
    public:
        SortListModelBuy() :
            ListModel()
        { }

        A_DELETE_COPY(SortListModelBuy)

        int getNumberOfElements() override final
        { return NUM_ELEMENTS(SORT_NAME_BUY); }

        std::string getElementAt(int i) override final
        {
            if (i >= getNumberOfElements() || i < 0)
                return "???";
            return gettext(SORT_NAME_BUY[i]);
        }
};

#endif  // GUI_MODELS_SORTLISTMODELBUY_H
