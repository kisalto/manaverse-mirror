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

#include "input/joystick.h"

#include "configuration.h"
#include "logger.h"
#include "settings.h"
#include "sdlshared.h"

#include "input/inputmanager.h"

#include "utils/foreach.h"
#include "utils/performance.h"
#include "utils/timer.h"

PRAGMA48(GCC diagnostic push)
PRAGMA48(GCC diagnostic ignored "-Wshadow")
#include <SDL.h>
PRAGMA48(GCC diagnostic pop)

#include "debug.h"

Joystick *joystick = nullptr;
int Joystick::joystickCount = 0;
bool Joystick::mEnabled = false;
bool Joystick::mInitialized = false;

Joystick::Joystick(const int no) :
    mDirection(0),
    mHatPosition(0),
    mJoystick(nullptr),
    mTolerance(0),
    mNumber(no >= joystickCount ? joystickCount : no),
    mAxesNumber(MAX_AXES),
    mButtonsNumber(MAX_BUTTONS),
    mUseHatForMovement(true),
    mUseInactive(false),
    mHaveHats(false),
    mKeyToAction(),
    mKeyToId(),
    mKeyTimeMap()
{
    for (int i = 0; i < MAX_AXES; i++)
    {
        mIsTrigger[i] = false;
        mAxesPositions[i] = 0;
    }
    for (int i = 0; i < MAX_BUTTONS; i++)
        mActiveButtons[i] = false;
}

Joystick::~Joystick()
{
    close();
}

void Joystick::init()
{
    // +++ possible to use SDL_EventState with different joystick features.
    SDL_JoystickEventState(SDL_ENABLE);
    mEnabled = config.getBoolValue("joystickEnabled");
    detect();
}

void Joystick::detect()
{
    // close current joystick device
    if (joystick != nullptr)
    {
        delete joystick;
        joystick = nullptr;
    }

    // need to completely reinitialize joystick subsystem
    // because SDL does not support hotplugging
    if (mInitialized)
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    mInitialized = true;

    joystickCount = SDL_NumJoysticks();
    logger->log("%i joysticks/gamepads found", joystickCount);
    for (int i = 0; i < joystickCount; i++)
        logger->log("- %s", SDL_JoystickNameForIndex(i));

    if (joystickCount > 0)
    {
        joystick = new Joystick(config.getIntValue("selectedJoystick"));
        if (mEnabled)
        {
            joystick->open();
            joystick->update();
        }
    }
}

bool Joystick::open()
{
    if (mNumber >= joystickCount)
        mNumber = joystickCount - 1;
    if (mNumber < 0)
    {
        logger->log1("error: incorrect joystick selection");
        return false;
    }
    logger->log("open joystick %d", mNumber);

    mJoystick = SDL_JoystickOpen(mNumber);

    if (mJoystick == nullptr)
    {
        logger->log("Couldn't open joystick: %s", SDL_GetError());
        return false;
    }

    mAxesNumber = SDL_JoystickNumAxes(mJoystick);
    mButtonsNumber = SDL_JoystickNumButtons(mJoystick);

    logger->log("Joystick: %i ", mNumber);
#ifdef USE_SDL2
    logger->log("Name: %s", SDL_JoystickName(mJoystick));
    SDL_JoystickGUID guid = SDL_JoystickGetGUID(mJoystick);
    std::string guidStr;
    for (int f = 0; f < 16; f ++)
        guidStr.append(strprintf("%02x", CAST_U32(guid.data[f])));
    logger->log("Guid: %s", guidStr.c_str());
#if SDL_VERSION_ATLEAST(2, 0, 6)
    logger->log("Device id: %u:%u.%u",
        CAST_U32(SDL_JoystickGetVendor(mJoystick)),
        CAST_U32(SDL_JoystickGetProduct(mJoystick)),
        CAST_U32(SDL_JoystickGetProductVersion(mJoystick)));

    SDL_JoystickType type = SDL_JoystickGetType(mJoystick);
    std::string typeStr;
    switch (type)
    {
        default:
        case SDL_JOYSTICK_TYPE_UNKNOWN:
            typeStr = "unknown";
            break;
        case SDL_JOYSTICK_TYPE_GAMECONTROLLER:
            typeStr = "game controller";
            break;
        case SDL_JOYSTICK_TYPE_WHEEL:
            typeStr = "wheel";
            break;
        case SDL_JOYSTICK_TYPE_ARCADE_STICK:
            typeStr = "arcade stick";
            break;
        case SDL_JOYSTICK_TYPE_FLIGHT_STICK:
            typeStr = "flight stick";
            break;
        case SDL_JOYSTICK_TYPE_DANCE_PAD:
            typeStr = "dance pad";
            break;
        case SDL_JOYSTICK_TYPE_GUITAR:
            typeStr = "guitar";
            break;
        case SDL_JOYSTICK_TYPE_DRUM_KIT:
            typeStr = "drum kit";
            break;
        case SDL_JOYSTICK_TYPE_ARCADE_PAD:
            typeStr = "arcade pad";
            break;
        case SDL_JOYSTICK_TYPE_THROTTLE:
            typeStr = "throttle";
            break;
    }
    logger->log("Type: " + typeStr);
#endif  // SDL_VERSION_ATLEAST(2, 0, 6)
    // probably need aslo dump SDL_JoystickCurrentPowerLevel
#else  // USE_SDL2

    logger->log("Name: %s", SDL_JoystickName(mNumber));
#endif  // USE_SDL2

    logger->log("Axes: %i ", mAxesNumber);
    logger->log("Balls: %i", SDL_JoystickNumBalls(mJoystick));
    logger->log("Hats: %i", SDL_JoystickNumHats(mJoystick));
    logger->log("Buttons: %i", mButtonsNumber);

    mHaveHats = (SDL_JoystickNumHats(mJoystick) > 0);

    if (mAxesNumber > MAX_AXES)
        mAxesNumber = mAxesNumber;

    if (mButtonsNumber > MAX_BUTTONS)
        mButtonsNumber = MAX_BUTTONS;

    mTolerance = config.getFloatValue("joystickTolerance");
    mUseHatForMovement = config.getBoolValue("useHatForMovement");
    mUseInactive = config.getBoolValue("useInactiveJoystick");

    for (int i = 0; i < mAxesNumber; i++)
    {
        // heuristic to detect controller triggers.
        // their resting position is at SDL_JOYSTICK_AXIS_MIN = -32768
        if (SDL_JoystickGetAxis(mJoystick, i) == SDL_JOYSTICK_AXIS_MIN)
            mIsTrigger[i] = true;
    }

    return true;
}

void Joystick::close()
{
    logger->log("close joystick %d", mNumber);
    if (mJoystick != nullptr)
    {
        SDL_JoystickClose(mJoystick);
        mJoystick = nullptr;
    }
}

void Joystick::setNumber(const int n)
{
    if (mJoystick != nullptr)
    {
        SDL_JoystickClose(mJoystick);
        mNumber = n;
        open();
    }
    else
    {
        mNumber = n;
    }
}

void Joystick::logic()
{
    BLOCK_START("Joystick::logic")
    if (!mEnabled)
    {
        BLOCK_END("Joystick::logic")
        return;
    }

    mDirection = 0;

    if (mUseInactive ||
        settings.inputFocused != KeyboardFocus::Unfocused)
    {
        for (int i = 0; i < mAxesNumber; i++)
        {
            int position = SDL_JoystickGetAxis(mJoystick, i);
            // transform range [SDL_JOYSTICK_AXIS_MIN, SDL_JOYSTICK_AXIS_MAX]
            // to [0, SDL_JOYSTICK_AXIS_MAX]
            if (mIsTrigger[i])
                position = (position - SDL_JOYSTICK_AXIS_MIN) / 2;
            mAxesPositions[i] = position;
        }

        // X-Axis
        int position = mAxesPositions[0];
        if (position >= mTolerance * SDL_JOYSTICK_AXIS_MAX)
            mDirection |= RIGHT;
        else if (position <= mTolerance * SDL_JOYSTICK_AXIS_MIN)
            mDirection |= LEFT;

        // Y-Axis
        position = mAxesPositions[1];
        if (position <= mTolerance * SDL_JOYSTICK_AXIS_MIN)
            mDirection |= UP;
        else if (position >= mTolerance * SDL_JOYSTICK_AXIS_MAX)
            mDirection |= DOWN;

#ifdef DEBUG_JOYSTICK
        if (SDL_JoystickGetAxis(mJoystick, 2))
            logger->log("axis 2 pos: %d", SDL_JoystickGetAxis(mJoystick, 2));
        if (SDL_JoystickGetAxis(mJoystick, 3))
            logger->log("axis 3 pos: %d", SDL_JoystickGetAxis(mJoystick, 3));
        if (SDL_JoystickGetAxis(mJoystick, 4))
            logger->log("axis 4 pos: %d", SDL_JoystickGetAxis(mJoystick, 4));
#endif  // DEBUG_JOYSTICK

        if (mHaveHats)
        {
            // reading only hat 0
            const uint8_t hat = SDL_JoystickGetHat(mJoystick, 0);
            mHatPosition = 0;
            if ((hat & SDL_HAT_RIGHT) != 0)
                mHatPosition |= RIGHT;
            else if ((hat & SDL_HAT_LEFT) != 0)
                mHatPosition |= LEFT;
            if ((hat & SDL_HAT_UP) != 0)
                mHatPosition |= UP;
            else if ((hat & SDL_HAT_DOWN) != 0)
                mHatPosition |= DOWN;
            if ((mDirection == 0U) && mUseHatForMovement)
                mDirection = mHatPosition;
        }

        // Buttons
        for (int i = 0; i < mButtonsNumber; i++)
        {
            const bool state = (SDL_JoystickGetButton(mJoystick, i) == 1);
            mActiveButtons[i] = state;
            if (!state)
                resetRepeat(i);
#ifdef DEBUG_JOYSTICK
            if (mActiveButtons[i])
                logger->log("button: %d", i);
#endif  // DEBUG_JOYSTICK
        }
    }
    else
    {
        mHatPosition = 0;
        for (int i = 0; i < mAxesNumber; i++)
            mAxesPositions[i] = 0;
        for (int i = 0; i < mButtonsNumber; i++)
            mActiveButtons[i] = false;
    }
    BLOCK_END("Joystick::logic")
}

bool Joystick::buttonPressed(const int no) const
{
    if (!mEnabled)
        return false;
    const int button = getButton(no);
    if (button >= 0)
    {
        if (button < mButtonsNumber)
            return mActiveButtons[button];
        if (button == KEY_UP)
            return (mHatPosition & UP) != 0;
        if (button == KEY_DOWN)
            return (mHatPosition & DOWN) != 0;
        if (button == KEY_LEFT)
            return (mHatPosition & LEFT) != 0;
        if (button == KEY_RIGHT)
            return (mHatPosition & RIGHT) != 0;
    }
    const int naxis = getNegativeAxis(no);
    if (naxis >= 0)
        return mAxesPositions[naxis] < mTolerance * SDL_JOYSTICK_AXIS_MIN;

    const int paxis = getPositiveAxis(no);
    if (paxis >= 0)
        return mAxesPositions[paxis] > mTolerance * SDL_JOYSTICK_AXIS_MAX;

    return false;
}

void Joystick::getNames(STD_VECTOR <std::string> &names)
{
    names.clear();
    for (int i = 0; i < joystickCount; i++)
        names.push_back(SDL_JoystickNameForIndex(i));
}

void Joystick::update()
{
    inputManager.updateKeyActionMap(mKeyToAction, mKeyToId,
        mKeyTimeMap, InputType::JOYSTICK);
}

bool Joystick::isActionEvent(const SDL_Event &event)
{
    return getButtonFromEvent(event) >= 0;
}

KeysVector *Joystick::getActionVector(const SDL_Event &event)
{
    const int i = getButtonFromEvent(event);
    return getActionVectorByKey(i);
}

KeysVector *Joystick::getActionVectorByKey(const int i)
{
    if (getButton(i) < 0
        && getNegativeAxis(i) < 0
        && getPositiveAxis(i) < 0)
    {
        return nullptr;
    }
//    logger->log("button triggerAction: %d", i);

    KeyToActionMapIter it = mKeyToAction.find(i);
    if (it != mKeyToAction.end())
        return &it->second;

    return nullptr;
}

InputActionT Joystick::getActionId(const SDL_Event &event)
{
    const int i = getButtonFromEvent(event);
    if (i < 0)
        return InputAction::NO_VALUE;

    KeyToIdMapIter it = mKeyToId.find(i);
    if (it != mKeyToId.end())
        return it->second;

    return InputAction::NO_VALUE;
}

int Joystick::getButtonFromEvent(const SDL_Event &event) const
{
    if (event.type == SDL_JOYBUTTONDOWN)
    {
        if (event.jbutton.which != mNumber)
            return -1;
        return event.jbutton.button;
    }
    if (!mUseHatForMovement && event.type == SDL_JOYHATMOTION)
    {
        // reading only hat 0
        if (event.jhat.which != mNumber || event.jhat.hat != 0)
            return -1;
        const int value = event.jhat.value;
        // SDL reports new hat position, not when d-pad button is pressed.
        // because of that we have to compare it to previously known state.
        if ((mHatPosition & UP) == 0 && (value & SDL_HAT_UP) != 0)
            return KEY_UP;
        if ((mHatPosition & DOWN) == 0 && (value & SDL_HAT_DOWN) != 0)
            return KEY_DOWN;
        if ((mHatPosition & LEFT) == 0 && (value & SDL_HAT_LEFT) != 0)
            return KEY_LEFT;
        if ((mHatPosition & RIGHT) == 0 && (value & SDL_HAT_RIGHT) != 0)
            return KEY_RIGHT;
    }
    if (event.type == SDL_JOYAXISMOTION)
    {
        if (event.jaxis.which != mNumber)
            return -1;
        const int axis = event.jaxis.axis;
        if (axis < RESERVED_AXES)
            return -1;
        int position = event.jaxis.value;
        // transform range [SDL_JOYSTICK_AXIS_MIN, SDL_JOYSTICK_AXIS_MAX]
        // to [0, SDL_JOYSTICK_AXIS_MAX]
        if (mIsTrigger[axis])
            position = (position - SDL_JOYSTICK_AXIS_MIN) / 2;
        if (position < mTolerance * SDL_JOYSTICK_AXIS_MIN
            && mAxesPositions[axis] > mTolerance * SDL_JOYSTICK_AXIS_MIN)
            return KEY_NEGATIVE_AXIS_FIRST + axis;
        if (position > mTolerance * SDL_JOYSTICK_AXIS_MAX
            && mAxesPositions[axis] < mTolerance * SDL_JOYSTICK_AXIS_MAX)
            return KEY_POSITIVE_AXIS_FIRST + axis;
    }
    return -1;
}

int Joystick::getButton(const int key) const
{
    if (key < 0
        || (mButtonsNumber <= key && key < MAX_BUTTONS)
        || (mUseHatForMovement && key >= MAX_BUTTONS)
        || key >= KEY_NEGATIVE_AXIS_FIRST)
    {
        return -1;
    }
    return key;
}

int Joystick::getNegativeAxis(const int key) const
{
    if (key < KEY_NEGATIVE_AXIS_FIRST || key >= KEY_POSITIVE_AXIS_FIRST)
        return -1;

    const int axis = key - KEY_NEGATIVE_AXIS_FIRST;
    if (axis < RESERVED_AXES || axis >= mAxesNumber)
        return -1;

    return axis;
}

int Joystick::getPositiveAxis(const int key) const
{
    if (key < KEY_POSITIVE_AXIS_FIRST || key >= KEY_END)
        return -1;

    const int axis = key - KEY_POSITIVE_AXIS_FIRST;
    if (axis < RESERVED_AXES || axis >= mAxesNumber)
        return -1;

    return axis;
}

bool Joystick::isActionActive(const InputActionT index) const
{
    if (!validate())
        return false;

    const InputFunction &key = inputManager.getKey(index);
    for (size_t i = 0; i < inputFunctionSize; i ++)
    {
        const InputItem &val = key.values[i];
        if (val.type != InputType::JOYSTICK)
            continue;
        return buttonPressed(val.value);
    }
    return false;
}

bool Joystick::validate() const
{
    if (!mEnabled)
        return false;

    return mUseInactive ||
        settings.inputFocused != KeyboardFocus::Unfocused;
}

void Joystick::handleRepeat(const int time)
{
    BLOCK_START("Joystick::handleRepeat")
    FOR_EACH (KeyTimeMapIter, it, mKeyTimeMap)
    {
        const int key = (*it).first;
        if (buttonPressed(key))
        {
            int &keyTime = (*it).second;
            if (time > keyTime && abs(time - keyTime)
                > SDL_DEFAULT_REPEAT_DELAY * 10)
            {
                keyTime = time;
                inputManager.triggerAction(getActionVectorByKey(key));
            }
        }
    }
    BLOCK_END("Joystick::handleRepeat")
}

void Joystick::resetRepeat(const int key)
{
    const KeyTimeMapIter it = mKeyTimeMap.find(key);
    if (it != mKeyTimeMap.end())
        (*it).second = tick_time;
}
