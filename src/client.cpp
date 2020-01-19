//  ===== Date Created: 19 January, 2020 ===== 

#include <stdio.h>

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
	
	// Create client host
	ENetHost* client = enet_host_create(0, 1, 2, 0, 0);
	if (!client)
	{
		printf("Error: Client creation failed.\n");
		return -1;
	}

	ENetAddress address;
	enet_address_set_host(&address, "localhost");
	address.port = 1234;

	// Connect
	ENetPeer* peer = enet_host_connect(client, &address, 2, 0);
	if (!peer)
	{
		printf("Error: No available peers.\n");
		return -1;
	}

	ENetEvent netEvent;
	if (!(enet_host_service(client, &netEvent, 5000) > 0 &&
		  netEvent.type == ENET_EVENT_TYPE_CONNECT))
	{
		enet_peer_reset(peer);
		peer = 0;
		printf("Error: No connection made.\n");

		return -1;
	}

	// Messaging loop
	char* message = "Hello there!";

	if (strlen(message) > 0)
	{
		ENetPacket* packet = enet_packet_create(message, strlen(message) + 1, ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, packet);
	}

	while (true)
	{
		while (enet_host_service(client, &netEvent, 0) > 0)
		{
			switch (netEvent.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
			{
				printf("New connection from %x\n", netEvent.peer->address.host);
			} break;

			case ENET_EVENT_TYPE_RECEIVE:
			{
				printf("Message from server: %s\n", netEvent.packet->data);
			} break;

			case ENET_EVENT_TYPE_DISCONNECT:
			{
				printf("%s disconnected.\n", (char*) netEvent.peer->data);
			} break;
			}
		}
	}
}
