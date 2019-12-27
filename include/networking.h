#pragma once

#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <vector>
#include <ws2tcpip.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

class networking
{
public:
	networking();
	~networking();

	static void init(bool value);

	static void dispose();
	static void createSocket();
	//static void createSocket(int af, int type, int protocol);
	static void closeSocket();

	static void setupSocket(int port);
	static void setupSocket(int port, char* ipAddress);
	static bool bindSocket();

	static void listen(int amount);
	static void acceptConnection();
	static void connect();

	static void sendMessage(char* message, int messageSize);
	static void receiveMessage(char* buffer, int bufferSize);

	static bool TYPE_SERVER;
	static bool TYPE_CLIENT;

private:
	static WSADATA wsaData;
	static SOCKET sock;
	static SOCKADDR_IN socketAddress;
	static int sizeAddress;

	static std::vector<SOCKET> connections;
	static std::vector<SOCKADDR_IN> connectionAddress;
	static bool type;
};

