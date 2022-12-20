#include "Network.h"
#include "System.h"
#include "MathExt.h"

namespace glib
{
	int Network::totalNetworks = 0;

	Network::Network(bool type, int port, std::string location, int amountOfConnectionsAllowed, bool TCP)
	{
		this->type = type;
		this->port = port;
		this->location = location;
		this->isTCP = TCP;
		this->totalAllowedConnections = amountOfConnectionsAllowed;

		if(!init())
		{
			//error occured
			//throw error
		}
		else
		{
			initNetwork(TCP);
			networkThread = std::thread(&Network::threadRun, this);
		}
	}

	Network::~Network()
	{
		startNetwork(); //Must start the network if it has not ever been started.
		setRunning(false);

		if(networkThread.joinable())
			networkThread.join();
		
		disconnect();
		dispose();
		
		shouldStart = false;
	}

	bool Network::init()
	{
		#ifndef __unix__
			//REQUIRED ON WINDOWS
			int status = WSAStartup(MAKEWORD(2, 2), &wsaData);
			Network::totalNetworks++;

			if (status != 0)
			{
				//error
				return false;
			}
			return true;
		#endif

		return true;
	}

	void Network::initNetwork(bool tcp)
	{
		if(type==TYPE_SERVER)
		{
			createSocket(tcp);
			setupSocket();
			bindSocket();

			setRunning(true);
		}
		else if(type==TYPE_CLIENT)
		{
			createSocket(tcp);
			setupSocket();
			connections.push_back(sock);
			waitingOnRead.push_back(false);
			sock = 0;

			setRunning(true);
		}
	}

	void Network::createSocket(bool tcp)
	{
		if(tcp)
			sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		else
			sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		u_long mode = 1;
		#ifdef __unix__
			ioctl(sock, FIONBIO, &mode);
		#else
			ioctlsocket(sock, FIONBIO, &mode);
		#endif
	}


	void Network::setupSocket()
	{
		socketAddress.sin_port = htons(port);
		socketAddress.sin_family = AF_INET;


		if(location[0] >= '0' && location[0] <= '9')
		{
			inet_pton(AF_INET, location.c_str(), &socketAddress.sin_addr); //IPv4
		}
		else
		{
			hostent* host = gethostbyname(location.c_str());
			socketAddress.sin_addr.s_addr = *((int32_t*)host->h_addr_list[0]); //Website Name
		}
		
		sizeAddress = sizeof(socketAddress);
	}

	bool Network::bindSocket()
	{
		if (type == Network::TYPE_SERVER)
		{
			int error = bind(sock, (sockaddr*)(&socketAddress), sizeAddress);

			if (error == SOCKET_ERROR)
			{
				return false;
			}

			return true;
		}
		return false;
	}

	void Network::listen()
	{
		if (type == Network::TYPE_SERVER)
		{
			::listen(sock, SOMAXCONN);
		}
	}

	void Network::acceptConnection()
	{
		if (type == Network::TYPE_SERVER)
		{
			SOCKET_TYPE tempSock = INVALID_SOCKET;

			while (tempSock == INVALID_SOCKET)
			{
				tempSock = accept(sock, (sockaddr*)&socketAddress, (socklen_t*)&sizeAddress);
			}
			
			connections.push_back(tempSock);
			waitingOnRead.push_back(false);
		}
	}

	void Network::closeSocket()
	{
		networkMutex.lock();

		for(size_t i=0; i<connections.size(); i++)
		{
			#ifdef __unix__
				close(connections[i]);
			#else
				closesocket(connections[i]);
			#endif
		}
		connections.clear();
		waitingOnRead.clear();
		
		#ifdef __unix__
			close(sock);
		#else
			closesocket(sock);
		#endif
		sock = 0;
		isConnected = false;
		timeWaited = 0;
		shouldConnect = false;

		networkMutex.unlock();
	}

	void Network::connect()
	{
		int status = SOCKET_ERROR;
		
		if (type == Network::TYPE_CLIENT)
		{
			status = ::connect(connections[0], (sockaddr*)&socketAddress, sizeAddress);
		}
	}

	bool Network::sendMessage(std::string message, int id)
	{
		return sendMessage((char*)message.c_str(), message.size()+1, id);
	}
	bool Network::sendMessage(std::vector<unsigned char> message, int id)
	{
		return sendMessage((char*)message.data(), message.size(), id);
	}
	bool Network::sendMessage(unsigned char* message, int size, int id)
	{
		return sendMessage((char*)message, size, id);
	}

	bool Network::receiveMessage(std::string& message, int id)
	{
		//allocate buffer of x size. read till '\0'
		networkMutex.lock();
		char* tempBuffer = new char[4096];
		memset(tempBuffer, 0, 4096);
		bool good = false;

		int status = SOCKET_ERROR;
		while(true)
		{
			if(type == Network::TYPE_CLIENT)
				status = recv(connections[0], tempBuffer, 4096, MSG_PEEK);
			else
				status = recv(connections[id], tempBuffer, 4096, MSG_PEEK);

			if(status>0)
			{
				int bytesToRead = -1;
				for(int i=0; i<status; i++)
				{
					if(tempBuffer[i]!='\0')
					{
						message += tempBuffer[i];
					}
					else
					{
						bytesToRead = i+1;
						break;
					}
				}

				if(bytesToRead==-1)
				{
					//did not find '\0', redo until we do
					if(type == Network::TYPE_CLIENT)
						status = recv(connections[0], tempBuffer, 4096, 0);
					else
						status = recv(connections[id], tempBuffer, 4096, 0);
				}
				else
				{
					if(type == Network::TYPE_CLIENT)
						status = recv(connections[0], tempBuffer, bytesToRead, 0);
					else
						status = recv(connections[id], tempBuffer, bytesToRead, 0);

					good = true;
					break;
				}
			}
			else
			{
				//error occured
				break;
			}
		}

		networkMutex.unlock();

		delete[] tempBuffer;
		return good;
	}

	bool Network::receiveMessage(std::vector<unsigned char>& buffer, int id)
	{
		//allocate buffer of x size. read till status == 0
		networkMutex.lock();
		char* tempBuffer = new char[4096];
		memset(tempBuffer, 0, 4096);
		bool good = false;

		int status = SOCKET_ERROR;
		while(true)
		{
			if(type == Network::TYPE_CLIENT)
				status = recv(connections[0], tempBuffer, 4096, MSG_PEEK);
			else
				status = recv(connections[id], tempBuffer, 4096, MSG_PEEK);

			if(status <= 0)
				break;
			
			if(type == Network::TYPE_CLIENT)
				status = recv(connections[0], tempBuffer, 4096, 0);
			else
				status = recv(connections[id], tempBuffer, 4096, 0);
				
			for(int i=0; i<status; i++)
			{
				buffer.push_back(tempBuffer[i]);
			}
			good = true;
		}
		
		networkMutex.unlock();

		delete[] tempBuffer;
		return good;
	}

	bool Network::receiveMessage(unsigned char* buffer, int bufferSize, int id)
	{
		return receiveMessage((char*)buffer, bufferSize, id);
	}

	bool Network::sendMessage(char * message, int messageSize, int id)
	{
		networkMutex.lock();

		int status = SOCKET_ERROR;
		if(type == Network::TYPE_CLIENT)
			status = send(connections[0], message, messageSize, 0);
		else
			status = send(connections[id], message, messageSize, 0);
		
		networkMutex.unlock();
		
		if(status == 0 || status == SOCKET_ERROR)
		{
			//error, if 0, closed
			return false;
		}
		return true;
	}

	bool Network::receiveMessage(char * buffer, int bufferSize, int id)
	{
		networkMutex.lock();

		int status = SOCKET_ERROR;
		if(type == Network::TYPE_CLIENT)
			status = recv(connections[0], buffer, bufferSize, 0);
		else
			status = recv(connections[id], buffer, bufferSize, 0);
		
		networkMutex.unlock();

		if(status == 0 || status == SOCKET_ERROR)
		{
			//error, if 0, closed
			return false;
		}
		return true;
	}
	
	bool Network::peek(std::vector<unsigned char>& buffer, int expectedSize, int id)
	{
		networkMutex.lock();

		int status = SOCKET_ERROR;
		buffer.resize(expectedSize);
		if(type == Network::TYPE_CLIENT)
			status = recv(connections[0], (char*)buffer.data(), expectedSize, MSG_PEEK);
		else
			status = recv(connections[id], (char*)buffer.data(), expectedSize, MSG_PEEK);
		
		if(status > 0 && status < expectedSize)
		{
			buffer.resize(status);
		}
		networkMutex.unlock();

		if(status == 0 || status == SOCKET_ERROR)
		{
			//error, if 0, closed
			return false;
		}
		return true;
	}

	void Network::dispose()
	{
		closeSocket();
		#ifndef __unix__
			Network::totalNetworks--;
			if(Network::totalNetworks==0)
			{
				int status = WSACleanup();
				if (status != 0)
				{
					//error
				}
			}
		#endif
	}

	void Network::reconnect()
	{
		networkMutex.lock();
		shouldConnect = true;
		networkMutex.unlock();
	}

	void Network::disconnect()
	{
		networkMutex.lock();
		for(size_t i=0; i<connections.size(); i++)
		{
			if(connections[i] != 0)
			{
				#ifdef __unix__
					close(connections[i]);
				#else
					closesocket(connections[i]);
				#endif
			}
		}
		connections.clear();
		waitingOnRead.clear();
		shouldConnect = false;
		isConnected = false;
		timeWaited = 0;
		networkMutex.unlock();
	}

	void Network::disconnect(int id)
	{
		removeSocket(connections[id]);
	}

	std::string Network::getIPFromConnection(int id)
	{
		networkMutex.lock();
		sockaddr_in addr;
		int len = sizeof(sockaddr_in);
		#ifdef __unix__
			int err = getpeername(connections[id], (sockaddr*)&addr, (socklen_t*)&len);
		#else
			int err = getpeername(connections[id], (sockaddr*)&addr, &len);
		#endif
		networkMutex.unlock();

		if(err == 0)
		{
			std::string returnVal = inet_ntoa(addr.sin_addr);
			return returnVal;
		}
		return "";
	}

	int Network::getIDFromIP(std::string s)
	{
		int id = -1;
		networkMutex.lock();
		for(size_t i=0; i<connections.size(); i++)
		{
			sockaddr_in addr;
			int len = sizeof(sockaddr_in);
			#ifdef __unix__
				int err = getpeername(connections[i], (sockaddr*)&addr, (socklen_t*)&len);
			#else
				int err = getpeername(connections[i], (sockaddr*)&addr, &len);
			#endif
			if(err == 0)
			{
				std::string testVal = inet_ntoa(addr.sin_addr);
				if(testVal == s)
				{
					id = i;
					break;
				}
			}
		}
		networkMutex.unlock();
		return id;
	}

	void Network::removeSocket(SOCKET_TYPE s)
	{
		networkMutex.lock();
		std::vector<SOCKET_TYPE> nSockets;
		std::vector<bool> nWaitingOnRead;
		for(size_t i=0; i<connections.size(); i++)
		{
			if(connections[i]!=s)
			{
				nSockets.push_back(connections[i]);
				nWaitingOnRead.push_back(waitingOnRead[i]);
			}
			else
			{
				#ifdef __unix__
					close(s);
				#else
					closesocket(s);
				#endif
			}
		}
		connections = nSockets;
		waitingOnRead = nWaitingOnRead;
		networkMutex.unlock();
	}

	void Network::setOnConnectFunction(std::function<void(int)> func)
	{
		networkMutex.lock();
		onConnectFunc = func;
		networkMutex.unlock();
	}
	void Network::setOnMessageArrivedFunction(std::function<void(int)> func)
	{
		networkMutex.lock();
		onMessageArrivedFunc = func;
		networkMutex.unlock();
	}
	void Network::setOnDisconnectFunction(std::function<void(int)> func)
	{
		networkMutex.lock();
		onDisconnectFunc = func;
		networkMutex.unlock();
	}

	std::function<void(int)> Network::getConnectFunc()
	{
		networkMutex.lock();
		std::function<void(int)> cpy = onConnectFunc;
		networkMutex.unlock();
		return cpy;
	}
	std::function<void(int)> Network::getMessageArriveFunc()
	{
		networkMutex.lock();
		std::function<void(int)> cpy = onMessageArrivedFunc;
		networkMutex.unlock();
		return cpy;
	}
	std::function<void(int)> Network::getDisconnectFunc()
	{
		networkMutex.lock();
		std::function<void(int)> cpy = onDisconnectFunc;
		networkMutex.unlock();
		return cpy;
	}

	void Network::setRunning(bool v)
	{
		networkMutex.lock();
		running = v;
		networkMutex.unlock();
	}
	bool Network::getRunning()
	{
		networkMutex.lock();
		bool v = running;
		networkMutex.unlock();
		return v;
	}
	
	bool Network::getTimeoutOccurred()
	{
		networkMutex.lock();
		bool v = timeoutOccurred;
		networkMutex.unlock();
		return v;
	}

	bool Network::getReconnect()
	{
		networkMutex.lock();
		bool v = shouldConnect;
		networkMutex.unlock();
		return v;
	}

	void Network::startNetwork()
	{
		networkMutex.lock();
		shouldStart = true;
		networkMutex.unlock();
	}

	void Network::endNetwork()
	{
		closeSocket();
		setRunning(false);
	}

	bool Network::getShouldStart()
	{
		networkMutex.lock();
		bool v = shouldStart;
		networkMutex.unlock();
		return v;
	}

	bool Network::isWaitingOnRead(int id)
	{
		networkMutex.lock();
		bool v = waitingOnRead[id];
		networkMutex.unlock();
		return v;
	}

	void Network::setDoneReceiving(int id)
	{
		networkMutex.lock();
		waitingOnRead[id] = false;
		networkMutex.unlock();
	}
	
	#ifdef __unix__
		void Network::threadRun()
		{
			bool init = false;

			while(!getShouldStart())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			while(getRunning())
			{
				if(type==TYPE_SERVER)
				{
					if(!init)
					{
						listen();
						init = true;
					}

					while(getRunning())
					{
						
						if(connections.size() < (size_t)totalAllowedConnections)
						{
							bool incommingConnection = false;
							networkMutex.lock();

							pollfd listeningSocketFD = {};
							listeningSocketFD.fd = sock;
							listeningSocketFD.events = POLLRDNORM;
							listeningSocketFD.revents = POLLRDNORM;

							int err = poll(&listeningSocketFD, 1, 1);
							if(err > 0)
							{
								if(listeningSocketFD.revents & POLLRDNORM)
								{
									//connected
									incommingConnection = true;
								}
							}

							networkMutex.unlock();
							
							//Note retVal of 0 means timeout
							if(incommingConnection)
							{
								acceptConnection();
								std::function<void(int)> cpy = getConnectFunc();

								if(cpy!=nullptr)
									cpy(connections.size()-1);
							}
							else if(err<0)
							{
								//error occured
								disconnect();
							}
						}

						networkMutex.lock();

						std::vector<pollfd> clientConnections;
						for(size_t i=0; i<connections.size(); i++)
						{
							pollfd newFD = {};
							newFD.fd = connections[i];
							newFD.events = POLLRDNORM;
							newFD.revents = POLLRDNORM;
							
							clientConnections.push_back( newFD );
						}

						networkMutex.unlock();
						
						int err = poll(clientConnections.data(), clientConnections.size(), 1);
						
						if(err>0)
						{
							for(size_t i=0; i<clientConnections.size(); i++)
							{
								if(clientConnections[i].revents & POLLERR || clientConnections[i].revents & POLLHUP || clientConnections[i].revents & POLLNVAL)
								{
									//error occured or graceful exit
									//disconnected
									std::function<void(int)> cpy = getDisconnectFunc();
									int finalErr = errno;

									if(finalErr == EWOULDBLOCK)
									{
										//Do nothing. Acceptable error
									}
									else
									{
										if(cpy!=nullptr)
											cpy(i);
										disconnect();
										break;
									}
								}
								else if(clientConnections[i].revents & POLLRDNORM)
								{
									//check for disconnect
									bool valid = true;
									networkMutex.lock();
									char testChar = 0;
									valid = recv(clientConnections[i].fd, &testChar, 1, MSG_PEEK) > 0;
									networkMutex.unlock();

									if(!valid)
									{
										//error occured or graceful exit
										//disconnected
										std::function<void(int)> cpy = getDisconnectFunc();
										int finalErr = errno;

										if(finalErr == EWOULDBLOCK)
										{
											//Do nothing. Acceptable error
										}
										else
										{
											if(cpy!=nullptr)
												cpy(i);
											disconnect();
											break;
										}
									}
									else
									{
										bool skip = isWaitingOnRead(i);
										if(!skip)
										{
											networkMutex.lock();
											waitingOnRead[i] = true;
											networkMutex.unlock();

											//can receive message
											std::function<void(int)> cpy = getMessageArriveFunc();
											if(cpy!=nullptr)
												cpy(i);
										}
									}
								}
							}
						}
						
						//sleep for x time
						std::this_thread::sleep_for(std::chrono::milliseconds(10));
					}
					
				}
				else
				{
					bool wouldConnect = false;
					while(getRunning())
					{
						if(getReconnect())
						{
							connect();
							int lastError = errno;
							wouldConnect = (lastError == EISCONN);

							if(wouldConnect)
							{
								std::function<void(int)> cpy = getConnectFunc();
								
								if(cpy!=nullptr)
									cpy(0);
								break;
							}
							
							networkMutex.lock();
							timeoutOccurred = false;
							networkMutex.unlock();

							timeWaited += 10;
							if(timeWaited >= timeoutTimer)
							{
								disconnect();
								networkMutex.lock();
								timeoutOccurred = true;
								networkMutex.unlock();
							}
						}

						//sleep for x time
						std::this_thread::sleep_for(std::chrono::milliseconds(10));
					}

					//MESSAGE
					while(getRunning())
					{
						networkMutex.lock();

						bool skip = waitingOnRead[0];
						bool isSet = false;

						pollfd mainSocket = {};
						mainSocket.fd = connections[0];
						mainSocket.events = POLLRDNORM;
						mainSocket.revents = POLLRDNORM;

						int err = poll(&mainSocket, 1, 1);

						networkMutex.unlock();

						//Note retVal of 0 means timeout
						if(err>0)
						{
							//check for disconnect
							if(mainSocket.revents & POLLERR || mainSocket.revents & POLLHUP || mainSocket.revents & POLLNVAL)
							{
								//error occured or graceful exit
								//disconnected
								std::function<void(int)> cpy = getDisconnectFunc();
								int finalErr = errno;

								if(finalErr == EWOULDBLOCK)
								{
									//Do nothing. Acceptable error
								}
								else
								{
									if(cpy!=nullptr)
										cpy(0);
									disconnect();
									break;
								}
							}
							else if(mainSocket.revents & POLLRDNORM)
							{
								//check for disconnect
								bool valid = true;
								networkMutex.lock();
								char testChar = 0;
								valid = recv(mainSocket.fd, &testChar, 1, MSG_PEEK) > 0;
								networkMutex.unlock();

								if(!valid)
								{
									//error occured or graceful exit
									//disconnected
									std::function<void(int)> cpy = getDisconnectFunc();
									int finalErr = errno;

									if(finalErr == EWOULDBLOCK)
									{
										//Do nothing. Acceptable error
									}
									else
									{
										if(cpy!=nullptr)
											cpy(0);
										disconnect();
										break;
									}
								}
								else
								{
									if(!skip)
									{
										networkMutex.lock();
										waitingOnRead[0] = true;
										networkMutex.unlock();

										std::function<void(int)> cpy = getMessageArriveFunc();
										if(cpy!=nullptr)
											cpy(0);
									}
								}
							}
							
						}
						else if(err<0)
						{
							//error occured
							//disconnected
							std::function<void(int)> cpy = getDisconnectFunc();
							int finalErr = errno;

							if(finalErr == EWOULDBLOCK)
							{
								//Do nothing. Acceptable error
							}
							else
							{
								if(cpy!=nullptr)
									cpy(0);
								disconnect();
								break;
							}
								
						}

						//sleep for x time
						std::this_thread::sleep_for(std::chrono::milliseconds(10));
					}

				}
				
			}
		}
	#else
		void Network::threadRun()
		{
			bool init = false;

			while(!getShouldStart())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			while(getRunning())
			{
				if(type==TYPE_SERVER)
				{
					if(!init)
					{
						listen();
						init = true;
					}

					while(getRunning())
					{
						
						if(connections.size() < (size_t)totalAllowedConnections)
						{
							bool incommingConnection = false;
							networkMutex.lock();

							WSAPOLLFD listeningSocketFD = {};
							listeningSocketFD.fd = sock;
							listeningSocketFD.events = POLLRDNORM;
							listeningSocketFD.revents = POLLRDNORM;

							int err = WSAPoll(&listeningSocketFD, 1, 1);
							if(err > 0)
							{
								if(listeningSocketFD.revents & POLLRDNORM)
								{
									//connected
									incommingConnection = true;
								}
							}

							networkMutex.unlock();
							
							//Note retVal of 0 means timeout
							if(incommingConnection)
							{
								acceptConnection();
								std::function<void(int)> cpy = getConnectFunc();

								if(cpy!=nullptr)
									cpy(connections.size()-1);
							}
							else if(err<0)
							{
								//error occured
								disconnect();
							}
						}

						networkMutex.lock();

						std::vector<WSAPOLLFD> clientConnections;
						for(size_t i=0; i<connections.size(); i++)
						{
							WSAPOLLFD newFD = {};
							newFD.fd = connections[i];
							newFD.events = POLLRDNORM;
							newFD.revents = POLLRDNORM;
							
							clientConnections.push_back( newFD );
						}

						networkMutex.unlock();
						
						int err = WSAPoll(clientConnections.data(), clientConnections.size(), 1);
						
						if(err>0)
						{
							for(size_t i=0; i<clientConnections.size(); i++)
							{
								//check for disconnect
								if(clientConnections[i].revents & POLLERR || clientConnections[i].revents & POLLHUP || clientConnections[i].revents & POLLNVAL)
								{
									//error occured or graceful exit
									//disconnected
									std::function<void(int)> cpy = getDisconnectFunc();
									int finalErr = WSAGetLastError();

									if(finalErr == WSAEWOULDBLOCK)
									{
										//Do nothing. Acceptable error
									}
									else
									{
										if(cpy!=nullptr)
											cpy(i);
										disconnect();
										break;
									}
								}
								else if(clientConnections[i].revents & POLLRDNORM)
								{
									//check for disconnect
									bool valid = true;
									networkMutex.lock();
									char testChar = 0;
									valid = recv(clientConnections[i].fd, &testChar, 1, MSG_PEEK) > 0;
									networkMutex.unlock();

									if(!valid)
									{
										//error occured or graceful exit
										//disconnected
										std::function<void(int)> cpy = getDisconnectFunc();
										int finalErr = WSAGetLastError();

										if(finalErr == WSAEWOULDBLOCK)
										{
											//Do nothing. Acceptable error
										}
										else
										{
											if(cpy!=nullptr)
												cpy(i);
											disconnect();
											break;
										}
									}
									else
									{
										bool skip = isWaitingOnRead(i);
										if(!skip)
										{
											networkMutex.lock();
											waitingOnRead[i] = true;
											networkMutex.unlock();

											//can receive message
											std::function<void(int)> cpy = getMessageArriveFunc();
											if(cpy!=nullptr)
												cpy(i);
										}
									}
								}
							}
						}
						
						//sleep for x time
						std::this_thread::sleep_for(std::chrono::milliseconds(10));
					}
					
				}
				else
				{
					bool wouldConnect = false;
					while(getRunning())
					{
						if(getReconnect())
						{
							if(connections.size() == 0)
							{
								initNetwork(isTCP);
							}
							connect();
							// int lastError = GetLastError(); //Not used
							wouldConnect = (GetLastError() == WSAEISCONN);

							if(wouldConnect)
							{
								std::function<void(int)> cpy = getConnectFunc();
								
								if(cpy!=nullptr)
									cpy(0);
								break;
							}
							
							networkMutex.lock();
							timeoutOccurred = false;
							networkMutex.unlock();

							timeWaited += 10;
							if(timeWaited >= timeoutTimer)
							{
								disconnect();
								networkMutex.lock();
								timeoutOccurred = true;
								networkMutex.unlock();
							}
						}

						//sleep for x time
						std::this_thread::sleep_for(std::chrono::milliseconds(10));
					}

					//MESSAGE
					while(getRunning())
					{
						networkMutex.lock();

						// bool skip = waitingOnRead[0]; //Not used

						WSAPOLLFD mainSocket = {};
						mainSocket.fd = connections[0];
						mainSocket.events = POLLRDNORM;
						mainSocket.revents = POLLRDNORM;

						int err = WSAPoll(&mainSocket, 1, 1);

						networkMutex.unlock();

						//Note retVal of 0 means timeout
						if(err>0)
						{
							//check for disconnect
							if(mainSocket.revents & POLLERR || mainSocket.revents & POLLHUP || mainSocket.revents & POLLNVAL)
							{
								//error occured or graceful exit
								//disconnected
								std::function<void(int)> cpy = getDisconnectFunc();
								int finalErr = WSAGetLastError();

								if(finalErr == WSAEWOULDBLOCK)
								{
									//Do nothing. Acceptable error
								}
								else
								{
									if(cpy!=nullptr)
										cpy(0);
									disconnect();
									break;
								}
							}
							else if(mainSocket.revents & POLLRDNORM)
							{
								//check for disconnect
								bool valid = true;
								networkMutex.lock();
								char testChar = 0;
								valid = recv(mainSocket.fd, &testChar, 1, MSG_PEEK) > 0;
								networkMutex.unlock();

								if(!valid)
								{
									//error occured or graceful exit
									//disconnected
									std::function<void(int)> cpy = getDisconnectFunc();
									int finalErr = WSAGetLastError();

									if(finalErr == WSAEWOULDBLOCK)
									{
										//Do nothing. Acceptable error
									}
									else
									{
										if(cpy!=nullptr)
											cpy(0);
										disconnect();
										break;
									}
								}
								else
								{
									bool skip = isWaitingOnRead(0);
									if(!skip)
									{
										networkMutex.lock();
										waitingOnRead[0] = true;
										networkMutex.unlock();

										//can receive message
										std::function<void(int)> cpy = getMessageArriveFunc();
										if(cpy!=nullptr)
											cpy(0);
									}
								}
							}
						}
						else if(err<0)
						{
							//error occured
							//disconnected
							std::function<void(int)> cpy = getDisconnectFunc();
							int finalErr = WSAGetLastError();

							if(finalErr == WSAEWOULDBLOCK)
							{
								//Do nothing. Acceptable error
							}
							else
							{
								if(cpy!=nullptr)
									cpy(0);
								disconnect();
								break;
							}
						}

						//sleep for x time
						std::this_thread::sleep_for(std::chrono::milliseconds(10));
					}

				}
				
			}
		}
	#endif

} //NAMESPACE glib END