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

		if(!init())
		{
			//error occured
			//throw error
		}
		else
		{
			if(type==TYPE_SERVER)
			{
				this->totalAllowedConnections = amountOfConnectionsAllowed;
				createSocket(TCP);
				setupSocket();
				bindSocket();

				setRunning(true);
				networkThread = std::thread(&Network::threadRun, this);
			}
			else if(type==TYPE_CLIENT)
			{
				createSocket(TCP);
				setupSocket();
				connections.push_back(sock);
				waitingOnRead.push_back(false);
				sock = 0;

				setRunning(true);
				networkThread = std::thread(&Network::threadRun, this);
			}
		}
	}

	Network::~Network()
	{
		disconnect();
		setRunning(false);
		dispose();

		startNetwork(); //Must start the network if it has not ever been started.
		
		if(networkThread.joinable())
			networkThread.join();

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

		for(int i=0; i<connections.size(); i++)
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
		for(int i=0; i<connections.size(); i++)
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
		sockaddr_in addr = {0};
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
		for(int i=0; i<connections.size(); i++)
		{
			sockaddr_in addr = {0};
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
		for(int i=0; i<connections.size(); i++)
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
		disconnect();
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
	
	void Network::threadRun()
	{
		bool init = false;
		timeval waitTime = {0,100};
		fd_set socketSet;

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
					
					if(connections.size() < totalAllowedConnections)
					{
						networkMutex.lock();

						FD_ZERO(&socketSet);
						FD_SET(sock, &socketSet);
						waitTime = {0, 100};

						int retVal = select(sock+1, &socketSet, NULL, NULL, &waitTime);

						bool isSet = FD_ISSET(sock, &socketSet);
						networkMutex.unlock();
						
						//Note retVal of 0 means timeout
						if(retVal>0)
						{
							if(isSet)
							{
								acceptConnection();
								std::function<void(int)> cpy = getConnectFunc();

								if(cpy!=nullptr)
									cpy(connections.size()-1);
							}
						}
						else if(retVal<0)
						{
							//error occured
							disconnect();
						}
						
						networkMutex.lock();
						FD_CLR(sock, &socketSet);
						networkMutex.unlock();
					}

					networkMutex.lock();

					int startIndex=0;
					std::vector<SOCKET_TYPE> clients = connections;

					networkMutex.unlock();
					
					while(startIndex < connections.size())
					{
						int currSize = 0;
						if(clients.size() - startIndex < 64)
							currSize = clients.size() - startIndex;
						else
							currSize = 64;
						FD_ZERO(&socketSet);
						SOCKET_TYPE maxSock = 0;
						for(int i=0; i<currSize; i++)
						{
							FD_SET(clients[i + startIndex], &socketSet);
							maxSock = MathExt::min<SOCKET_TYPE>(maxSock, clients[i + startIndex]);
						}

						int retVal = select(maxSock+1, &socketSet, NULL, NULL, &waitTime);

						if(retVal>0)
						{
							for(int i=0; i<currSize; i++)
							{
								if(FD_ISSET(clients[startIndex+i], &socketSet))
								{
									//check for disconnect
									bool skip = isWaitingOnRead(startIndex+i);

									networkMutex.lock();
									char c = '\0';
									int status = recv(clients[startIndex+i], &c, 1, MSG_PEEK);
									networkMutex.unlock();
									
									if(status <= 0)
									{
										#ifdef __unix__
											if(errno == EWOULDBLOCK)
											{
												//Do nothing. Acceptable error
											}
											else
											{
												//disconnect
												std::function<void(int)> cpy = getDisconnectFunc();
												if(cpy!=nullptr)
													cpy(startIndex+i);
												removeSocket(clients[startIndex+i]);
												break;
											}
										#else
											int finalErr = WSAGetLastError();
											if(finalErr == WSAEWOULDBLOCK)
											{
												//Do nothing. Acceptable error
											}
											else
											{
												//disconnect
												std::function<void(int)> cpy = getDisconnectFunc();
												if(cpy!=nullptr)
													cpy(startIndex+i);
												removeSocket(clients[startIndex+i]);
												break;
											}
										#endif
									}
									else
									{
										if(!skip)
										{
											networkMutex.lock();
											waitingOnRead[startIndex+i] = true;
											networkMutex.unlock();

											//can receive message
											std::function<void(int)> cpy = getMessageArriveFunc();
											if(cpy!=nullptr)
												cpy(startIndex+i);
										}
									}
									
								}
							}
						}
						startIndex+=currSize;
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
						#ifdef __unix__
							int lastError = errno;
							wouldConnect = (lastError == EISCONN);
						#else
							int lastError = GetLastError();
							wouldConnect = (GetLastError() == WSAEISCONN);
						#endif

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

					FD_ZERO(&socketSet);
					FD_SET(connections[0], &socketSet);

					int retVal = select(connections[0]+1, &socketSet, NULL, NULL, &waitTime);
					bool isSet = FD_ISSET(connections[0], &socketSet);
					networkMutex.unlock();

					//Note retVal of 0 means timeout
					if(retVal>0)
					{
						if(isSet)
						{
							//check for disconnect
							networkMutex.lock();
							char c = '\0';
							int status = recv(connections[0], (char*)&c, 1, MSG_PEEK);
							networkMutex.unlock();

							if(status <= 0)
							{
								//error occured or graceful exit
								//disconnected
								std::function<void(int)> cpy = getDisconnectFunc();
								#ifdef __unix__
									if(errno == EWOULDBLOCK)
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
								#else
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
								#endif
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
					else if(retVal<0)
					{
						//error occured
						//disconnected
						std::function<void(int)> cpy = getDisconnectFunc();
						#ifdef __unix__
							if(errno == EWOULDBLOCK)
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
						#else
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
						#endif
							
					}
					FD_CLR(connections[0], &socketSet);

					//sleep for x time
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}

			}
			
		}
	}


} //NAMESPACE glib END