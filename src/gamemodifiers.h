/*
 *  The ManaVerse Client
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

#ifndef GAMEMODIFIERS_H
#define GAMEMODIFIERS_H

#include <string>

#include "localconsts.h"

#define declModifier(typeName) \
    static void change##typeName(const bool forward); \
    static std::string get##typeName##String(); \
    static const unsigned m##typeName##Size; \
    static const char *const m##typeName##Strings[];

class GameModifiers final
{
    public:
        A_DELETE_COPY(GameModifiers)

        /*
         * Load most modifiers from config.
         */
        static void init();

        /*
         * Cycle through available modifier settings
         *
         * Assumes that settings start at 0 and end at limit-1 without holes.
         *
         * @param var        the variable that stores the current settings
         * @param first      first valid setting (used when limit is reached)
         * @param limit      how many settings there are (for looping around)
         * @param configKey  name of this setting in config
         * @param func       obtain human readable representation of the next setting
         *                   used to print new setting state into debug log.
         * @param save       toggles whether to save changed setting
         * @param forward    direction of cycling
        */
        static void changeMode(unsigned *restrict const var,
                               const unsigned first,
                               const unsigned limit,
                               const char *restrict const configKey,
                               std::string (*const func)(),
                               const bool save,
                               const bool forward);

        declModifier(MoveType)
        declModifier(CrazyMoveType)
        declModifier(MoveToTargetType)
        declModifier(FollowMode)
        declModifier(AttackWeaponType)
        declModifier(AttackType)
        declModifier(TargetingType)
        declModifier(QuickDropCounter)
        declModifier(PickUpType)
        declModifier(MagicAttackType)
        declModifier(PvpAttackType)
        declModifier(ImitationMode)
        declModifier(GameModifiers)
        declModifier(MapDrawType)
        declModifier(CameraMode)
        declModifier(AwayMode)

        static void setQuickDropCounter(const int n);

        /*
         * Reset modifiers to their default values
         */
        static void resetModifiers();

    protected:
        /*
         * Return untranslated human representation of a setting's state
         *
         * @param     lut    look up table with strings before translation
         * @param     index  setting's value
         * @param     size   upper limit on setting's value (index of top
         *                   element reserved for unknown/error string)
         */
        static const char *getHumanString(const char *const *const lut,
                                       const unsigned index,
                                       const unsigned size)
                                       A_WARN_UNUSED A_NONNULL(1);
};

#undef declModifier

#endif  // GAMEMODIFIERS_H
