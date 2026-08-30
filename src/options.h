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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdint.h> // see commit's discussion
#include <string>

#include "localconsts.h"

/**
 * A structure holding the values of various options that can be passed
 * from the command line.
 */
struct Options final
{
    Options() :
        username(),
        password(),
        character(),
        brandingPath(),
        updateHost(),
        dataPath(),
        homeDir(),
        logFileName(),
        chatLogDir(),
        configDir(),
        localDataDir(),
        screenshotDir(),
        test(),
        serverName(),
        serverType(),
        renderer(-1),
        serverPort(0),
        printHelp(false),
        printVersion(false),
        skipUpdate(false),
        chooseDefault(false),
        noOpenGL(false),
        safeMode(false),
        testMode(false),
        ipc(false),
        hideCursor(false),
        error(false),
        validate(false),
        uniqueSession(false)
    {}

    A_DELETE_COPY(Options)

    std::string username;
    std::string password;
    std::string character;
    std::string brandingPath;
    std::string updateHost;
    std::string dataPath;
    std::string homeDir;
    std::string logFileName;
    std::string chatLogDir;
    std::string configDir;
    std::string localDataDir;
    std::string screenshotDir;
    std::string test;
    std::string serverName;
    std::string serverType;
    int renderer;
    uint16_t serverPort;
    bool printHelp;
    bool printVersion;
    bool skipUpdate;
    bool chooseDefault;
    bool noOpenGL;
    bool safeMode;
    bool testMode;
    bool ipc;
    bool hideCursor;
    bool error;
    bool validate;
    bool uniqueSession;
};

#endif  // OPTIONS_H
