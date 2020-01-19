//  ===== Date Created: 19 January, 2020 ===== 

#include <stdio.h>
#include <iostream>

#include <enet/enet.h>

int main()
{
	// Initialise ENet
	if (enet_initialize() != 0)
	{
		printf("Error: Init failed.\n");
		return -1;
	}
	atexit(enet_deinitialize);
	
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = 1234;

	// Create server
	ENetHost* server = enet_host_create(&address, 32, 2, 0, 0);
	if (!server)
	{
		printf("Error: Server creation failed.\n");
		return -1;
	}

	// Listen for packets
	ENetEvent netEvent;
	while (true)
	{
		while (enet_host_service(server, &netEvent, 1000) > 0)
		{
			switch (netEvent.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
			{
				printf("Client connected from %x:%u.\n", netEvent.peer->address.host, netEvent.peer->address.port);
			} break;

			case ENET_EVENT_TYPE_RECEIVE:
			{
				printf("%s > %s\n", (char*) netEvent.peer->data, netEvent.packet->data);

				// enet_host_broadcast(server, 0, netEvent.packet);
			} break;

			case ENET_EVENT_TYPE_DISCONNECT:
			{
				printf("%s disconnected.\n", (char*)  netEvent.peer->data);
			} break;
			}
		}
	}
}
