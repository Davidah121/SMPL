#pragma once

#ifdef __unix__
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>

	#include <arpa/inet.h> //inet_addr
	#include <netdb.h> //hostent
	#include <sys/ioctl.h>
	#define SOCKET_TYPE int
	
	#ifndef SOCKET_ERROR
		#define SOCKET_ERROR (-1)
	#endif
	#ifndef INVALID_SOCKET
		#define INVALID_SOCKET (~0)
	#endif
#else
	#pragma comment(lib, "Ws2_32.lib")
	#define _WINSOCK_DEPRECATED_NO_WARNINGS

	#include <WinSock2.h>
	#include <ws2tcpip.h>
	#define SOCKET_TYPE SOCKET
#endif

#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <functional>


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
		 *		Note that in a linux environment, the OS will keep sockets open for 60 seconds and they can not be
		 * 			reopened. This may cause errors when re-running a program
		 * @param type 
		 * 		Servers can have multiple connections where a client can only have one connection.
		 * 		valid types are:
		 * 			TYPE_SERVER
		 * 			TYPE_CLIENT
		 * @param port 
		 * 		Must be a valid port between [0,65535]
		 * @param location 
		 * 		Must be a valid ipv4 address or website name.
		 * 		For clients, it is the IP or name to connect to.
		 * 		For servers, it is the IP or name that will receive connections.
		 * @param amountOfConnectionsAllowed 
		 * 		Default is 64.
		 * @param TCP 
		 * 		If set to false, UDP is used.
		 * 		Default is true
		 */
		Network(bool type, int port, std::string location, int amountOfConnectionsAllowed = 64, bool TCP = true);

		/**
		 * @brief Destroy the Network object
		 * 
		 * 		Note that in a linux environment, the OS will keep sockets open for 60 seconds and they can not be
		 * 			reopened. This may cause errors when re-running a program.
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
		 * @param maxBufferSize
		 * 		The maximum size to read. 
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
		 * @brief Receives a message from the specified connected IP.
		 * 		Returns true if it was successful.
		 * 		Does not remove the data from the internal queue. 
		 * 		Buffer will be resized to the minimum of expected size or the number of bytes read.
		 * 
		 * @param buffer 
		 * 		The buffer to receive the message.
		 * @param expectedSize
		 * 		The size of the buffer passed in.
		 * @param id 
		 * 		Which connection to receive from.
		 * 		Client will always receive from 0.
		 * 		Default is 0.
		 * @return true 
		 * @return false 
		 */
		bool peek(std::vector<unsigned char>& buffer, int expectedSize, int id=0);

		/**
		 * @brief Call when done reading data from the internal queue.
		 * 		This allows multiple receiveMessage calls without affecting the message arrived callback.
		 * 		
		 * 		The class now will only notify you of a message after you have specified that you have read the previous stuff.
		 * @param id 
		 * 		Which connection to receive from.
		 * 		Client will always receive from 0.
		 * 		Default is 0.
		 */
		void setDoneReceiving(int id=0);

		/**
		 * @brief Attempts to reconnect.
		 * 		Only used for TYPE_CLIENT
		 * 
		 */
		void reconnect();

		/**
		 * @brief Disconnects from all connected IPs.
		 * 		
		 * 		Note that in a linux environment, the OS will keep sockets open for 60 seconds and they can not be
		 * 			reopened. This may cause errors when re-running a program.
		 */
		void disconnect();

		/**
		 * @brief Disconnects from the specified id.
		 * 		
		 * 		Note that in a linux environment, the OS will keep sockets open for 60 seconds and they can not be
		 * 			reopened. This may cause errors when re-running a program.
		 */
		void disconnect(int id);

		/**
		 * @brief Returns a string representing the ipaddress of the specified connection id.
		 * 
		 * @param id 
		 * @return std::string 
		 */
		std::string getIPFromConnection(int id);

		/**
		 * @brief Returns a connection id using the ipaddress provided.
		 * 		Returns -1 if nothing was found.
		 * 
		 * @param s 
		 * @return int 
		 */
		int getIDFromIP(std::string s);

		/**
		 * @brief Returns if the thread maintaining the Network connection is running.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getRunning();

		/**
		 * @brief Gets whether a Timeout Occurred while sending messages or connecting.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getTimeoutOccurred();
		
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
		
		/**
		 * @brief Starts up the network allowing it to connect 
		 * 		or receive connections. This delays any network interactions until
		 * 		after the programmer has specifies they should start.
		 * 
		 * 		Best called after initializing the necessary functions and data.
		 */
		void startNetwork();

		/**
		 * @brief Destroys the network. Afterwards, it can no longer be used.
		 * 		This is equivalent to calling the destructor of the object.
		 * 		After calling this, the thread maintaining the network will be closed
		 * 		and no longer running. This should end loops that run while the network is
		 * 		running.
		 * 
		 * 		Note that in a linux environment, the OS will keep sockets open for 60 seconds and they can not be
		 * 			reopened. This may cause errors when re-running a program.
		 * 
		 */
		void endNetwork();

		static const bool TYPE_SERVER = false;
		static const bool TYPE_CLIENT = true;
	private:
		
		bool init();

		void dispose();
		void createSocket(bool TCP);
		void closeSocket();

		void setupSocket();
		bool bindSocket();


		void listen();
		void acceptConnection();
		void connect();

		void setRunning(bool v);

		void threadRun();

		bool getReconnect();
		bool getShouldStart();

		bool isWaitingOnRead(int id);

		std::function<void(int)> getConnectFunc();
		std::function<void(int)> getMessageArriveFunc();
		std::function<void(int)> getDisconnectFunc();
		

		std::function<void(int)> onConnectFunc;
		std::function<void(int)> onMessageArrivedFunc;
		std::function<void(int)> onDisconnectFunc;

		#ifndef __unix__
			WSADATA wsaData;
		#endif
		
		SOCKET_TYPE sock;
		sockaddr_in socketAddress;
		std::vector<SOCKET_TYPE> connections;
		std::vector<bool> waitingOnRead;
		void removeSocket(SOCKET_TYPE s);

		int sizeAddress = 0;
		unsigned long timeoutTimer = 5000; //In milliseconds. 5 seconds total
		unsigned long timeWaited = 0;
		bool shouldStart = false;

		bool timeoutOccurred = false;

		int totalAllowedConnections = 64;

		bool type = TYPE_SERVER;
		int port = 0;
		std::string location;

		static int totalNetworks;
		
		std::thread networkThread;
		std::mutex networkMutex;

		bool running = false;
		bool shouldConnect = true;
		bool isConnected = false;
	};

} //NAMESPACE glib END