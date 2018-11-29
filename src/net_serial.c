//
// Copyright(C) 2018 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//     Networking code for sersetup.exe-compatible setup protocol.
//
// Hi! Are you looking at this file this because you're thinking of trying
// to add native serial support to Chocolate Doom? Please don't do that.
// Instead, just use a TCP-to-serial server. This avoids adding the
// complexity of having to deal with system-specific APIs.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "i_system.h"
#include "i_timer.h"
#include "m_misc.h"
#include "net_io.h"
#include "net_packet.h"
#include "net_sdl.h"
#include "net_vanilla.h"

static net_addr_t *remote_addr;

net_context_t *NET_Serial_Connect(char *address)
{
    net_context_t *context;

    context = NET_NewContext();
    net_tcp_module.InitClient();
    NET_AddModule(context, &net_tcp_module);

    remote_addr = NET_ResolveAddress(context, address);
    if (remote_addr == NULL)
    {
        I_Error("NET_Serial_Connect: Failed to resolve address %s",
                address);
    }

    // TCP connection won't be opened until we send something.
    // So make sure we send at least one packet.
    {
        net_packet_t *packet;
        packet = NET_NewPacket(8);
        NET_WriteString(packet, "Hello!");
        NET_SendPacket(remote_addr, packet);
        NET_FreePacket(packet);
    }

    printf("NET_Serial_Connect: Connected to DOSBox modem server at %s.\n",
           NET_AddrToString(remote_addr));

    return context;
}

static void SendSetupPacket(int id, int stage)
{
    net_packet_t *packet;
    char buf[32];
    int i;

    // TODO: This only works for the later version of sersetup.exe; the
    // protocol was changed in later versions. Maybe add support for the
    // older protocol in the future.
    M_snprintf(buf, sizeof(buf), "ID%06d_%d", id, stage);

    packet = NET_NewPacket(16);
    for (i = 0; i < strlen(buf); ++i)
    {
        NET_WriteInt8(packet, buf[i]);
    }
    NET_SendPacket(remote_addr, packet);
    NET_FreePacket(packet);
}

static boolean ProcessSetupPacket(net_packet_t *packet, int *id, int *stage)
{
    char buf[32];

    if (packet->len > sizeof(buf) - 1)
    {
        return false;
    }

    memcpy(buf, packet->data, packet->len);
    buf[packet->len] = '\0';

    return sscanf(buf, "ID%d_%d", id, stage) == 2;
}

void NET_Serial_ArbitrateGame(net_context_t *context,
                              net_vanilla_settings_t *settings)
{
    int id, stage, remote_id, remote_stage;
    net_packet_t *packet;
    net_addr_t *addr;

    id = random() % 1000000;
    stage = 0;
    remote_id = 0;
    remote_stage = 0;

    while (stage < 2)
    {
        while (NET_RecvPacket(context, &addr, &packet))
        {
            if (addr == remote_addr
             && ProcessSetupPacket(packet, &remote_id, &remote_stage))
            {
                stage = remote_stage + 1;
            }
            NET_FreePacket(packet);
        }

        SendSetupPacket(id, stage);
        I_Sleep(1000);
    }

    settings->addrs[0] = remote_addr;
    settings->num_nodes = 1;
    settings->num_players = 2;
    if (id < remote_id)
    {
        settings->consoleplayer = 0;
    }
    else
    {
        settings->consoleplayer = 1;
    }
}

