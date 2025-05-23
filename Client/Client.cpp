#include "pch.h"

struct Header
{
	int dataSize;
	unsigned char timeChecksum[0x14];
};

struct Data : Header
{
	unsigned char lotsOfData[0x200];
};

void FinalizeSecData(Header* Header, int size, std::time_t time)
{
	unsigned char* realDataBuffer = (unsigned char*)Header;

	unsigned long long tempValue = 0;

	for (int i = 0; i < size; i++)
		tempValue += (realDataBuffer[i] ^ time) % 255;


	sha1(&tempValue, 4, Header->timeChecksum);
}

int main()
{
	WSADATA wsaData = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	std::time_t time_now = std::time(nullptr);
	printf("Time in the client is %lli\n", time_now);

	Data clientData;

	memset(&clientData, 0, sizeof(Data));
	memset(clientData.lotsOfData, 0xFF, 0x200);
	FinalizeSecData(&clientData, sizeof(Header) - 0x14, time_now);

	Sleep(5000);

	Sockets Client("127.0.0.1", 1337);

	if (Client.InitConnection(0))
	{
		char buffer[4096] = { 0 };
	
		memcpy(buffer, &clientData, sizeof(Data));

		Client.Send(buffer, 4096);
	
		printf("Client Connected\n");
	}

	system("pause");
	return 0;
}
