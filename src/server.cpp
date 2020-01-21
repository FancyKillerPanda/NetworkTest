//  ===== Date Created: 19 January, 2020 ===== 

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define DEFAULT_PORT "58000"
#define DEFAULT_BUFLEN 512

int main()
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

	addrinfo hints = {};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(0, DEFAULT_PORT, &hints, &result);

	if (iResult != 0)
	{
		printf("Error: getaddrinfo() failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	
	if (listenSocket == INVALID_SOCKET)
	{
		printf("Error: invalid socket (%ld)\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	//
	// Bind socket
	//
	iResult = bind(listenSocket, result->ai_addr, (int) result->ai_addrlen);

	if (iResult == SOCKET_ERROR)
	{
		printf("Error: bind failed (%ld)\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	//
	// Listen
	//
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("Error: listen failed (%ld)\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//
	// Accept connnection
	//
	SOCKET clientSocket = accept(listenSocket, 0, 0);

	if (clientSocket == INVALID_SOCKET)
	{
		printf("Error: accept failed (%ld)\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//
	// Send and recieve
	//
	int recvBufLen = DEFAULT_BUFLEN;
	char recvBuf[DEFAULT_BUFLEN];

	do
	{
		iResult = recv(clientSocket, recvBuf, recvBufLen, 0);

		if (iResult > 0)
		{
			printf("Bytes received: %d\n", iResult);

			// Echo the buffer back to the sender
			int iSendResult = send(clientSocket, recvBuf, iResult, 0);

			if (iSendResult == SOCKET_ERROR)
			{
				printf("Error: send failed (%ld)\n", WSAGetLastError());
				closesocket(clientSocket);
				WSACleanup();
				return 1;
			}

			printf("Bytes sent: %ld\n", iSendResult);
		}
		else if (iResult == 0)
		{
			printf("Connection closing...\n");
		}
		else
		{
			printf("Error: recv failed (%ld)\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}
	} while (iResult > 0);

	//
	// Disconnect
	//
	iResult = shutdown(clientSocket, SD_SEND);

	if (iResult == SOCKET_ERROR)
	{
		printf("Error: shutdown failed (%ld)\n", WSAGetLastError());
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	closesocket(clientSocket);
	WSACleanup();

	return 0;
}
