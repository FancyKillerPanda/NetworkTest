//  ===== Date Created: 19 January, 2020 ===== 

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define DEFAULT_PORT "58000"
#define DEFAULT_BUFLEN 512

int main(int argc, char* argv[])
{
	//
	// Initialisation
	//
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Request Winsock version 2.2

	if (iResult != 0)
	{
		printf("Error: Initialisation failed.\n");
		return 1;
	}

	//
	// Socket creation
	//
	addrinfo* result = nullptr;
	addrinfo* ptr = nullptr;

	addrinfo hints = {};
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);

	if (iResult != 0)
	{
		printf("Error: getaddrinfo() failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	ptr = result;
	SOCKET connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (connectSocket == INVALID_SOCKET)
	{
		printf("Error: invalid socket (%ld)\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	//
	// Connect socket to a server
	//
	iResult = connect(connectSocket, ptr->ai_addr, (int) ptr->ai_addrlen);

	if (iResult == SOCKET_ERROR)
	{
		closesocket(connectSocket);
		connectSocket = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	if (connectSocket == INVALID_SOCKET)
	{
		printf("Error: unable to connect to a server.\n");
		WSACleanup();
		return 1;
	}

	//
	// Send an initial buffer
	//
	const char* message = "Hello, sockets!";
	iResult = send(connectSocket, message, (int) strlen(message), 0);

	if (iResult == SOCKET_ERROR)
	{
		printf("Error: send failed (%ld)\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes sent: %d\n", iResult);

	//
	// Receive data until the serve closes the connection
	//
	int recvBufLen = DEFAULT_BUFLEN;
	char recvBuf[DEFAULT_BUFLEN];
	
	do
	{
		iResult = recv(connectSocket, recvBuf, recvBufLen, 0);

		if (iResult > 0)
		{
			printf("Bytes received: %d\n", iResult);
		}
		else if (iResult == 0)
		{
			printf("Connection closed.\n");
		}
		else
		{
			printf("Error: recv failed (%ld)\n", WSAGetLastError());
		}
	} while (iResult > 0);

	//
	// Disconnect
	//
	iResult = shutdown(connectSocket, SD_SEND);

	if (iResult == SOCKET_ERROR)
	{
		printf("Error: shutdown failed (%ld)\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	closesocket(connectSocket);
	WSACleanup();
	
	return 0;
}
