#include "pch.h"

bool Sockets::Send(const char* Buffer, int Size)
{
	int RecvLen = Size;
	int BufferPos = 0;

	while (RecvLen)
	{
		int len = send(this->fd, (char*)Buffer + BufferPos, min(Size, 1024), 0);

		if (len == -1)
			break;

		RecvLen -= len;
		BufferPos += len;

		if (!len || BufferPos == Size)
			return true;
	}

	return false;

}

bool Sockets::Receive(const char* Buffer, int Size)
{
	int RecvLen = Size;
	int BufferPos = 0;

	int timeOut = 1000;

	while (RecvLen)
	{
		int len = recv(this->fd, (char*)Buffer + BufferPos, min(Size, 1024), 0);

		if (len == -1)
			break;

		RecvLen -= len;
		BufferPos += len;

		if (!len || BufferPos == Size)
			return true;
	}

	return false;
}

bool Sockets::StablishConnection()
{
	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = inet_addr(this->ipBuffer);
	this->address.sin_port = htons(this->portVar);

	this->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (connect(this->fd, (struct sockaddr*)&this->address, sizeof(sockaddr)) < 0)
		return false;

	this->isConnectionStablished = true;

	return true;
}

void Sockets::Close()
{
	shutdown(this->fd, SB_BOTH);
	closesocket(this->fd);

	this->isConnectionClosed = true;
}


Sockets* Sockets::Accept(sockaddr* addr, int* len)
{
	SOCKET clfd = accept(this->fd, addr, len);

	if (clfd == -1 || clfd == 0)
		return NULL;

	return new Sockets(clfd);
}

int Sockets::SetSocketOption(int Level, int OptionName, void* Value, int ValueSize)
{
	return setsockopt(this->fd, Level, OptionName, (const char*)Value, ValueSize);
}

bool Sockets::Listen(int backLog)
{
	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = INADDR_ANY;
	this->address.sin_port = htons(this->portVar);

	this->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (bind(this->fd, (struct sockaddr*)&this->address, sizeof(sockaddr_in)) < 0)
		return false;

	if (listen(this->fd, backLog) < 0)
		return false;

	return true;
}

Sockets::Sockets(const char* Ip, short Port)
{
	if (Ip && Port)
	{
		this->portVar = Port;
		strcpy_s(this->ipBuffer, 32, Ip);

		this->isConnectionClosed = false;
		this->isListener = false;
		this->isClient = true;
	}
	else this->invalidData = true;

}

Sockets::Sockets(short Port)
{
	if (Port)
	{
		this->isConnectionClosed = false;
		this->portVar = Port;
		this->isListener = true;
		this->isClient = false;
	}
	else this->invalidData = true;
}

Sockets::Sockets(SOCKET fd)
{
	if (fd)
	{
		this->isConnectionClosed = false;
		this->isListener = false;
		this->fd = fd;
		this->isClient = true;
	}
	else this->invalidData = true;
}

Sockets::~Sockets()
{
	if (!this->isConnectionClosed)
	{
		this->Close();
		this->isConnectionClosed = true;
	}
}