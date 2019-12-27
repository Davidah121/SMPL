#include "networking.h"
#include "StringTools.h"

WSADATA networking::wsaData;
SOCKET networking::sock;
SOCKADDR_IN networking::socketAddress;
int networking::sizeAddress = 0;

std::vector<SOCKET> networking::connections;
std::vector<SOCKADDR_IN> networking::connectionAddress;

bool networking::TYPE_SERVER = false;
bool networking::TYPE_CLIENT = true;

bool networking::type = false;

networking::networking()
{
}


networking::~networking()
{
}

void networking::init(bool value)
{
	type = value;

	int status = WSAStartup(MAKEWORD(1, 1), &wsaData);

	if (status != 0)
	{
		StringTools::println("ERROR ON STARTUP");
	}
	else
	{
		if (LOBYTE(wsaData.wVersion) == 1)
		{
			//Worked
			StringTools::println("WORKED");
		}
		else
		{
			//Not correct version
			StringTools::println("DID NOT WORK");
		}
	}
}

void networking::createSocket()
{
	sock = socket(AF_INET, SOCK_STREAM, NULL);
}

/*
void networking::createSocket(int af, int type, int protocol)
{
	sock = socket(af, type, protocol);
}
*/

void networking::setupSocket(int port)
{
	socketAddress.sin_port = htons(port);
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_addr.S_un.S_addr = INADDR_ANY;
}

void networking::setupSocket(int port, char* ipAddress)
{
	socketAddress.sin_port = htons(port);
	socketAddress.sin_family = AF_INET;
	
	inet_pton(AF_INET, ipAddress, &socketAddress.sin_addr);
	//socketAddress.sin_addr.S_un.S_addr = inet_addr(ipAddress);
	sizeAddress = sizeof(socketAddress);
}

bool networking::bindSocket()
{
	if (type == networking::TYPE_SERVER)
	{
		int error = bind(sock, (SOCKADDR*)(&socketAddress), sizeAddress);

		if (error == SOCKET_ERROR)
		{
			StringTools::println("ERROR ON BIND");
			return false;
		}

		return true;
	}
	else
	{
		StringTools::println("Can't bind on a Client");
	}
	return false;
}

void networking::listen(int amount)
{
	if (type == networking::TYPE_SERVER)
	{
		::listen(sock, SOMAXCONN);
	}
	else
	{
		StringTools::println("Can't Listen on a Client");
	}
}

void networking::acceptConnection()
{
	if (type == networking::TYPE_SERVER)
	{
		SOCKET tempSock = NULL;

		while (tempSock == NULL)
		{
			tempSock = accept(sock, (SOCKADDR*)&socketAddress, &sizeAddress);
		}

		connections.push_back(tempSock);

		StringTools::println("CONNECTED");
		//connectionAddress.push_back(tempAddress);
	}
	else
	{
		StringTools::println("Can't perform this operation on a Client");
	}
	
}

void networking::closeSocket()
{
	for (int i = 0; i < connections.size(); i++)
	{
		closesocket(connections[i]);
	}
	closesocket(sock);
}

void networking::connect()
{
	if (type == networking::TYPE_CLIENT)
	{
		int status = ::connect(sock, (SOCKADDR*)(&socketAddress), sizeAddress);
		if (status != 0)
		{
			StringTools::println("Failed to connect to the specified ip");
		}
		else
		{
			StringTools::println("Connected to server");
		}
	}
	else
	{
		StringTools::println("Can't perform this operation of a Server");
	}
}

void networking::sendMessage(char * message, int messageSize)
{
	if (type == TYPE_CLIENT)
		send(sock, message, messageSize, 0);
	else
		send(connections[0], message, messageSize, 0);

	/*
	int status = SOCKET_ERROR;

	while (true)
	{
		if(type==TYPE_CLIENT)
			status = send(sock, message, messageSize, 0);
		else
			status = send(tempSock, message, messageSize, 0);

		std::cout << status << std::endl;
		if (status == 0)
		{
			std::cout << "Connection was closed" << std::endl;
			break;
		}
		else if (status == SOCKET_ERROR)
		{
			break;
		}
		else
		{
			//Something
			break;
		}
	}
	*/
}

void networking::receiveMessage(char * buffer, int bufferSize)
{
	
	if (type == TYPE_CLIENT)
		recv(sock, buffer, bufferSize, 0);
	else
		recv(connections[0], buffer, bufferSize, 0);
	/*

	int status = SOCKET_ERROR;

	while (true)
	{
		status = recv(sock, buffer, bufferSize, 0);
		std::cout << status << std::endl;

		if (status == 0)
		{
			std::cout << "Connection was closed" << std::endl;
			break;
		}
		else if (status == SOCKET_ERROR)
		{
			std::cout << "SOCKET ERROR" << std::endl;
			break;
		}
		else
		{
			//Something
			break;
		}
	}
	*/
}

void networking::dispose()
{
	int status = WSACleanup();
	if (status != 0)
	{
		StringTools::println("ERROR ON DISPOSE");
	}
}
