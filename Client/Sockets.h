#pragma once

class Sockets
{
public:
	Sockets(const char* Ip, short Port);
	Sockets(short Port);
	Sockets(SOCKET fd);
	bool StablishConnection();
	bool Listen(int backLog);
	Sockets* Accept(sockaddr* addr, int* len);
	void Close();
	bool Receive(const char* Buffer, int Size);
	bool Send(const char* Buffer, int Size);
	int SetSocketOption(int Level, int OptionName, void* Value, int ValueSize);
	~Sockets();
private:
	short portVar;
	SOCKET fd;
	sockaddr_in address;
	char ipBuffer[32];
	bool isConnectionClosed;
	bool isConnectionStablished;
	bool isListener;
	bool isClient;
	bool invalidData;
};