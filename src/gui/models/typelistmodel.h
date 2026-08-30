/*
 *  The ManaVerse Client
 *  Copyright (C) 2011-2012  The Mana Developers
 *  Copyright (C) 2012-2020  The ManaPlus Developers
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

#ifndef GUI_MODELS_TYPELISTMODEL_H
#define GUI_MODELS_TYPELISTMODEL_H

#include "gui/models/listmodel.h"

// Yes, I could've swapped TMWA and Evol2 in type list, but Hello=)
// would've screamed at me.
#ifdef TMWA_SUPPORT
#define SERVER_TYPE_INDEX_OFFSET 0
#else   // TMWA_SUPPORT
#define SERVER_TYPE_INDEX_OFFSET 1
#endif  // TMWA_SUPPORT

// No translations here.
const char *SERVER_TYPE_TEXT[] =
{
#ifdef TMWA_SUPPORT
    "TMW Athena",
#endif  // defined(TMWA_SUPPORT)
    "Hercules",
    "Evol2 Hercules",
};

/**
 * Server Type List Model
 */
class TypeListModel final : public ListModel
{
    public:
        TypeListModel() :
            ListModel()
        { }

        A_DELETE_COPY(TypeListModel)

        /**
         * Used to get number of line in the list
         */
        int getNumberOfElements() override final A_WARN_UNUSED
        {
            return NUM_ELEMENTS(SERVER_TYPE_TEXT);
        }

        /**
         * Used to get an element from the list
         */
        std::string getElementAt(int index)
                                 override final A_WARN_UNUSED
        {
            if (0 <= index && index < getNumberOfElements())
                return SERVER_TYPE_TEXT[index];
            return "Unknown";
        }
};

#endif  // GUI_MODELS_TYPELISTMODEL_H
