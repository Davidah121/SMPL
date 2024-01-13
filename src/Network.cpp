#include "Network.h"
#include "StringTools.h"
#include <signal.h>

namespace glib
{
	unsigned int Network::totalNetworks = 0;

	Network::Network(bool type, int port, std::string location, unsigned int amountOfConnectionsAllowed, bool TCP)
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

	int Network::getPort()
	{
		return port;
	}

	std::string Network::getLocation()
	{
		return location;
	}

	
	SocketInfo* Network::getSocketInformation(size_t id)
	{
		if(type == Network::TYPE_CLIENT)
			return &mainSocketInfo;
		else
		{
			auto it = connections.find(id);
			if(it != connections.end())
				return it->second;
			
			return nullptr;
		}
	}

	bool Network::init()
	{
		#ifdef __unix__
			signal(SIGPIPE, SIG_IGN);
		#endif
		
		#ifdef _WIN32
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
		}
		else if(type==TYPE_CLIENT)
		{
			createSocket(tcp);
			setupSocket();

			mainSocketInfo.socket = sock;
			mainSocketInfo.id = 0;
			mainSocketInfo.ip = location;
			mainSocketInfo.waitingOnRead = false;

			sock = 0;
		}


		setRunning(true);
	}

	void Network::createSocket(bool tcp)
	{
		if(tcp)
			sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		else
			sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		u_long mode = 1;
		#ifdef _WIN32
			int err = ioctlsocket(sock, FIONBIO, &mode);
		#else
			int err = ioctl(sock, FIONBIO, &mode);
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

			u_long mode = 1;
			#ifdef __unix__
				int err = ioctl(tempSock, FIONBIO, &mode);
			#else
				int err = ioctlsocket(tempSock, FIONBIO, &mode);
			#endif

			SocketInfo* inf = new SocketInfo();
			inf->socket = tempSock;
			inf->id = runningID;
			inf->waitingOnRead = false;
			inf->lastInteractTime = std::chrono::system_clock::now();

			sockaddr_in addr;
			int len = sizeof(sockaddr_in);
			#ifdef _WIN32
				err = getpeername(tempSock, (sockaddr*)&addr, &len);
			#else
				err = getpeername(tempSock, (sockaddr*)&addr, (socklen_t*)&len);
			#endif

			if(err == 0)
			{
				inf->ip = inet_ntoa(addr.sin_addr);
			}

			runningID = (runningID + 1) % (SIZE_MAX-1);
			connections.insert(std::pair<size_t, SocketInfo*>{inf->id, inf});
		}
	}

	void Network::closeSocket()
	{
		networkMutex.lock();

		for(std::pair<const size_t, SocketInfo*>& s : connections)
		{
			#ifdef __unix__
				close(s.second->socket);
			#else
				closesocket(s.second->socket);
			#endif

			delete s.second;
			s.second = 0;
		}
		connections.clear();
		
		#ifdef __unix__
			if(type == Network::TYPE_SERVER)
				close(sock);
			else
				close(mainSocketInfo.socket);
		#else
			if(type == Network::TYPE_SERVER)
				closesocket(sock);
			else
				closesocket(mainSocketInfo.socket);
		#endif
		sock = 0;
		mainSocketInfo = SocketInfo();
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
			status = ::connect(sock, (sockaddr*)&socketAddress, sizeAddress);
			mainSocketInfo.lastInteractTime = std::chrono::system_clock::now();
		}
	}

	int Network::sendMessage(std::string message, size_t id)
	{
		return sendMessage((char*)message.c_str(), message.size()+1, id);
	}
	int Network::sendMessage(std::vector<unsigned char> message, size_t id)
	{
		return sendMessage((char*)message.data(), message.size(), id);
	}
	int Network::sendMessage(unsigned char* message, int size, size_t id)
	{
		return sendMessage((char*)message, size, id);
	}

	int Network::sendMessage(char * message, int messageSize, size_t id)
	{
		//It is okay to do this but makes no sense. No error though.
		if(messageSize == 0)
			return 0;

		networkMutex.lock();

		SocketInfo* currSockInfo = getSocketInformation(id);

		if(currSockInfo == nullptr)
		{
			networkMutex.unlock();
			return -1; //SOCKET does not exist
		}

		int bytesWritten = 0;
		int status = SOCKET_ERROR;
		int flag = 0;
		#ifdef __unix__
		flag = MSG_NOSIGNAL;
		#endif
		
		while(true)
		{
			pollfd currSocketFD = {};
			currSocketFD.fd = currSockInfo->socket;
			currSocketFD.events = POLLOUT;
			currSocketFD.revents = POLLOUT;
			#ifdef _WIN32
				int err = WSAPoll(&currSocketFD, 1, 1000);
			#else
				int err = poll(&currSocketFD, 1, 1000);
			#endif

			if( err == 0 )
			{
				break; //TIMEOUT
			}
			else if( err < 0)
			{
				break; //POLL ERROR
			}

			if(currSocketFD.revents & POLLOUT)
			{
				status = send(currSockInfo->socket, message + bytesWritten, messageSize - bytesWritten, flag);
				
				if(status > 0)
					bytesWritten += status;
			}
			else
			{
				break; //POLLOUT NOT AVAILABLE FOR SOME REASON. PROBABLY WON'T BE EITHER
			}

			if(bytesWritten >= messageSize)
			{
				break; //SENT ALL DATA
			}
			
			if(status <= 0)
			{
				break; //SOME SOCKET ERROR
			}
		}

		//update last interact time		
		currSockInfo->lastInteractTime = std::chrono::system_clock::now();
		
		networkMutex.unlock();
		
		if(status == 0 || status == SOCKET_ERROR)
		{
			//error, if 0, closed
			return -1;
		}
		return bytesWritten;
	}

	int Network::receiveMessage(std::string& message, size_t id, bool flagRead)
	{
		//allocate buffer of x size. read till '\0'
		networkMutex.lock();

		SocketInfo* currSockInfo = getSocketInformation(id);

		if(currSockInfo == nullptr)
		{
			networkMutex.unlock();
			return -1; //SOCKET IS CLOSED AND SET TO ZERO. CAN'T SEND
		}

		char* tempBuffer = new char[4096];
		std::memset(tempBuffer, 0, 4096);
		bool good = false;

		int status = SOCKET_ERROR;
		int bytesRead = 0;
		while(true)
		{
			status = recv(currSockInfo->socket, tempBuffer, 4096, MSG_PEEK);

			if(status>0)
			{
				bytesRead += status;
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
					status = recv(currSockInfo->socket, tempBuffer, 4096, 0);
				}
				else
				{
					status = recv(currSockInfo->socket, tempBuffer, bytesToRead, 0);

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

		if(flagRead)
			currSockInfo->waitingOnRead = false;
		
		//update last interact time		
		currSockInfo->lastInteractTime = std::chrono::system_clock::now();

		networkMutex.unlock();

		delete[] tempBuffer;

		if(bytesRead > 0)
			return bytesRead;
		else if(status < 0)
		{
			#ifdef _WIN32
				if(GetLastError() == WSAEWOULDBLOCK)
					return 0;
			#else
				if(errno == EWOULDBLOCK)
					return 0;
			#endif
			return -1;
		}
		else 
			return 0;
	}

	int Network::receiveMessage(std::vector<unsigned char>& buffer, size_t id, bool flagRead)
	{
		//allocate buffer of x size. read till status == 0
		networkMutex.lock();

		SocketInfo* currSockInfo = getSocketInformation(id);

		if(currSockInfo == nullptr)
		{
			networkMutex.unlock();
			return -1; //SOCKET IS CLOSED AND SET TO ZERO. CAN'T SEND
		}

		char* tempBuffer = new char[4096];
		std::memset(tempBuffer, 0, 4096);
		bool good = false;

		int status = SOCKET_ERROR;
		int bytesRead = 0;
		while(true)
		{
			status = recv(currSockInfo->socket, tempBuffer, 4096, 0);
			
			if(status <= 0)
				break;
			
			bytesRead += status;
			for(int i=0; i<status; i++)
			{
				buffer.push_back(tempBuffer[i]);
			}
			good = true;
		}
		
		if(flagRead)
		{
			currSockInfo->waitingOnRead = false;
		}

		//update last interact time		
		currSockInfo->lastInteractTime = std::chrono::system_clock::now();

		networkMutex.unlock();

		delete[] tempBuffer;

		if(bytesRead > 0)
			return bytesRead;
		else if(status < 0)
		{
			#ifdef _WIN32
				if(GetLastError() == WSAEWOULDBLOCK)
					return 0;
			#else
				if(errno == EWOULDBLOCK)
					return 0;
			#endif
			return -1;
		}
		else 
			return 0;
	}

	int Network::receiveMessage(unsigned char* buffer, int bufferSize, size_t id, bool flagRead)
	{
		return receiveMessage((char*)buffer, bufferSize, id, flagRead);
	}

	int Network::receiveMessage(char * buffer, int bufferSize, size_t id, bool flagRead)
	{
		networkMutex.lock();

		SocketInfo* currSockInfo = getSocketInformation(id);

		if(currSockInfo == nullptr)
		{
			networkMutex.unlock();
			return -1; //SOCKET IS CLOSED AND SET TO ZERO. CAN'T SEND
		}

		int status = SOCKET_ERROR;
		status = recv(currSockInfo->socket, buffer, bufferSize, 0);
		
		if(flagRead)
			currSockInfo->waitingOnRead = false;
		
		//update last interact time		
		currSockInfo->lastInteractTime = std::chrono::system_clock::now();

		networkMutex.unlock();

		if(status > 0)
			return status;
		else if(status == 0)
			return 0;
		else
		{
			#ifdef _WIN32
				int lastErr = GetLastError();
				if(lastErr == WSAEWOULDBLOCK)
					return 0;
			#else
				if(errno == EWOULDBLOCK)
					return 0;
			#endif
			return -1;
		}
	}
	
	int Network::peek(std::vector<unsigned char>& buffer, int expectedSize, size_t id)
	{
		networkMutex.lock();

		SocketInfo* currSockInfo = getSocketInformation(id);

		if(currSockInfo == nullptr)
		{
			networkMutex.unlock();
			return -1; //SOCKET IS CLOSED AND SET TO ZERO. CAN'T SEND
		}

		int status = SOCKET_ERROR;
		buffer.resize(expectedSize);
		status = recv(currSockInfo->socket, (char*)buffer.data(), expectedSize, MSG_PEEK);
		
		if(status > 0 && status < expectedSize)
		{
			buffer.resize(status);
		}
		
		//update last interact time		
		currSockInfo->lastInteractTime = std::chrono::system_clock::now();
		
		networkMutex.unlock();

		if(status > 0)
			return status;
		else if(status == 0)
			return 0;
		else
		{
			#ifdef _WIN32
				if(GetLastError() == WSAEWOULDBLOCK)
					return 0;
			#else
				if(errno == EWOULDBLOCK)
					return 0;
			#endif
			return -1;
		}
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
		for(std::pair<const size_t, SocketInfo*>& inf : connections)
		{
			if(inf.second->socket != 0)
			{
				removeSocketInternal(inf.second->socket);
				inf.second->socket = 0;
			}

			delete inf.second;
			inf.second = nullptr;
		}
		connections.clear();

		//if client, close main socket
		if(type == Network::TYPE_CLIENT)
		{
			removeSocketInternal(mainSocketInfo.socket);
			mainSocketInfo = SocketInfo();
		}

		shouldConnect = false;
		isConnected = false;
		timeWaited = 0;
		networkMutex.unlock();
	}

	void Network::disconnect(size_t id)
	{
		removeSocket(id);
	}

	std::string Network::getIPFromConnection(size_t id)
	{
		std::string s;

		networkMutex.lock();
		SocketInfo* inf = getSocketInformation(id);
		if(inf != nullptr)
		{
			s = inf->ip;
		}
		networkMutex.unlock();

		return s;
	}

	size_t Network::getIDFromIP(std::string s)
	{
		size_t id = SIZE_MAX;
		networkMutex.lock();
		for(std::pair<const size_t, SocketInfo*>& inf : connections)
		{
			if(inf.second->ip == s)
			{
				id = inf.first;
				break;
			}
		}
		networkMutex.unlock();
		return id;
	}

	void Network::removeSocket(size_t s)
	{
		networkMutex.lock();

		SocketInfo* inf = getSocketInformation(s);
		if(inf != nullptr)
		{
			//call disconnect function
			if(onDisconnectFunc!=nullptr)
				onDisconnectFunc(inf->id);
			
			removeSocketInternal(inf->socket);
			delete inf;
			inf = nullptr;
		}

		//always try to remove
		connections.erase(s);
		networkMutex.unlock();
	}

	void Network::removeSocketInternal(SOCKET_TYPE s)
	{
		if(s != 0)
		{
			char junk[2048];
			//shutdown socket for send
			#ifdef _WIN32
				int err = shutdown(s, SD_SEND);
				auto currTime = std::chrono::system_clock::now();
				while(true)
				{
					pollfd newFD = {};
					newFD.fd = s;
					newFD.events = POLLRDNORM;
					newFD.revents = POLLRDNORM;

					err = WSAPoll(&newFD, 1, 1);

					if(err)
						break;
					
					if(newFD.revents | POLLRDNORM)
					{
						int status = recv(s, junk, sizeof(junk), 0);
						if(status <= 0)
							break;
					}

					auto timePassed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - currTime);
					if(timePassed.count() >= 30)
					{
						//waiting too long
						break;
					}
				}
				err = shutdown(s, SD_RECEIVE);
				err = closesocket(s);
			#else
				int err = shutdown(s, SHUT_WR);
				auto currTime = std::chrono::system_clock::now();
				while(true)
				{
					pollfd newFD = {};
					newFD.fd = s;
					newFD.events = POLLRDNORM;
					newFD.revents = POLLRDNORM;

					err = poll(&newFD, 1, 1);

					if(err)
						break;
					
					if(newFD.revents | POLLRDNORM)
					{
						int status = recv(s, junk, sizeof(junk), 0);
						if(status <= 0)
							break;
					}

					auto timePassed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - currTime);
					if(timePassed.count() >= 30)
					{
						//waiting too long
						break;
					}
				}

				err = shutdown(s, SHUT_RD);
				err = close(s);
			#endif
		}
	}

	void Network::setOnConnectFunction(std::function<void(size_t)> func)
	{
		networkMutex.lock();
		onConnectFunc = func;
		networkMutex.unlock();
	}
	void Network::setOnDataAvailableFunction(std::function<void(size_t)> func)
	{
		networkMutex.lock();
		onDataAvailableFunc = func;
		networkMutex.unlock();
	}
	void Network::setOnDisconnectFunction(std::function<void(size_t)> func)
	{
		networkMutex.lock();
		onDisconnectFunc = func;
		networkMutex.unlock();
	}

	std::function<void(size_t)> Network::getConnectFunc()
	{
		networkMutex.lock();
		std::function<void(size_t)> cpy = onConnectFunc;
		networkMutex.unlock();
		return cpy;
	}
	std::function<void(size_t)> Network::getDataAvailableFunc()
	{
		networkMutex.lock();
		std::function<void(size_t)> cpy = onDataAvailableFunc;
		networkMutex.unlock();
		return cpy;
	}
	std::function<void(size_t)> Network::getDisconnectFunc()
	{
		networkMutex.lock();
		std::function<void(size_t)> cpy = onDisconnectFunc;
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
	
	void Network::setTimeoutLength(long millis)
	{
		networkMutex.lock();
		timeoutTimer = millis;
		networkMutex.unlock();
	}
	
	long Network::getTimeoutLength()
	{
		networkMutex.lock();
		long retVal = timeoutTimer;
		networkMutex.unlock();
		return retVal;
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

	
	size_t Network::getSocketsConnectedSize()
	{
		networkMutex.lock();
		size_t v = connections.size();
		networkMutex.unlock();
		return v;
	}

	bool Network::getShouldStart()
	{
		networkMutex.lock();
		bool v = shouldStart;
		networkMutex.unlock();
		return v;
	}

	void Network::setDoneReceiving(size_t id)
	{
		networkMutex.lock();
		SocketInfo* inf = getSocketInformation(id);
		if(inf != nullptr)
			inf->waitingOnRead = false;
		networkMutex.unlock();
	}
	
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

						#ifdef _WIN32
						int err = WSAPoll(&listeningSocketFD, 1, 1);
						#else
						int err = poll(&listeningSocketFD, 1, 1);
						#endif

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
							std::function<void(size_t)> cpy = getConnectFunc();

							if(cpy!=nullptr)
								cpy(runningID-1);
						}
						else if(err<0)
						{
							//error occured
							disconnect();
						}
					}

					networkMutex.lock();

					//check for timeout

					std::vector<pollfd> clientConnections;
					std::vector<size_t> clientConnectionsID;
					std::vector<size_t> removeThese;
					
					for(std::pair<const size_t, SocketInfo*>& inf : connections)
					{
						if(inf.second == nullptr)
							continue;
						
						if(timeoutTimer > 0)
						{
							auto t1 = std::chrono::system_clock::now();
							std::chrono::duration<double> elapsedTime = t1 - inf.second->lastInteractTime;
							if(elapsedTime.count() > (double)timeoutTimer/1000)
							{
								removeThese.push_back(inf.first);
								continue;
							}
						}

						pollfd newFD = {};
						newFD.fd = inf.second->socket;
						newFD.events = POLLRDNORM;
						newFD.revents = POLLRDNORM;
						
						clientConnections.push_back( newFD );
						clientConnectionsID.push_back( inf.first );
					}

					networkMutex.unlock();

					//remove timed out sockets
					for(size_t idToRemove : removeThese)
					{
						disconnect(idToRemove);
					}
					
					networkMutex.lock();
					std::vector<int> statusFlag = std::vector<int>(clientConnections.size());

					#ifdef _WIN32
					int err = WSAPoll(clientConnections.data(), clientConnections.size(), 1);
					#else
					int err = poll(clientConnections.data(), clientConnections.size(), 1);
					#endif

					if(err>0)
					{
						for(size_t i=0; i<clientConnections.size(); i++)
						{
							//check for disconnect
							if(clientConnections[i].revents & POLLERR || clientConnections[i].revents & POLLHUP || clientConnections[i].revents & POLLNVAL)
							{
								//error occured or graceful exit
								//disconnected
								#ifdef _WIN32
								int finalErr = WSAGetLastError();
								int errChecking = WSAEWOULDBLOCK;
								#else
								int finalErr = errno;
								int errChecking = EWOULDBLOCK;
								#endif

								if(finalErr == errChecking)
								{
									//Do nothing. Acceptable error
									statusFlag[i] = 0; //nothing
								}
								else
								{
									statusFlag[i] = 1; //disconnect
								}
							}
							else if(clientConnections[i].revents & POLLRDNORM)
							{
								//check for disconnect
								bool valid = true;

								char testChar = 0;
								int actualStatus = recv(clientConnections[i].fd, &testChar, 1, MSG_PEEK);
								valid = actualStatus > 0;

								if(!valid)
								{
									//error occured or graceful exit
									//disconnected

									#ifdef _WIN32
									int finalErr = WSAGetLastError();
									int errChecking = WSAEWOULDBLOCK;
									#else
									int finalErr = errno;
									int errChecking = EWOULDBLOCK;
									#endif

									if(finalErr == errChecking)
									{
										//Do nothing. Acceptable error
										statusFlag[i] = 0; //nothing
									}
									else
									{
										statusFlag[i] = 1; //disconnect
									}
								}
								else
								{
									SocketInfo* inf = getSocketInformation( clientConnectionsID[i] );
									bool skip = inf->waitingOnRead == true;
									if(!skip)
									{
										inf->lastInteractTime = std::chrono::system_clock::now();
										inf->waitingOnRead = true;

										statusFlag[i] = 2; //read
									}
								}
							}
						}
						networkMutex.unlock();

						std::function<void(size_t)> recvFunc = getDataAvailableFunc();

						for(size_t i=0; i<clientConnections.size(); i++)
						{
							if(statusFlag[i] == 1)
							{
								//disconnect
								disconnect(clientConnectionsID[i]);
							}
							else if(statusFlag[i] == 2)
							{
								//data avail
								if(recvFunc!=nullptr)
									recvFunc(clientConnectionsID[i]);
							}
						}
					}
					else
					{
						networkMutex.unlock();
					}
					
					//sleep for x time
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
				
			}
			else
			{
				bool wouldConnect = false;
				while(getRunning())
				{
					if(getReconnect())
					{
						if(mainSocketInfo.socket == 0)
						{
							initNetwork(isTCP);
						}
						connect();

						#ifdef _WIN32
						int lastError = GetLastError();
						wouldConnect = (lastError == WSAEISCONN);
						#else
						int lastError = errno;
						wouldConnect = (lastError == EISCONN);
						#endif

						if(wouldConnect)
						{
							std::function<void(size_t)> cpy = getConnectFunc();
							
							if(cpy!=nullptr)
								cpy(0);
							break;
						}
						
						networkMutex.lock();
						timeoutOccurred = false;
						long knownTimeoutLength = timeoutTimer;
						networkMutex.unlock();

						timeWaited += 10;
						if(timeWaited >= knownTimeoutLength)
						{
							disconnect();
							networkMutex.lock();
							timeoutOccurred = true;
							networkMutex.unlock();
						}
					}

					//sleep for x time
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}

				//MESSAGE
				while(getRunning())
				{
					networkMutex.lock();

					pollfd mainSocket = {};
					mainSocket.fd = mainSocketInfo.socket;
					mainSocket.events = POLLRDNORM;
					mainSocket.revents = POLLRDNORM;

					#ifdef _WIN32
					int err = WSAPoll(&mainSocket, 1, 1);
					#else
					int err = poll(&mainSocket, 1, 1);
					#endif

					networkMutex.unlock();

					//Note retVal of 0 means timeout
					if(err>0)
					{
						//check for disconnect
						if(mainSocket.revents & POLLERR || mainSocket.revents & POLLHUP || mainSocket.revents & POLLNVAL)
						{
							//error occured or graceful exit
							//disconnected
							std::function<void(size_t)> cpy = getDisconnectFunc();

							#ifdef _WIN32
							int finalErr = WSAGetLastError();
							int errChecking = WSAEWOULDBLOCK;
							#else
							int finalErr = errno;
							int errChecking = EWOULDBLOCK;
							#endif

							if(finalErr == errChecking)
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
								std::function<void(size_t)> cpy = getDisconnectFunc();
								#ifdef _WIN32
								int finalErr = WSAGetLastError();
								int errChecking = WSAEWOULDBLOCK;
								#else
								int finalErr = errno;
								int errChecking = EWOULDBLOCK;
								#endif

								if(finalErr == errChecking)
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
								networkMutex.lock();
								bool skip = mainSocketInfo.waitingOnRead;
								networkMutex.unlock();

								if(!skip)
								{
									networkMutex.lock();
									mainSocketInfo.waitingOnRead = true;
									mainSocketInfo.lastInteractTime = std::chrono::system_clock::now();
									networkMutex.unlock();

									//can receive message
									std::function<void(size_t)> cpy = getDataAvailableFunc();
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
						std::function<void(size_t)> cpy = getDisconnectFunc();
						#ifdef _WIN32
						int finalErr = WSAGetLastError();
						int errChecking = WSAEWOULDBLOCK;
						#else
						int finalErr = errno;
						int errChecking = EWOULDBLOCK;
						#endif

						if(finalErr == errChecking)
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
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}

			}
			
		}
	}

} //NAMESPACE glib END