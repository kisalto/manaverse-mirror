/*
 *  The ManaVerse Client
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

#include "gui/widgets/tabs/chat/whispertab.h"

#include "being/localplayer.h"

#include "net/chathandler.h"

#include "gui/windows/chatwindow.h"

#include "gui/widgets/windowcontainer.h"

#include "utils/chatutils.h"

#include "debug.h"

WhisperTab::WhisperTab(const Widget2 *const widget,
                       const std::string &caption,
                       const std::string &nick) :
    ChatTab(widget, caption, nick, nick, ChatTabType::WHISPER),
    mNick(nick)
{
    setWhisperTabColors();
}

WhisperTab::~WhisperTab()
{
    if (chatWindow != nullptr)
        chatWindow->removeWhisper(mNick);
}

void WhisperTab::rawSend(const std::string &msg)
{
    chatHandler->privateMessage(mNick, msg);

    // Whispers are not echoed back by the server(s).
    if (localPlayer != nullptr)
        chatLog(localPlayer->getName(), msg);
    else
        chatLog("?", msg);
}

bool WhisperTab::handleCommand(const std::string &restrict type,
                               const std::string &restrict args)
{
    if (type == "close")
    {
        if (windowContainer != nullptr)
            windowContainer->scheduleDelete(this);
        else
            delete this;
        if (chatWindow != nullptr)
            chatWindow->defaultTab();
    }
    else if (type == "me")
    {
        std::string str = textToMe(args);
        chatHandler->privateMessage(mNick, str);
        // Whispers are not echoed back by the server(s).
        if (localPlayer != nullptr)
            chatLog(localPlayer->getName(), str);
        else
            chatLog("?", str);
    }
    else
    {
        return false;
    }

    return true;
}

void WhisperTab::getAutoCompleteList(StringVect &names) const
{
    names.push_back(mNick);
}

void WhisperTab::getAutoCompleteCommands(StringVect& commands) const
{
    commands.push_back("/close");
}

void WhisperTab::setWhisperTabColors()
{
    setTabColors(ThemeColorId::WHISPER_TAB);
}

void WhisperTab::setWhisperTabOfflineColors()
{
    setTabColors(ThemeColorId::WHISPER_TAB_OFFLINE);
}
