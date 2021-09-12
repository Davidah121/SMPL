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

namespace glib
{

	class Network
	{
	public:

		/**
		 * @brief Construct a new Network object
		 * 		Allows network connection using TCP or UDP.
		 * 		Uses a separate thread to maintain the connection and check for message arrival.
		 * 
		 * @param type 
		 * 		Servers can have multiple connections where a client can only have one connection.
		 * 		valid types are:
		 * 			TYPE_SERVER
		 * 			TYPE_CLIENT
		 * @param port 
		 * 		Must be a valid port between [0,65535]
		 * @param ipaddress 
		 * 		Must be a valid ipv4 address.
		 * 		For clients, it is the IP to connect to.
		 * 		For servers, it is the IP that will receive connections.
		 * @param amountOfConnectionsAllowed 
		 * 		Default is 64.
		 * @param TCP 
		 * 		If set to false, UDP is used.
		 * 		Default is true
		 */
		Network(bool type, int port, std::string ipaddress, int amountOfConnectionsAllowed = 64, bool TCP = true);

		/**
		 * @brief Destroy the Network object
		 * 
		 */
		~Network();

		/**
		 * @brief Sends a message to the specified connected IP.
		 * 		Returns true if it was successful.
		 * 
		 * @param message 
		 * @param id 
		 * 		Which connection to send to.
		 * 		Client will always send to 0
		 * 		Default is 0.
		 * @return true 
		 * @return false 
		 */
		bool sendMessage(std::string message, int id=0);

		/**
		 * @brief Sends a message to the specified connected IP.
		 * 		Returns true if it was successful.
		 * 
		 * @param message 
		 * @param id 
		 * 		Which connection to send to.
		 * 		Client will always send to 0
		 * 		Default is 0.
		 * @return true 
		 * @return false 
		 */
		bool sendMessage(std::vector<unsigned char> message, int id=0);

		/**
		 * @brief Sends a message to the specified connected IP.
		 * 		Returns true if it was successful.
		 * 
		 * @param message 
		 * @param size
		 * 		The size of the message.
		 * @param id 
		 * 		Which connection to send to.
		 * 		Client will always send to 0
		 * 		Default is 0.
		 * @return true 
		 * @return false 
		 */
		bool sendMessage(unsigned char* message, int size, int id=0);
		
		/**
		 * @brief Receives a message from the specified connected IP.
		 * 		Returns true if it was successful.
		 * 		Continues to read until the null character is found specifying the end of the string.
		 * 
		 * @param message 
		 * 		The string to receive the message.
		 * @param id 
		 * 		Which connection to receive from.
		 * 		Client will always receive from 0.
		 * 		Default is 0.
		 * @return true 
		 * @return false 
		 */
		bool receiveMessage(std::string& message, int id=0);

		/**
		 * @brief Receives a message from the specified connected IP.
		 * 		Returns true if it was successful.
		 * 		Continues to read until the buffer is filled or there is no more data to read.
		 * 
		 * @param buffer 
		 * 		The buffer to receive the message.
		 * @param id 
		 * 		Which connection to receive from.
		 * 		Client will always receive from 0.
		 * 		Default is 0.
		 * @return true 
		 * @return false 
		 */
		bool receiveMessage(std::vector<unsigned char>& buffer, int id=0);

		/**
		 * @brief Receives a message from the specified connected IP.
		 * 		Returns true if it was successful.
		 * 		Continues to read until the buffer is filled or there is no more data to read.
		 * 
		 * @param buffer 
		 * 		The buffer to receive the message.
		 * @param bufferSize
		 * 		The size of the buffer.
		 * @param id 
		 * 		Which connection to receive from.
		 * 		Client will always receive from 0.
		 * 		Default is 0.
		 * @return true 
		 * @return false 
		 */
		bool receiveMessage(unsigned char* buffer, int bufferSize, int id=0);

		/**
		 * @brief Sends a message to the specified connected IP.
		 * 		Returns true if it was successful.
		 * 
		 * @param message 
		 * @param size
		 * 		The size of the message.
		 * @param id 
		 * 		Which connection to send to.
		 * 		Client will always send to 0
		 * 		Default is 0.
		 * @return true 
		 * @return false 
		 */
		bool sendMessage(char* message, int messageSize, int id=0);

		/**
		 * @brief Receives a message from the specified connected IP.
		 * 		Returns true if it was successful.
		 * 		Continues to read until the buffer is filled or there is no more data to read.
		 * 
		 * @param buffer 
		 * 		The buffer to receive the message.
		 * @param bufferSize
		 * 		The size of the buffer.
		 * @param id 
		 * 		Which connection to receive from.
		 * 		Client will always receive from 0.
		 * 		Default is 0.
		 * @return true 
		 * @return false 
		 */
		bool receiveMessage(char* buffer, int bufferSize, int id=0);

		/**
		 * @brief Attempts to reconnect.
		 * 		Only used for TYPE_CLIENT
		 * 
		 */
		void reconnect();

		/**
		 * @brief Disconnects from all connected IPs.
		 * 
		 */
		void disconnect();

		/**
		 * @brief Returns if the thread maintaining the Network connection is running.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getRunning();
		
		/**
		 * @brief Sets the On Connection Function.
		 * 		This is called as soon as a connection is established.
		 * 		The int passed into the function is the ID of the connection.
		 * 
		 * @param func 
		 */
		void setOnConnectFunction(std::function<void(int)> func);

		/**
		 * @brief Sets the On Message Arrived Function.
		 * 		This is called as soon as a message arrives.
		 * 		The int passed into the function is the ID of the connection that sent the message.
		 * 
		 * @param func 
		 */
		void setOnMessageArrivedFunction(std::function<void(int)> func);

		/**
		 * @brief Sets the On Disconnection Function.
		 * 		This is called as soon as a disconnection is found.
		 * 		The int passed into the function is the ID that disconnected.
		 * 
		 * @param func 
		 */
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

} //NAMESPACE glib END