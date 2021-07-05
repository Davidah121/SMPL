#pragma once

#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <vector>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <functional>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

class Network
{
public:

	Network(bool type, int port, std::string ipaddress, int amountOfConnectionsAllowed = 64, bool TCP = true);
	~Network();

	bool sendMessage(std::string message, int id=0);
	bool sendMessage(std::vector<unsigned char> message, int id=0);
	bool sendMessage(unsigned char* message, int size, int id=0);
	
	bool receiveMessage(std::string& message, int id=0);
	bool receiveMessage(std::vector<unsigned char> message, int id=0);
	bool receiveMessage(unsigned char* buffer, int bufferSize, int id=0);

	bool sendMessage(char* message, int messageSize, int id=0);
	bool receiveMessage(char* buffer, int bufferSize, int id=0);

	void reconnect();
	void disconnect();

	bool getRunning();
	
	void setOnConnectFunction(std::function<void(int)> func);
	void setOnMessageArrivedFunction(std::function<void(int)> func);
	void setOnDisconnectFunction(std::function<void(int)> func);
	

	static const bool TYPE_SERVER = false;
	static const bool TYPE_CLIENT = true;
private:
	
	bool init();

	void dispose();
	void createSocket(bool TCP);
	void closeSocket();

	void setupSocket();
	bool bindSocket();

	void removeSocket(SOCKET s);

	void listen();
	void acceptConnection();
	void connect();

	void setRunning(bool v);

	void threadRun();

	bool getReconnect();
	std::function<void(int)> getConnectFunc();
	std::function<void(int)> getMessageArriveFunc();
	std::function<void(int)> getDisconnectFunc();
	

	std::function<void(int)> onConnectFunc;
	std::function<void(int)> onMessageArrivedFunc;
	std::function<void(int)> onDisconnectFunc;

	WSADATA wsaData;
	SOCKET sock;
	SOCKADDR_IN socketAddress;
	int sizeAddress = 0;
	unsigned long connectionTimeout = 1000;
	unsigned long messageTimeout = 100;

	std::vector<SOCKET> connections;
	int totalAllowedConnections = 64;

	bool type = TYPE_SERVER;
	int port = 0;
	std::string ipAddress;

	static int totalNetworks;
	
	std::thread networkThread;
	std::mutex networkMutex = std::mutex();

	bool running = false;
	bool shouldConnect = true;
	bool isConnected = false;
};

