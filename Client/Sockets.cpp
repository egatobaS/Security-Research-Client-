#include "pch.h"

bool Sockets::InitSockets()
{
	return true;
}

bool Sockets::InitConnection(int Protocol)
{
	struct sockaddr_in addr = { 0 };

	Socket = socket(AF_INET, SOCK_STREAM, Protocol);

	if (Socket == -1)
		return false;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(IP);

	if (addr.sin_addr.s_addr == INADDR_NONE)
	{
		struct hostent *phost = gethostbyname(IP);
		if ((phost) && (phost->h_addrtype == AF_INET))
			addr.sin_addr = *(in_addr*)(phost->h_addr);
	}

	if (connect(Socket, (sockaddr*)&addr, sizeof(addr)) < 0)
		return false;

	return true;
}

Sockets::Sockets(const char* ConnectionAddr, unsigned short Port)
{
	if (InitSockets())
	{
		strcpy_s(IP, ConnectionAddr);

		port = Port;

		hasConnectionBeenClosed = false;
	}
}

Sockets::Sockets(unsigned short Port)
{
	if (InitSockets())
	{
		hasConnectionBeenClosed = false;
		port = Port;
	}
}

Sockets::Sockets(unsigned int s)
{
	Socket = s;
	hasConnectionBeenClosed = false;
}

Sockets::~Sockets()
{
	Close();
}

void Sockets::Close()
{
#if defined(_WIN32)
	shutdown(Socket, 2);
	closesocket(Socket);
#else
	shutdown(Socket, SHUT_RDWR);
	close(Socket);
#endif

}

bool Sockets::isSocketConnected()
{
	char testBuffer = 0;

#if defined(_WIN32)
	return true;
#else
	int retError = recv(Socket, &testBuffer, 1, MSG_PEEK | MSG_DONTWAIT);

	if (retError == -1) {

		if (errno != EWOULDBLOCK)
			return false;
	}
#endif
	return true;
}

bool Sockets::Receive(char* Data, int Size)
{
	int Start = GetTickCount();

	char* CurrentPosition = (char*)Data;

	int DataLeft = Size;

	int ReceiveStatus = 0;

	while (DataLeft > 0)
	{
		if ((GetTickCount() - Start) > SOCKET_TIME_OUT) {
			return false;
		}

		int DataChunkSize = min(1024 * 2, DataLeft);

		if (!isSocketConnected()) return false;

#if defined(_WIN32)
		ReceiveStatus = recv(Socket, CurrentPosition, DataChunkSize, 0);
#else
		ReceiveStatus = recv(Socket, CurrentPosition, DataChunkSize, MSG_NOSIGNAL);
#endif
		if (ReceiveStatus == -1 && errno != EWOULDBLOCK)
			break;

		CurrentPosition += ReceiveStatus;
		DataLeft -= ReceiveStatus;

		if (ReceiveStatus == 0)
			break;
	}

	if (ReceiveStatus == -1) {
		printf("Receive Failed With Error %i\n", WSAGetLastError());
		return false;
	}
	return true;
}

bool Sockets::Send(const char* Data, int Length)
{
	int Start = GetTickCount();

	char* CurrentPosition = (char*)Data;

	int DataLeft = Length;

	int SentStatus = 0;

	while (DataLeft > 0)
	{
		if ((GetTickCount() - Start) > SOCKET_TIME_OUT) {
			return false;
		}

		int DataChunkSize = min(1024 * 2, DataLeft);

		if (!isSocketConnected()) return false;

#if defined(_WIN32)
		SentStatus = send(Socket, CurrentPosition, DataChunkSize, 0);
#else
		SentStatus = send(Socket, CurrentPosition, DataChunkSize, MSG_NOSIGNAL);
#endif
		if (SentStatus == -1 && errno != EWOULDBLOCK)
			break;

		DataLeft -= SentStatus;
		CurrentPosition += SentStatus;
	}

	if (SentStatus == -1) {
		printf("Send Failed With Error %i\n", errno);
		return false;
	}
	return true;
}

int Sockets::SetSocketOption(int Level, int OptionName, void * OptionValue, int OptionLenght)
{
#if defined(_WIN32)
	return setsockopt(Socket, Level, OptionName, (const char*)OptionValue, OptionLenght);
#else
	return setsockopt(Socket, Level, OptionName, (void*)OptionValue, OptionLenght);
#endif
	return 1;
}

bool Sockets::StartListener(int MaxConnections)
{
	struct sockaddr_in addr;

	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int Enable = 1;
	SetSocketOption(SOL_SOCKET, SO_REUSEADDR, &Enable, sizeof(int));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

#if defined(_WIN32)
	if (bind(Socket, (sockaddr*)&addr, sizeof(sockaddr)))
		return false;
#else
	if (bind(Socket, (struct sockaddr *)&addr, sizeof(sockaddr_in)))
		return false;
#endif

	if (listen(Socket, MaxConnections))
		return false;

	return true;
}

SOCKET Sockets::Accept(sockaddr * address, int* length)
{
#if defined(_WIN32)
	return accept(Socket, (struct sockaddr *)address, length);
#else
	return accept(Socket, (struct sockaddr *)address, (socklen_t*)length);
#endif

}