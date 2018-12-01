#pragma once

class Sockets
{
public:
	void Close();
	Sockets(unsigned short Port);
	Sockets(unsigned int s);
	Sockets(const char* ConnectReceiveionAddr, unsigned short Port);
	bool Receive(char* Data, int Size);
	bool Send(const char* Data, int Length);
	int SetSocketOption(int Level, int OptionName, void * OptionValue, int OptionLenght);
	bool StartListener(int MaxConnections);
	SOCKET Accept(sockaddr * address, int* length);
	bool InitConnection(int Protocol);
	SOCKET Socket;
	~Sockets();

private:
	bool InitSockets();
	bool isSocketConnected();
	bool hasConnectionBeenClosed = false;
	unsigned short port = 0;
	char IP[1024] = { 0 };
};

#define SOCKET_TIME_OUT 		5000