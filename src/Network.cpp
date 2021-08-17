#include "Network.h"
#include "System.h"
#include "MathExt.h"

namespace glib
{

	int Network::totalNetworks = 0;

	Network::Network(bool type, int port, std::string ipaddress, int amountOfConnectionsAllowed, bool TCP)
	{
		this->type = type;
		this->port = port;
		this->ipAddress = ipaddress;

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
				sock = NULL;

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

		if(networkThread.joinable())
			networkThread.join();
	}

	bool Network::init()
	{
		int status = WSAStartup(MAKEWORD(1, 1), &wsaData);
		Network::totalNetworks++;

		if (status != 0)
		{
			//error
			return false;
		}
		else
		{
			if (LOBYTE(wsaData.wVersion) == 1)
			{
				//Worked
				return true;
			}
			else
			{
				//Not correct version
				return false;
			}
		}
	}

	void Network::createSocket(bool tcp)
	{
		if(tcp)
			sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		else
			sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		u_long mode = 1;
		ioctlsocket(sock, FIONBIO, &mode);
	}


	void Network::setupSocket()
	{
		socketAddress.sin_port = htons(port);
		socketAddress.sin_family = AF_INET;
		
		inet_pton(AF_INET, ipAddress.c_str(), &socketAddress.sin_addr);
		//socketAddress.sin_addr.S_un.S_addr = inet_addr(ipAddress);
		sizeAddress = sizeof(socketAddress);
	}

	bool Network::bindSocket()
	{
		if (type == Network::TYPE_SERVER)
		{
			int error = bind(sock, (SOCKADDR*)(&socketAddress), sizeAddress);

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
			SOCKET tempSock = INVALID_SOCKET;

			while (tempSock == INVALID_SOCKET)
			{
				tempSock = accept(sock, (SOCKADDR*)&socketAddress, &sizeAddress);
			}
			
			connections.push_back(tempSock);
		}
	}

	void Network::closeSocket()
	{
		for(int i=0; i<connections.size(); i++)
		{
			closesocket(connections[i]);
		}
		
		closesocket(sock);
	}

	void Network::connect()
	{
		int status = SOCKET_ERROR;
		
		if (type == Network::TYPE_CLIENT)
		{
			status = ::connect(connections[0], (SOCKADDR*)(&socketAddress), sizeAddress);
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
		ZeroMemory(tempBuffer, 4096);
		bool good = false;

		while(true)
		{
			int status = SOCKET_ERROR;
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
						status = recv(connections[0], tempBuffer, 4096, NULL);
					else
						status = recv(connections[id], tempBuffer, 4096, NULL);
				}
				else
				{
					if(type == Network::TYPE_CLIENT)
						status = recv(connections[0], tempBuffer, bytesToRead, NULL);
					else
						status = recv(connections[id], tempBuffer, bytesToRead, NULL);

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

	bool Network::receiveMessage(std::vector<unsigned char> buffer, int id)
	{
		return receiveMessage((char*)buffer.data(), buffer.size(), id);
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

	void Network::dispose()
	{
		closeSocket();
		Network::totalNetworks--;
		if(Network::totalNetworks==0)
		{
			int status = WSACleanup();
			if (status != 0)
			{
				//error
			}
		}
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
			closesocket(connections[i]);
		}
		shouldConnect = false;
		isConnected = false;
		networkMutex.unlock();
	}

	void Network::removeSocket(SOCKET s)
	{
		networkMutex.lock();
		std::vector<SOCKET> nSockets;
		for(int i=0; i<connections.size(); i++)
		{
			if(connections[i]!=s)
				nSockets.push_back(connections[i]);
			else
				closesocket(s);
		}
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

	bool Network::getReconnect()
	{
		networkMutex.lock();
		bool v = shouldConnect;
		networkMutex.unlock();
		return v;
	}

	void Network::threadRun()
	{
		bool init = false;
		TIMEVAL waitTime = {0,1};
		FD_SET socketSet;

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
						FD_ZERO(&socketSet);
						FD_SET(sock, &socketSet);

						int retVal = select(NULL, &socketSet, NULL, NULL, &waitTime);
						
						//Note retVal of 0 means timeout
						if(retVal>0)
						{
							if(FD_ISSET(sock, &socketSet))
							{
								acceptConnection();
								std::function<void(int)> cpy = getConnectFunc();

								if(cpy!=nullptr)
									cpy(connections.size());
							}
						}
						else if(retVal!=0)
						{
							//error occured
							disconnect();
						}
						FD_CLR(sock, &socketSet);
					}

					int startIndex=0;
					std::vector<SOCKET> clients = connections;

					while(startIndex < connections.size())
					{
						int currSize = MathExt::min((int)clients.size() - startIndex, 64);
						FD_ZERO(&socketSet);

						for(int i=0; i<currSize; i++)
						{
							FD_SET(clients[i + startIndex], &socketSet);
						}

						int retVal = select(NULL, &socketSet, NULL, NULL, &waitTime);

						if(retVal>0)
						{
							for(int i=0; i<currSize; i++)
							{
								if(FD_ISSET(clients[startIndex+i], &socketSet))
								{
									//check for disconnect
									char c = '\0';
									int status = recv(clients[startIndex+i], &c, 1, MSG_PEEK);
									if(status != 1)
									{
										//disconnect
										std::function<void(int)> cpy = getDisconnectFunc();
										if(cpy!=nullptr)
											cpy(startIndex+i);
										removeSocket(clients[startIndex+i]);
										break;
									}
									else
									{
										//can receive message
										std::function<void(int)> cpy = getMessageArriveFunc();
										if(cpy!=nullptr)
											cpy(startIndex+i);
									}
									
								}
							}
						}
						startIndex+=currSize;
					}
					//sleep for x time
					System::sleep(0,1);
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
						int lastError = GetLastError();
						wouldConnect = (GetLastError() == WSAEISCONN);
						
						if(wouldConnect)
						{
							std::function<void(int)> cpy = getConnectFunc();
							
							if(cpy!=nullptr)
								cpy(0);
							break;
						}
						
					}
					//sleep for x time
					System::sleep(0,1);
				}

				//MESSAGE
				while(getRunning())
				{
					FD_ZERO(&socketSet);
					FD_SET(connections[0], &socketSet);

					int retVal = select(NULL, &socketSet, NULL, NULL, &waitTime);

					//Note retVal of 0 means timeout
					if(retVal>0)
					{
						if(FD_ISSET(connections[0], &socketSet))
						{
							std::function<void(int)> cpy = getMessageArriveFunc();
							if(cpy!=nullptr)
								cpy(0);
						}
					}
					else if(retVal!=0)
					{
						//error occured
						//disconnected
						std::function<void(int)> cpy = getDisconnectFunc();

						if(cpy!=nullptr)
							cpy(0);
						disconnect();
						break;
					}
					FD_CLR(connections[0], &socketSet);

					//sleep for x time
					System::sleep(0,1);
				}

			}
			
		}
	}

} //NAMESPACE glib END