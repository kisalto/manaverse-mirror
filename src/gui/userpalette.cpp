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

#include "gui/userpalette.h"

#include "configuration.h"

#include "utils/foreach.h"
#include "utils/gettext.h"

#include "debug.h"

UserPalette *userPalette = nullptr;


const std::string colorTypeConfigKeys[
    CAST_SIZE(UserColorId::USER_COLOR_LAST)] =
{
    "",
    "ColorBeing",
    "ColorFriend",
    "ColorDisregarded",
    "ColorIgnored",
    "ColorErased",
    "ColorEnemy",
    "ColorPlayer",
    "ColorSelf",
    "ColorGM",
    "ColorNPC",
    "ColorMonster",
    "ColorPet",
    "ColorMercenary",
    "ColorHomunculus",
    "ColorSkillUnit",
    "ColorParty",
    "ColorGuild",
    "ColorTeam1",
    "ColorTeam2",
    "ColorTeam3",
    "",
    "ColorParticle",
    "ColorPickupInfo",
    "ColorExpInfo",
    "",
    "ColorPlayerHp",
    "ColorPlayerHp2",
    "ColorPlayerMp",
    "ColorPlayerMp2",
    "ColorMonsterHp",
    "ColorMonsterHp2",
    "ColorHomunHp",
    "ColorHomunHp2",
    "ColorMercHp",
    "ColorMercHp2",
    "ColorElementalHp",
    "ColorElementalHp2",
    "",
    "ColorHitPlayerMonster",
    "ColorHitMonsterPlayer",
    "ColorHitPlayerPlayer",
    "ColorHitCritical",
    "ColorHitLocalPlayerMonster",
    "ColorHitLocalPlayerCritical",
    "ColorHitLocalPlayerMiss",
    "ColorMiss",
    "",
    "ColorPortalHighlight",
    "ColorCollisionHighlight",
    "ColorCollisionAirHighlight",
    "ColorCollisionWaterHighlight",
    "ColorCollisionMonsterHighlight",
    "ColorCollisionGroundtopHighlight",
    "ColorWalkableTileHighlight",
    "ColorNet",
    "",
    "ColorAttackRange",
    "ColorAttackRangeBorder",
    "ColorMonsterAttackRange",
    "ColorIgnoredMonsterAttackRange",
    "ColorSkillAttackRange",
    "",
    "ColorFloorItemText",
    "ColorHomePlace",
    "ColorHomePlaceBorder",
    "ColorRoadPoint",
};

UserPalette::UserPalette() :
    Palette(CAST_S32(UserColorId::USER_COLOR_LAST)),
    ListModel()
{
    addLabel(UserColorId::LABEL_BEING,
        // TRANSLATORS: palette label
        _("Beings"));
    addColor(UserColorId::BEING,
        0xffffff,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Being"),
        GRADIENT_DELAY);
    addColor(UserColorId::FRIEND,
        0xb0ffb0,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Friend names"),
        GRADIENT_DELAY);
    addColor(UserColorId::DISREGARDED,
        0xa00000,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Disregarded names"),
        GRADIENT_DELAY);
    addColor(UserColorId::IGNORED,
        0xff0000,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Ignored names"),
        GRADIENT_DELAY);
    addColor(UserColorId::ERASED,
        0xff0000,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Erased names"),
        GRADIENT_DELAY);
    addColor(UserColorId::ENEMY,
        0xff4040,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Enemy"),
        GRADIENT_DELAY);
    addColor(UserColorId::PC,
        0xffffff,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Other players names"),
        GRADIENT_DELAY);
    addColor(UserColorId::SELF,
        0xff8040,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Own name"),
        GRADIENT_DELAY);
    addColor(UserColorId::GM,
        0x00ff00,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("GM names"),
        GRADIENT_DELAY);
    addColor(UserColorId::NPC,
        0xc8c8ff,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("NPCs"),
        GRADIENT_DELAY);
    addColor(UserColorId::MONSTER,
        0xff4040,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Monsters"),
        GRADIENT_DELAY);
    addColor(UserColorId::PET,
        0xffffff,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Pets"),
        GRADIENT_DELAY);
    addColor(UserColorId::MERCENARY,
        0xffffff,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Mercenary"),
        GRADIENT_DELAY);
    addColor(UserColorId::HOMUNCULUS,
        0xffffff,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Homunculus"),
        GRADIENT_DELAY);
    addColor(UserColorId::SKILLUNIT,
        0xffffff,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Skill unit"),
        GRADIENT_DELAY);
    addColor(UserColorId::PARTY,
        0xff00d8,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Party members"),
        GRADIENT_DELAY);
    addColor(UserColorId::GUILD,
        0xff00d8,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Guild members"),
        GRADIENT_DELAY);
    addColor(UserColorId::TEAM1,
        0x0000ff,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        strprintf(_("Team %d"), 1),
        GRADIENT_DELAY);
    addColor(UserColorId::TEAM2,
        0x00a020,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        strprintf(_("Team %d"), 2),
        GRADIENT_DELAY);
    addColor(UserColorId::TEAM3,
        0xffff20,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        strprintf(_("Team %d"), 3),
        GRADIENT_DELAY);
    addLabel(UserColorId::LABEL_PARTICLES,
        // TRANSLATORS: palette label
        _("Particles"));
    addColor(UserColorId::PARTICLE,
        0xffffff,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Particle effects"),
        GRADIENT_DELAY);
    addColor(UserColorId::PICKUP_INFO,
        0x28dc28,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Pickup notification"),
        GRADIENT_DELAY);
    addColor(UserColorId::EXP_INFO,
        0xffff00,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Exp notification"),
        GRADIENT_DELAY);
    addLabel(UserColorId::LABEL_HP,
        // TRANSLATORS: palette label
        _("Hp bars"));
    addColor(UserColorId::PLAYER_HP_FG,
        0x00ff00,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Player HP bar"),
        50);
    addColor(UserColorId::PLAYER_HP_BG,
        0xff0000,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Player HP bar (background)"),
        50);
    addColor(UserColorId::PLAYER_MP_FG,
        0x00a0ff,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Player MP bar"),
        50);
    addColor(UserColorId::PLAYER_MP_BG,
        0x303030,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Player MP bar (background)"),
        50);
    addColor(UserColorId::MONSTER_HP_FG,
        0x00ff00,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Monster HP bar"),
        50);
    addColor(UserColorId::MONSTER_HP_BG,
        0xff0000,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Monster HP bar (background)"),
        50);
    addColor(UserColorId::HOMUN_HP_FG,
        0x00ff00,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Homunculus HP bar"),
        50);
    addColor(UserColorId::HOMUN_HP_BG,
        0xff0000,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Homunculus HP bar (background)"),
        50);
    addColor(UserColorId::MERC_HP_FG,
        0x00ff00,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Mercenary HP bar"),
        50);
    addColor(UserColorId::MERC_HP_BG,
        0xff0000,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Mercenary HP bar (background)"),
        50);

    addColor(UserColorId::ELEMENTAL_HP_FG,
        0x00ff00,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Elemental HP bar"),
        50);
    addColor(UserColorId::ELEMENTAL_HP_BG,
        0xff0000,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Elemental HP bar (background)"),
        50);
    addLabel(UserColorId::LABEL_HITS,
        // TRANSLATORS: palette label
        _("Hits"));
    addColor(UserColorId::HIT_PLAYER_MONSTER,
        0x0064ff,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Player hits monster"),
        GRADIENT_DELAY);
    addColor(UserColorId::HIT_MONSTER_PLAYER,
        0xff3232,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Monster hits player"),
        GRADIENT_DELAY);
    addColor(UserColorId::HIT_PLAYER_PLAYER,
        0xff5050,
        GradientType::STATIC,
       // TRANSLATORS: palette color
       _("Other player hits local player"),
        GRADIENT_DELAY);
    addColor(UserColorId::HIT_CRITICAL,
        0xff0000,
        GradientType::RAINBOW,
        // TRANSLATORS: palette color
        _("Critical Hit"),
        GRADIENT_DELAY);
    addColor(UserColorId::HIT_LOCAL_PLAYER_MONSTER,
        0x00ff00,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Local player hits monster"),
        GRADIENT_DELAY);
    addColor(UserColorId::HIT_LOCAL_PLAYER_CRITICAL,
        0xff0000,
        GradientType::RAINBOW,
        // TRANSLATORS: palette color
        _("Local player critical hit"),
        GRADIENT_DELAY);
    addColor(UserColorId::HIT_LOCAL_PLAYER_MISS,
        0x00ffa6,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Local player miss"),
        GRADIENT_DELAY);
    addColor(UserColorId::MISS, 0xffff00,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Misses"),
        GRADIENT_DELAY);
    addLabel(UserColorId::LABEL_TILES,
        // TRANSLATORS: palette label
        _("Tiles"));
    addColor(UserColorId::PORTAL_HIGHLIGHT,
        0xC80000,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Portal highlight"),
        GRADIENT_DELAY);
    addColor(UserColorId::COLLISION_HIGHLIGHT,
        0x0000C8,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Default collision highlight"),
        64);
    addColor(UserColorId::AIR_COLLISION_HIGHLIGHT,
        0xe0e0ff,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Air collision highlight"),
        64);
    addColor(UserColorId::WATER_COLLISION_HIGHLIGHT,
        0x2050e0,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Water collision highlight"),
        64);
    addColor(UserColorId::MONSTER_COLLISION_HIGHLIGHT,
        0x2050e0,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Monster collision highlight"),
        64);
    addColor(UserColorId::GROUNDTOP_COLLISION_HIGHLIGHT,
        0xffff00,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Special ground collision highlight"),
        20);
    addColor(UserColorId::WALKABLE_HIGHLIGHT,
        0x00D000,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Walkable highlight"),
        255);
    addColor(UserColorId::NET,
        0x000000,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Tiles border"), 64);
    addLabel(UserColorId::LABEL_RANGES,
        // TRANSLATORS: palette label
        _("Ranges"));
    addColor(UserColorId::ATTACK_RANGE,
        0xffffff,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Local player attack range"),
        5);
    addColor(UserColorId::ATTACK_RANGE_BORDER,
        0x0,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Local player attack range border"),
        76);
    addColor(UserColorId::MONSTER_ATTACK_RANGE,
        0xff0000,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Monster attack range"),
        20);
    addColor(UserColorId::IGNORED_MONSTER_ATTACK_RANGE,
        0xff00ff,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Ignored monster attack range"),
        15);
    addColor(UserColorId::SKILL_ATTACK_RANGE,
        0x0,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Skill attack range border"),
        76);
    addLabel(UserColorId::LABEL_OTHER,
        // TRANSLATORS: palette label
        _("Other"));
    addColor(UserColorId::FLOOR_ITEM_TEXT,
        0xffffff,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Floor item amount color"),
        100);
    addColor(UserColorId::HOME_PLACE,
        0xffffff,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Home place"),
        20);
    addColor(UserColorId::HOME_PLACE_BORDER,
        0xffff00,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Home place border"),
        200);
    addColor(UserColorId::ROAD_POINT,
        0x000000,
        GradientType::STATIC,
        // TRANSLATORS: palette color
        _("Road point"), 100);
    commit();
}

inline unsigned int bytePack(const Color &color)
{
    return (color.r << 16) |
           (color.g << 8) |
            color.b;
}

UserPalette::~UserPalette()
{
    FOR_EACH (Colors::const_iterator, col, mColors)
    {
        const auto &grad = col->committedGrad;
        if (grad == GradientType::LABEL)
            continue;
        const std::string &configName = colorTypeConfigKeys[col->type];
        config.setValue(configName + "Gradient", CAST_S32(grad));
        config.setValue(configName + "Delay", col->committedDelay);

        if (grad == GradientType::STATIC ||
            grad == GradientType::PULSE)
        {
            char buffer[2 + 2*sizeof(unsigned int) + 1];
            snprintf(buffer, sizeof(buffer), "0x%06x",
                     bytePack(col->committedColorChannelSettings));
            config.setValue(configName, std::string(buffer));
        }
    }
}

const Color &UserPalette::getColorChannelSettings(const UserColorIdT type)
    const
{
    const auto &entry = mColors[CAST_SIZE(type)];
    return entry.colorChannelSettings;
}

void UserPalette::setColorChannelSettings(const UserColorIdT type,
                                          const Color &color)
{
    auto &entry = mColors[CAST_SIZE(type)];
    entry.colorChannelSettings = color;
    // in all other cases, the animation code handles it:
    if (entry.grad == GradientType::STATIC)
        entry.color = color;
}

void UserPalette::setGradient(const UserColorIdT type,
                              const GradientTypeT grad)
{
    ColorElem *const elem = &mColors[CAST_SIZE(type)];

    if (elem->grad != GradientType::STATIC && grad == GradientType::STATIC)
    {
        const size_t sz = mGradVector.size();
        for (size_t i = 0; i < sz; i++)
        {
            if (mGradVector[i] == elem)
            {
                mGradVector.erase(mGradVector.begin() + i);
                break;
            }
        }
    }
    else if (elem->grad == GradientType::STATIC &&
             grad != GradientType::STATIC)
    {
        mGradVector.push_back(elem);
    }

    if (elem->grad != grad)
        elem->grad = grad;
}

std::string UserPalette::getElementAt(int i)
{
    if (i < 0 || i >= getNumberOfElements())
        return "";

    return mColors[i].description;
}

void UserPalette::commit()
{
    FOR_EACH (Colors::iterator, i, mColors)
    {
        i->committedGrad = i->grad;
        i->committedDelay = i->delay;
        i->committedColorChannelSettings = i->colorChannelSettings;
    }
}

void UserPalette::rollback()
{
    FOR_EACH (Colors::iterator, i, mColors)
    {
        if (i->grad != i->committedGrad)
            setGradient(static_cast<UserColorIdT>(i->type), i->committedGrad);

        setGradientDelay(static_cast<UserColorIdT>(i->type),
            i->committedDelay);

        const Color &settings = i->committedColorChannelSettings;
        setColorChannelSettings(
            static_cast<UserColorIdT>(i->type),
            Color(
                settings.r,
                settings.g,
                settings.b,
                255));
    }
}

int UserPalette::getColorTypeAt(const int i)
{
    if (i < 0 || i >= getNumberOfElements())
        return 0;

    return mColors[i].type;
}

void UserPalette::addColor(const UserColorIdT type,
                           const unsigned rgb,
                           GradientTypeT grad,
                           const std::string &description,
                           int delay)
{
    if (CAST_U32(type) >= NUM_ELEMENTS(colorTypeConfigKeys))
        return;

    const std::string &configName = colorTypeConfigKeys[CAST_SIZE(type)];

    static const std::string empty;
    const std::string rgbString = config.getValue(configName, empty);

    unsigned int rgbValue;
    if (sscanf(rgbString.c_str(), "0x%06x", &rgbValue) != 1)
        rgbValue = rgb; // (use default on parse failure)

    const Color &trueCol = Color(rgbValue);
    grad = static_cast<GradientTypeT>(config.getValue(
        configName + "Gradient",
        CAST_S32(grad)));
    delay = config.getValueInt(configName + "Delay", delay);
    ColorElem *elem = &mColors[CAST_SIZE(type)];
    elem->set(CAST_S32(type), trueCol, grad, delay);
    elem->description = description;

    if (grad != GradientType::STATIC)
        mGradVector.push_back(elem);
}

void UserPalette::addLabel(const UserColorIdT type,
                           const std::string &description)
{
    if (CAST_U32(type) >= NUM_ELEMENTS(colorTypeConfigKeys))
        return;


    mColors[CAST_SIZE(type)] = ColorElem();
    // Connected horizontal line (as separator in list)
    const std::string str(" \342\200\225\342\200\225\342\200\225"
        "\342\200\225\342\200\225 ");
    mColors[CAST_SIZE(type)].grad = GradientType::LABEL;
    mColors[CAST_SIZE(type)].description
        = std::string(str).append(description).append(str);
}

int UserPalette::getIdByChar(const signed char c, bool &valid) const
{
    const CharColors::const_iterator it = mCharColors.find(c);
    if (it != mCharColors.end())
    {
        valid = true;
        return (*it).second;
    }

    valid = false;
    return 0;
}
