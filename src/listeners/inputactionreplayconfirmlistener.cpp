/*
 *  The ManaVerse Client
 *  Copyright (C) 2024-2025 The ManaVerse Developers
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

#include "listeners/inputactionreplayconfirmlistener.h"

#include "gui/windows/confirmdialog.h"

#include "gui/widgets/createwidget.h"

#include "input/inputmanager.h"

#include "const/sound.h"

#include "debug.h"

InputActionReplayConfirmListener inputActionReplayConfirmListener;

InputActionReplayConfirmListener::InputActionReplayConfirmListener() :
    ActionListener(),
    mDialog(nullptr),
    mAction(InputAction::NO_VALUE)
{
}

void InputActionReplayConfirmListener::openDialog(
    const std::string &title,
    const std::string &text,
    const InputActionT action0)
{
    CREATEWIDGETV(mDialog, ConfirmDialog,
                  title, text,
                  SOUND_REQUEST,
                  false,
                  Modal_true,
                  nullptr);

    mDialog->addActionListener(this);
    mAction = action0;
}

void InputActionReplayConfirmListener::action(const ActionEvent &event)
{
    if (mDialog != nullptr)
    {
        const std::string &eventId = event.getId();
        inputManager.executeChatCommand(mAction, eventId, nullptr);
        // the dialog deletes itself with scheduleDelete()
        mDialog = nullptr;
    }
    mAction = InputAction::NO_VALUE;
}
