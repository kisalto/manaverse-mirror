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

#include "net/packetlimiter.h"

#include "configuration.h"
#include "settings.h"

#include "utils/cast.h"
#include "utils/checkutils.h"
#include "utils/timer.h"

#include <fstream>

#include "debug.h"

// Client-side packet limiter
// The time limits here are all in units of 10ms (SDL timer tick).
// See MILLISECONDS_IN_A_TICK.

struct PacketLimit final
{
    A_DEFAULT_COPY(PacketLimit)

    // timer tick of the first packet in a burst
    int lastTime;
    // number of basic ticks in one packet burst
    int timeLimit;
    // number of packets inside an active packet burst
    int cnt;
    // max number of packets allowed inside a packet burst
    int cntLimit;
};

PacketLimit mPacketLimits[CAST_SIZE(PacketType::PACKET_SIZE) + 1];
// remember to increment this if you (massively) change the above,
// so that the limits file on user's system is properly discarded.
const int PACKET_LIMIT_FILE_VERSION = 4;

void PacketLimiter::initPacketLimiter()
{
    // here i setting packet limits. but current server is broken,
    // and this limits may not help.

    // see also: server-side logic in:
    // TMWA: src/map/clif.cpp (clif_check_packet_flood)
    // Herc: src/map/clif.c (clif_parse)
    //       src/common/socket.c (do_sockets(func_parse))
    //         (herc has no per-packet limits, just a per-connection
    //         fairness system.)

    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_CHAT)].timeLimit = 10 + 5;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_CHAT)].lastTime = 0;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_CHAT)].cntLimit = 1;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_CHAT)].cnt = 0;

    // 10
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_PICKUP)].timeLimit = 10 + 5;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_PICKUP)].lastTime = 0;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_PICKUP)].cntLimit = 1;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_PICKUP)].cnt = 0;

    // 10 5
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_DROP)].timeLimit = 5;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_DROP)].lastTime = 0;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_DROP)].cntLimit = 1;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_DROP)].cnt = 0;

    // 100
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_NPC_NEXT)].timeLimit = 0;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_NPC_NEXT)].lastTime = 0;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_NPC_NEXT)].cntLimit = 1;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_NPC_NEXT)].cnt = 0;

    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_NPC_INPUT)].timeLimit = 100;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_NPC_INPUT)].lastTime = 0;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_NPC_INPUT)].cntLimit = 1;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_NPC_INPUT)].cnt = 0;

    // 50
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_NPC_TALK)].timeLimit = 60;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_NPC_TALK)].lastTime = 0;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_NPC_TALK)].cntLimit = 1;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_NPC_TALK)].cnt = 0;

    // 10
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_EMOTE)].timeLimit = 10 + 5;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_EMOTE)].lastTime = 0;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_EMOTE)].cntLimit = 1;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_EMOTE)].cnt = 0;

    // 100
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_SIT)].timeLimit = 100;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_SIT)].lastTime = 0;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_SIT)].cntLimit = 1;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_SIT)].cnt = 0;

    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_DIRECTION)].timeLimit = 50;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_DIRECTION)].lastTime = 0;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_DIRECTION)].cntLimit = 1;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_DIRECTION)].cnt = 0;

    // 2+
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_ATTACK)].timeLimit = 2 + 10;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_ATTACK)].lastTime = 0;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_ATTACK)].cntLimit = 1;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_ATTACK)].cnt = 0;

    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_STOPATTACK)].timeLimit = 2 + 10;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_STOPATTACK)].lastTime = 0;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_STOPATTACK)].cntLimit = 1;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_STOPATTACK)].cnt = 0;

    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_ONLINELIST)].timeLimit = 1800;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_ONLINELIST)].lastTime = 0;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_ONLINELIST)].cntLimit = 1;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_ONLINELIST)].cnt = 0;

    // 300ms + 50 fix
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_WHISPER)].timeLimit = 30 + 5;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_WHISPER)].lastTime = 0;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_WHISPER)].cntLimit = 1;
    mPacketLimits[CAST_SIZE(
        PacketType::PACKET_WHISPER)].cnt = 0;

    if (!settings.serverConfigDir.empty())
    {
        const std::string packetLimitsName = settings.serverConfigDir
            + "/packetlimiter.txt";

        std::ifstream inPacketFile(packetLimitsName);
        if (!inPacketFile)
        {
            // normally by destructor but not out of scope yet
            inPacketFile.close();
            // wtiting new file
            writePacketLimits(packetLimitsName);
        }
        else
        {   // reading existent file
            std::string line;

            if (!inPacketFile.is_open() || !std::getline(inPacketFile, line))
                return;

            const int fileVersion = atoi(line.c_str());

            for (int f = 0;
                 f < CAST_S32(PacketType::PACKET_SIZE);
                 f ++)
            {
                if (!std::getline(inPacketFile, line))
                    break;

                if (!(fileVersion == 1 &&
                    (static_cast<PacketTypeT>(f) == PacketType::PACKET_DROP ||
                    static_cast<PacketTypeT>(f)
                    == PacketType::PACKET_NPC_NEXT)))
                {
                    mPacketLimits[f].timeLimit = atoi(line.c_str());
                }
            }
            if (fileVersion < PACKET_LIMIT_FILE_VERSION)
                writePacketLimits(packetLimitsName);
        }
    }
}

void PacketLimiter::writePacketLimits(const std::string &packetLimitsName)
{
    std::ofstream outPacketFile;
    outPacketFile.open(packetLimitsName.c_str(), std::ios::out);
    if (!outPacketFile.is_open())
    {
        reportAlways("Error opening file for writing: %s",
            packetLimitsName.c_str())
        outPacketFile.close();
        return;
    }
    outPacketFile << PACKET_LIMIT_FILE_VERSION << std::endl;
    for (int f = 0; f < CAST_S32(PacketType::PACKET_SIZE); f ++)
    {
        outPacketFile << toString(mPacketLimits[f].timeLimit)
                      << std::endl;
    }

    outPacketFile.close();
}

bool PacketLimiter::checkPackets(const PacketTypeT type)
{
    if (type > PacketType::PACKET_SIZE)
        return false;

    if (!serverConfig.getValueBool("enableBuggyServers", true))
        return true;

    const PacketLimit &limit = mPacketLimits[CAST_SIZE(type)];
    const int timeLimit = limit.timeLimit;

    if (timeLimit == 0)
        return true;

    const int time = tick_time;
    const int lastTime = limit.lastTime;
    const int cnt = limit.cnt;
    const int cntLimit = limit.cntLimit;

    if (lastTime > tick_time)
    { // integer wrap-around check
//        instance()->mPacketLimits[type].lastTime = time;
//        instance()->mPacketLimits[type].cnt = 0;
        return true;
    }
    else if (lastTime + timeLimit > time)
    {
        if (cnt >= cntLimit)
        {
            return false;
        }
//        instance()->mPacketLimits[type].cnt ++;
        return true;
    }
//    instance()->mPacketLimits[type].lastTime = time;
//    instance()->mPacketLimits[type].cnt = 1;
    return true;
}

bool PacketLimiter::limitPackets(const PacketTypeT type)
{
    if (CAST_S32(type) < 0 || type > PacketType::PACKET_SIZE)
        return false;

    if (!serverConfig.getValueBool("enableBuggyServers", true))
        return true;

    PacketLimit &pack = mPacketLimits[CAST_SIZE(type)];
    const int timeLimit = pack.timeLimit;

    if (timeLimit == 0)
        return true;

    const int time = tick_time;
    const int lastTime = pack.lastTime;
    const int cnt = pack.cnt;
    const int cntLimit = pack.cntLimit;

    if (lastTime > tick_time)
    { // integer wrap-around check
        pack.lastTime = time;
        pack.cnt = 0;
        return true;
    }
    else if (lastTime + timeLimit > time)
    {
        if (cnt >= cntLimit)
        {
            return false;
        }
        pack.cnt ++;
        return true;
    }
    pack.lastTime = time;
    pack.cnt = 1;
    return true;
}
