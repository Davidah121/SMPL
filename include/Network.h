#pragma once
#include "BuildOptions.h"

#ifndef NO_SOCKETS

#ifdef __unix__
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/sendfile.h>
	#include <poll.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>

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
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
    #include "TargetVer.h"

	#include <Winsock2.h>
	#include <winerror.h>
	#include <ws2tcpip.h>
	#include <MSWSock.h>
	
	#define SOCKET_TYPE SOCKET
#endif

#include <vector>
#include <map>
#include <iostream>
#include <functional>
#include <chrono>
#include <cstring>
#include "WebRequest.h"
#include "ext/SSLSingleton.h"
#include "Concurrency.h"

namespace smpl
{
	struct SocketInfo
	{
		SOCKET_TYPE socket = 0;
		std::string ip = "";
		size_t id = 0;
		bool waitingOnRead = false;
		std::chrono::system_clock::time_point lastInteractTime;
	};

	struct NetworkConfig
	{
		bool type = true;
		bool TCP = true;
		bool secure = false;
		int port = 4000;
		unsigned int amountOfConnectionsAllowed = 64;
		std::string location = "";
	};

	class DLL_OPTION Network
	{
	public:

		/**
		 * @brief Construct a new Network object
		 * 		Allows network connection using TCP or UDP.
		 * 		Uses a separate thread to maintain the connection and check for message arrival.
		 * 
		 *		Note that in a linux environment, the OS will keep sockets open for 60 seconds and they can not be
		 * 			reopened. This may cause errors when re-running a program
		 * @param NetworkConfig
		 * 		Configuration information for the network.
		 * 			Controls whether its UDP or TCP, the port, location to connect to, etc.
		 * 			The extraInfo part is for subclasses.
		 * @param certificateFile
		 * 		For a server that is secure (SSL), it must provide a certificate file.
		 * @param keyFile
		 * 		For a server that is secure (SSL), it must provide a key file.
		 */
		Network(NetworkConfig configuration, std::string certificateFile = "", std::string keyFile = "");

		/**
		 * @brief Destroy the Network object
		 * 
		 * 		Note that in a linux environment, the OS will keep sockets open for 60 seconds and they can not be
		 * 			reopened. This may cause errors when re-running a program.
		 */
		~Network();

		/**
		 * @brief Sends a message to the specified connected IP.
		 * 		Returns how many bytes were sent.
		 * 			Returns -1 if unsuccessful
		 * 		Sends a 0 to specify the end of the string.
		 * 			When receiving data, you can receive a string which will stop when a 0 is encountered.
		 * 			If this behavior is not desired, use sendMessage(message.c_str(), message.size(), id);
		 * 		
		 * @param message 
		 * @param id 
		 * 		Which connection to send to.
		 * 		Client will always send to 0
		 * 		Default is 0.
		 * @return size_t
		 */
		int sendMessage(const std::string& message, size_t id=0);

		/**
		 * @brief Sends a message to the specified connected IP.
		 * 		Returns how many bytes were sent.
		 * 			Returns -1 if unsuccessful
		 * 
		 * @param message 
		 * @param id 
		 * 		Which connection to send to.
		 * 		Client will always send to 0
		 * 		Default is 0.
		 * @return int 
		 */
		int sendMessage(std::vector<unsigned char>& message, size_t id=0);

		/**
		 * @brief Sends a WebRequest. Just used to prevent errors.
		 * 		Returns how many bytes were sent.
		 * 			Returns -1 if unsuccessful
		 * 
		 * @param message 
		 * @param id 
		 * 		Which connection to send to.
		 * 		Client will always send to 0
		 * 		Default is 0.
		 * @return int 
		 */
		int sendMessage(WebRequest& message, size_t id=0);

		/**
		 * @brief Sends a message to the specified connected IP.
		 * 		Returns how many bytes were sent.
		 * 			Returns -1 if unsuccessful
		 * 
		 * @param message 
		 * @param size
		 * 		The size of the message.
		 * @param id 
		 * 		Which connection to send to.
		 * 		Client will always send to 0
		 * 		Default is 0.
		 * @return int 
		 */
		int sendMessage(unsigned char* message, int size, size_t id=0);

		/**
		 * @brief Sends a message to the specified connected IP.
		 * 		Returns how many bytes were sent.
		 * 			Returns -1 if unsuccessful
		 * 
		 * @param message 
		 * @param size
		 * 		The size of the message.
		 * @param id 
		 * 		Which connection to send to.
		 * 		Client will always send to 0
		 * 		Default is 0.
		 * @return int
		 */
		int sendMessage(char* message, int messageSize, size_t id=0);

		template<typename T>
        int sendMessage(const T msg, size_t id)
        {
            //send raw bytes
            return sendMessage((char*)&msg, sizeof(T), id);
        }

        template<typename T>
        int sendMessage(const T* msg, int size, size_t id)
        {
            //send raw bytes
            return sendMessage((char*)msg, sizeof(T)*size, id);
        }

		int sendFile(char* filename, size_t length, size_t offset, size_t id=0);
		
		/**
		 * @brief Receives a message from the specified connected IP.
		 * 		Returns the number of bytes read if it was successful.
		 * 			Returns -1 if unsuccessful
		 * 		Continues to read until the null character is found specifying the end of the string.
		 * 			null character is 0.
		 * 		
		 * 		Resets the waiting on read flag.
		 * 			This will cause further onDataAvailable calls when data is available.
		 * 
		 * @param message 
		 * 		The string to receive the message.
		 * @param id 
		 * 		Which connection to receive from.
		 * 		Client will always receive from 0.
		 * 		Default is 0.
		 * @param flagRead
		 * 		If set to false, does not change the waiting on read flag.
		 * 			This must be set by the programmer later.
		 * 		By default, it is true.
		 * @return true 
		 * @return false 
		 */
		int receiveMessage(std::string& message, size_t id=0, bool flagRead = true);

		/**
		 * @brief Receives a message from the specified connected IP.
		 * 		Returns the number of bytes read if it was successful.
		 * 			Returns -1 if unsuccessful
		 * 		Continues to read until the buffer is filled or there is no more data to read.
		 * 
		 * 		Resets the waiting on read flag.
		 * 			This will cause further onDataAvailable calls when data is available.
		 * 
		 * @param buffer 
		 * 		The buffer to receive the message.
		 * @param maxBufferSize
		 * 		The maximum size to read. 
		 * @param id 
		 * 		Which connection to receive from.
		 * 		Client will always receive from 0.
		 * 		Default is 0.
		 * @param flagRead
		 * 		If set to false, does not change the waiting on read flag.
		 * 			This must be set by the programmer later.
		 * 		By default, it is true.
		 * 
		 * @return int
		 */
		int receiveMessage(std::vector<unsigned char>& buffer, size_t id=0, bool flagRead = true);

		/**
		 * @brief Receives a message from the specified connected IP.
		 * 		Returns the number of bytes read if it was successful.
		 * 			Returns -1 if unsuccessful
		 * 		Continues to read until the buffer is filled or there is no more data to read.
		 * 
		 * 		Resets the waiting on read flag.
		 * 			This will cause further onDataAvailable calls when data is available.
		 * 
		 * @param buffer 
		 * 		The buffer to receive the message.
		 * @param bufferSize
		 * 		The size of the buffer.
		 * @param id 
		 * 		Which connection to receive from.
		 * 		Client will always receive from 0.
		 * 		Default is 0.
		 * @param flagRead
		 * 		If set to false, does not change the waiting on read flag.
		 * 			This must be set by the programmer later.
		 * 		By default, it is true.
		 * @return true 
		 * @return false 
		 */
		int receiveMessage(unsigned char* buffer, int bufferSize, size_t id=0, bool flagRead = true);

		/**
		 * @brief Receives a message from the specified connected IP.
		 * 		Returns the number of bytes read if it was successful.
		 * 			Returns -1 if unsuccessful
		 * 		Continues to read until the buffer is filled or there is no more data to read.
		 * 
		 * 		Resets the waiting on read flag.
		 * 			This will cause further onDataAvailable calls when data is available.
		 * 
		 * @param buffer 
		 * 		The buffer to receive the message.
		 * @param bufferSize
		 * 		The size of the buffer.
		 * @param id 
		 * 		Which connection to receive from.
		 * 		Client will always receive from 0.
		 * 		Default is 0.
		 * @param flagRead
		 * 		If set to false, does not change the waiting on read flag.
		 * 			This must be set by the programmer later.
		 * 		By default, it is true.
		 * @return true 
		 * @return false 
		 */
		int receiveMessage(char* buffer, int bufferSize, size_t id=0, bool flagRead = true);

		template<typename T>
        int receiveMessage(const T msg, size_t id)
        {
            //send raw bytes
            return receiveMessage((char*)&msg, sizeof(T), id);
        }

        template<typename T>
        int receiveMessage(const T* msg, int size, size_t id)
        {
            //send raw bytes
            return receiveMessage((char*)msg, sizeof(T)*size, id);
        }

		/**
		 * @brief Receives a message from the specified connected IP.
		 * 		Returns the number of bytes read if it was successful.
		 * 			Returns -1 if unsuccessful
		 * 		Does not remove the data from the internal queue. 
		 * 		Buffer will be resized to the minimum of expected size or the number of bytes read.
		 * 
		 * 		Does not reset the waiting on read flag since the data was not removed from the internal queue.
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
		int peek(std::vector<unsigned char>& buffer, int expectedSize, size_t id=0);

		/**
		 * @brief Attempts to dump bytes from the receive buffer.
		 * 
		 * @param bytesToDump 
		 * @param id 
		 * @return int 
		 */
		int dumpReceiveBytes(int bytesToDump, size_t id=0);

		/**
		 * @brief Gets the amount of bytes available for receiving currently for
		 * 		the specified ID.
		 * 
		 * @param id 
		 * @return size_t 
		 */
		size_t getReadSizeAvailable(size_t id=0);

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
		void setDoneReceiving(size_t id=0);

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
		 * @param id
		 */
		void disconnect(size_t id);

		/**
		 * @brief Returns a string representing the ipaddress of the specified connection id.
		 * 
		 * @param id 
		 * @return std::string 
		 */
		std::string getIPFromConnection(size_t id);

		/**
		 * @brief Returns a connection id using the ipaddress provided.
		 * 		Returns SIZE_MAX if nothing was found.
		 * 
		 * @param s 
		 * @return size_t 
		 */
		size_t getIDFromIP(std::string s);

		/**
		 * @brief Get the Port that the network was opened on.
		 * 
		 * @return int 
		 */
		int getPort();

		/**
		 * @brief Get the Location that the network was opened on.
		 * 
		 * @return std::string 
		 */
		std::string getLocation();

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
		 * @brief Gets whether the client is connected or not.
		 * 		Must check before attempting any network operations.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getClientConnected();

		/**
		 * @brief Sets the timeout length.
		 * 		This is specified in milliseconds.
		 * 		This applies to all connections so for a server,
		 * 			this can be used to determine when to disconnect from a client that doesn't do anything.
		 * 		
		 * 		If set to a value less than 0, no timeout exist.
		 * 		
		 * 		Implementation notes:
		 * 			For a client, the timeout is only for establishing a connection.
		 * 			For a server, the timeout is for the time between interactions.
		 * 				The interactions are reads or writes. Not ACKs or anything else.
		 * @param millis 
		 * 		Default is 5000
		 * 			or 5 seconds
		 */
		void setTimeoutLength(long millis);

		/**
		 * @brief Gets the length of the timeout.
		 * 
		 * @param millis 
		 * @return long 
		 */
		long getTimeoutLength();
		
		/**
		 * @brief Sets the On Connection Function.
		 * 		This is called as soon as a connection is established.
		 * 		The int passed into the function is the ID of the connection.
		 * 
		 * @param func 
		 */
		void setOnConnectFunction(std::function<void(size_t)> func);

		/**
		 * @brief Sets the On Data Available Function.
		 * 		This is called whenever data is available to be read and has not been read yet.
		 * 		The int passed into the function is the ID of the connection that sent the message.
		 * 			Note that this does not mean new data. So, if you got 12 bytes but only read 8, then this will be called again.
		 * 			Note that this is only called once between reads.
		 * @param func 
		 */
		void setOnDataAvailableFunction(std::function<void(size_t)> func);

		/**
		 * @brief Sets the On Disconnection Function.
		 * 		This is called as soon as a disconnection is found.
		 * 		The int passed into the function is the ID that disconnected.
		 * 
		 * @param func 
		 */
		void setOnDisconnectFunction(std::function<void(size_t)> func);
		
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

		/**
		 * @brief Get the total number of sockets connected
		 * 
		 * @return size_t 
		 */
		size_t getSocketsConnectedSize();

		/**
		 * @brief Returns whether or not this uses SSL or is just a standard TCP socket.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool isSecure();

		static const bool TYPE_SERVER = false;
		static const bool TYPE_CLIENT = true;
		
	protected:
		SocketInfo mainSocketInfo;
		void sslInit();
		int internalRecv(SOCKET_TYPE sock, char* buff, int len); //negative value == problem. zero == fail but okay. positive value == success.
		int internalPeek(SOCKET_TYPE sock, char* buff, int len); //negative value == problem. zero == fail but okay. positive value == success.
		int internalSend(SOCKET_TYPE sock, char* buff, int len); //negative value == problem. zero == fail but okay. positive value == success.
		int internalSendfile(SOCKET_TYPE sock, char* filename, long offset, size_t length);
		int internalOnAccept(SOCKET_TYPE sock); //negative value == problem. zero == fail but okay. positive value == success.
		int internalOnConnect(SOCKET_TYPE sock); //negative value == problem. zero == fail but okay. positive value == success.
		void internalOnDelete(SOCKET_TYPE sock);

	private:
		
		bool init();
		void initNetwork();
		void dispose();
		
		void closeSocket();

		void setupSocket();
		void createSocket(int, int, int);
		bool bindSocket();


		void listen();
		bool acceptConnection();
		bool connect();

		void setRunning(bool v);

		void threadRun();

		bool getReconnect();
		bool getShouldStart();

		void runClient();
		void runServer();
		
		SocketInfo* getSocketInformation(size_t id);

		std::function<void(size_t)> getConnectFunc();
		std::function<void(size_t)> getDataAvailableFunc();
		std::function<void(size_t)> getDisconnectFunc();
		

		std::function<void(size_t)> onConnectFunc;
		std::function<void(size_t)> onDataAvailableFunc;
		std::function<void(size_t)> onDisconnectFunc;

		static const bool LOCK_TYPE_IMPORTANT = true;
		static const bool LOCK_TYPE_NONIMPORTANT = false;
		
		void obtainLock(bool type); //Important = true | Non Important = false
		void releaseLock();

		#ifndef __unix__
			WSADATA wsaData;
		#endif
		
		SOCKET_TYPE temporarySocket;
		sockaddr sockAddrInfo;
		
		std::map<size_t, SocketInfo*> connections;

		void removeSocket(size_t id);
		void removeSocketInternal(SOCKET_TYPE s);

		socklen_t sizeAddress = 0;
		long timeoutTimer = 5000; //In milliseconds. 5 seconds total
		long timeWaited = 0;
		bool shouldStart = false;

		bool timeoutOccurred = false;

		NetworkConfig config;
		static unsigned int totalNetworks;

		size_t runningID = 0;
		
		std::thread networkThread;

		bool inDispose = false;
		bool running = false;
		bool shouldConnect = true;
		bool isConnected = false;
		bool isSecureNetwork = false;
		std::string certificateFile = "";
		std::string keyFile = "";

		//Secure Socket stuff (SSL) Must have OpenSSL
		#ifdef USE_OPENSSL
		SSL* getSSLFromSocket(SOCKET_TYPE s);
		std::map<SOCKET_TYPE, SSL*> sslConnectionMapping;
		#endif

		HybridSpinSemaphore networkSemaphore;
		std::atomic_ullong timeWaitedOnImportantLock = 0;
		std::atomic_ullong timeWaitedOnNonImportantLock = 0;
	};

} //NAMESPACE smpl END

#endif