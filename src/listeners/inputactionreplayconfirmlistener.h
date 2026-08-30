/*
 *  The ManaVerse Client
 *  Copyright (C) 2024-2025  The ManaVerse Developers
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

#ifndef LISTENERS_INPUTACTIONREPLAYCONFIRMLISTENER_H
#define LISTENERS_INPUTACTIONREPLAYCONFIRMLISTENER_H

#include "enums/input/inputaction.h"

#include "listeners/actionlistener.h"

#include "localconsts.h"

class ConfirmDialog;

class InputActionReplayConfirmListener final : public ActionListener
{
    public:
        InputActionReplayConfirmListener();

        A_DELETE_COPY(InputActionReplayConfirmListener)

        void action(const ActionEvent &event) override final;

        void openDialog(const std::string &title,
                        const std::string &text,
                        const InputActionT action0);

    protected:
        ConfirmDialog *mDialog;
        InputActionT mAction;
};

extern InputActionReplayConfirmListener inputActionReplayConfirmListener;

#endif  // LISTENERS_INPUTACTIONREPLAYCONFIRMLISTENER_H
