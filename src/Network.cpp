#include "Network.h"

#ifndef NO_SOCKETS

#include "StringTools.h"
#include <signal.h>
#include "System.h"

#ifdef _WIN32
	#include <io.h>
	#define crossPlatformPoll(socketInfo, size, timeout) WSAPoll(socketInfo, size, timeout)
	#define crossPlatformIoctl(socketInfo, request, mode) ioctlsocket(socketInfo, request, mode)
	#define crossPlatformClose(socketInfo) closesocket(socketInfo)
	#define crossPlatformGetLastError() GetLastError()

	#define crossPlatform_ConnectError WSAEISCONN
	#define crossPlatform_NoError NO_ERROR
	#define crossPlatform_WouldBlockError WSAEWOULDBLOCK
	#define crossPlatform_ShutdownSend SD_SEND
	#define crossPlatform_ShutdownRecv SD_RECEIVE
#else
	#define crossPlatformPoll(socketInfo, size, timeout) poll(socketInfo, size, timeout)
	#define crossPlatformIoctl(socketInfo, request, mode) ioctl(socketInfo, request, mode)
	#define crossPlatformClose(socketInfo) close(socketInfo)
	#define crossPlatformGetLastError() errno

	#define crossPlatform_ConnectError EISCONN
	#define crossPlatform_NoError 0
	#define crossPlatform_WouldBlockError EWOULDBLOCK
	#define crossPlatform_ShutdownSend SHUT_WR
	#define crossPlatform_ShutdownRecv SHUT_RD
#endif

namespace smpl
{
	unsigned int Network::totalNetworks = 0;

	Network::Network(NetworkConfig config, std::string certificateFile, std::string keyFile)
	{
		this->config = config;
		this->certificateFile = certificateFile;
		this->keyFile = keyFile;
		isSecureNetwork = config.secure;

		if(!init())
		{
			//error occured
			//throw error
		}
		else
		{
			setRunning(true);
			networkThread = std::thread(&Network::threadRun, this);
		}
	}

	Network::~Network()
	{
		inDispose = true;
		startNetwork(); //Must start the network if it has not ever been started.
		setRunning(false);

		if(networkThread.joinable())
			networkThread.join();
		
		disconnect();
		dispose();
		
		shouldStart = false;
		inDispose = false;
	}

	int Network::getPort()
	{
		return config.port;
	}

	std::string Network::getLocation()
	{
		return config.location;
	}

	
	SocketInfo* Network::getSocketInformation(size_t id)
	{
		if(config.type == Network::TYPE_CLIENT)
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
	
		#ifdef USE_OPENSSL
		SSL_Singleton sslSingleton = SSL_Singleton::getSingleton(); //init on main thread
		#endif

		return true;
	}

	void Network::initNetwork()
	{
		setupSocket();
		isSecureNetwork = config.secure;
		
		mainSocketInfo.socket = temporarySocket;
		mainSocketInfo.id = 0;
		mainSocketInfo.ip = config.location;
		mainSocketInfo.waitingOnRead = false;

		temporarySocket = 0;
	}

	
	void Network::sslInit()
	{
		if(isSecureNetwork && !inDispose)
		{
			#ifdef USE_OPENSSL
			SSL_Singleton sslSingleton = SSL_Singleton::getSingleton();

			SSL* conn = SSL_new(sslSingleton.getCTX());
			SSL_set_fd(conn, mainSocketInfo.socket);
			sslConnectionMapping.insert({mainSocketInfo.socket, conn});

			if(config.type == Network::TYPE_SERVER)
			{
				int err = SSL_CTX_use_certificate_file(sslSingleton.getCTX(), certificateFile.c_str(), SSL_FILETYPE_PEM);
				if(err != 1)
				{
					StringTools::println("FAILED SSL CERTIFICATE STUFF - %d", err);
				}
				err = SSL_CTX_use_PrivateKey_file(sslSingleton.getCTX(), keyFile.c_str(), SSL_FILETYPE_PEM);
				if(err != 1)
				{
					StringTools::println("FAILED SSL PRIVATEKEY STUFF - %d", err);
				}
				err = SSL_CTX_check_private_key(sslSingleton.getCTX());
				if(err != 1)
				{
					StringTools::println("FAILED SSL CHECK PRIVATEKEY - %d", err);
				}
			}
			#endif
		}
	}
	
	int Network::internalRecv(SOCKET_TYPE sock, char* buff, int len)
	{
		if(!isSecureNetwork)
		{
			#ifdef __unix__
				return recv(sock, buff, len, 0);
			#else
				WSABUF winBuff;
				winBuff.buf = buff;
				winBuff.len = len;
				DWORD bytesRead = 0;
				DWORD flag = 0;
				int err = WSARecv(sock, &winBuff, 1, &bytesRead, &flag, nullptr, nullptr);
				if(err == SOCKET_ERROR)
					return -1;
				return bytesRead;
			#endif
		}
		else
		{
			#ifdef USE_OPENSSL
			SSL* conn = getSSLFromSocket(sock);
			int readAmount = SSL_read(conn, buff, len);
			if(readAmount <= 0)
			{
				int err = SSL_get_error(conn, readAmount);
				if(err == SSL_ERROR_WANT_WRITE || err == SSL_ERROR_WANT_READ)
					readAmount = 0;
				else
					readAmount = -1;
			}
			return readAmount;
			#endif
		}

		return -1;
	}

	int Network::internalPeek(SOCKET_TYPE sock, char* buff, int len)
	{
		if(!isSecureNetwork)
		{
			#ifdef __unix__
				return recv(sock, buff, len, MSG_PEEK);
			#else
				WSABUF winBuff;
				winBuff.buf = buff;
				winBuff.len = len;
				DWORD bytesRead = 0;
				DWORD flag = MSG_PEEK;
				int err = WSARecv(sock, &winBuff, 1, &bytesRead, &flag, nullptr, nullptr);
				if(err == SOCKET_ERROR)
					return -1;
				return bytesRead;
			#endif
		}
		else
		{
			#ifdef USE_OPENSSL
			SSL* conn = getSSLFromSocket(sock);
			int readAmount = SSL_peek(conn, buff, len);
			if(readAmount <= 0)
			{
				int err = SSL_get_error(conn, readAmount);
				if(err == SSL_ERROR_WANT_WRITE || err == SSL_ERROR_WANT_READ)
					readAmount = 0;
				else
					readAmount = -1;
			}
			return readAmount;
			#endif
		}
		return -1;
	}

	int Network::internalSend(SOCKET_TYPE sock, char* buff, int len)
	{
		if(!isSecureNetwork)
		{
			#ifdef __unix__
				int flag = MSG_NOSIGNAL;
				return send(sock, buff, len, flag);
			#else
				int err = send(sock, buff, len, 0);
				if(err == SOCKET_ERROR)
					return -1;
				if(err == SOCKET_ERROR)
					if(crossPlatformGetLastError() == WSAEWOULDBLOCK)
						return 0;
				return err;
			#endif
		}
		else
		{
			#ifdef USE_OPENSSL
			SSL_Singleton singleton = SSL_Singleton::getSingleton();
			SSL* conn = getSSLFromSocket(sock);
			int sentAmount = SSL_write(conn, buff, len);
			
			if(sentAmount <= 0)
			{
				int err = SSL_get_error(conn, sentAmount);
				if(err == SSL_ERROR_WANT_WRITE || err == SSL_ERROR_WANT_READ)
					sentAmount = 0;
				else
				{
					StringTools::println("SEND: %s", ERR_error_string(ERR_get_error(), nullptr));
					sentAmount = -1;
				}
			}
			return sentAmount;
			#endif
		}
		return -1;
	}
	
	int Network::internalSendfile(SOCKET_TYPE sock, char* filename, long offset, size_t length)
	{
		int64_t bytesSent = length;
		if(!isSecureNetwork)
		{
			#ifdef __unix__
				int fd = open(filename, O_RDONLY | O_LARGEFILE);
				off_t truncOffset = offset;
				bytesSent = sendfile(sock, fd, &truncOffset, length);
				if(bytesSent == -1)
				{
					if(crossPlatformGetLastError() == EAGAIN)
						bytesSent = 0;
				}
				close(fd);
			#else
				HANDLE fileHandle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

				_LARGE_INTEGER temp;
				temp.QuadPart = offset;
				SetFilePointerEx(fileHandle, temp, nullptr, FILE_BEGIN);

				BOOL good = TransmitFile(sock, fileHandle, length, NULL, NULL, NULL, 0);
				if(good == FALSE)
					bytesSent = -1;
				CloseHandle(fileHandle);
			#endif
			return bytesSent;
		}
		else
		{
			#ifdef USE_OPENSSL
				SSL_Singleton singleton = SSL_Singleton::getSingleton();
				SSL* conn = getSSLFromSocket(sock);

				// #if OPENSSL_VERSION_NUMBER >= (3<<28)
				// 	#ifdef __unix__
				// 		int fd = open(filename, O_RDONLY | O_LARGEFILE);
				// 	#else
				// 		int fd = _open(filename, O_RDONLY, 0);
				// 	#endif

				// 	off_t truncOffset = offset;
				// 	bytesSent = SSL_sendfile(conn, fd, truncOffset, length, 0);
				// 	if(bytesSent <= 0)
				// 	{

				// 		int err = SSL_get_error(conn, bytesSent);
				// 		if(err == SSL_ERROR_WANT_WRITE || err == SSL_ERROR_WANT_READ)
				// 			bytesSent = 0;
				// 		else
				// 		{
				// 			StringTools::println("SENDFILE: %s", ERR_error_string(ERR_get_error(), nullptr));
				// 			bytesSent = -1;
				// 		}
				// 	}

				// 	#ifdef __unix__
				// 		close(fd);
				// 	#else
				// 		_close(fd);
				// 	#endif
				// #else
					//DO IT RAW
					FILE* f = fopen(filename, "rb");
					fseek(f, offset, SEEK_SET);
					std::vector<char> buffer = std::vector<char>(length);
					size_t bytesRead = fread(buffer.data(), 1, length, f);
					fclose(f);

					bytesSent = SSL_write(conn, buffer.data(), bytesRead);
					if(bytesSent <= 0)
					{
						int err = SSL_get_error(conn, bytesSent);
						if(err == SSL_ERROR_WANT_WRITE || err == SSL_ERROR_WANT_READ)
							bytesSent = 0;
						else
						{
							StringTools::println("SENDFILE: %s", ERR_error_string(ERR_get_error(), nullptr));
							bytesSent = -1;
						}
					}
				// #endif
				return bytesSent;
			#endif
		}
		
		return -2;
	}
	
    int Network::internalOnAccept(SOCKET_TYPE sock)
	{
		if(!isSecureNetwork)
			return 1;
		else
		{
			#ifdef USE_OPENSSL
			//new connection, add to mapping and call SSL_accept
			SSL* conn = SSL_new(SSL_Singleton::getSingleton().getCTX());
			SSL_set_fd(conn, sock);
			sslConnectionMapping.insert({sock, conn});

			int err = SSL_accept(conn);
			if(err <= 0)
			{
				int err2 = SSL_get_error(conn, err);
				if(err2 == SSL_ERROR_WANT_ACCEPT)
					err = 0;
				else
					err = -1;
			}
			return err;
			#endif
		}
		return -1;
	}
	
	int Network::internalOnConnect(SOCKET_TYPE sock)
	{
		if(!isSecureNetwork)
			return 1;
		else
		{
			#ifdef USE_OPENSSL
			//new connection, add to mapping and call SSL_connect
			SSL* conn = SSL_new(SSL_Singleton::getSingleton().getCTX());
			SSL_set_fd(conn, sock);
			sslConnectionMapping.insert({sock, conn});

			SSL_set_tlsext_host_name(conn, this->mainSocketInfo.ip.c_str());

			int err = SSL_connect(conn);
			if(err <= 0)
			{
				int err2 = SSL_get_error(conn, err);
				if(err2 == SSL_ERROR_WANT_CONNECT)
					err = 0;
				else
					err = -1;
			}
			return err;
			#endif
		}
		return -1;
	}

	void Network::internalOnDelete(SOCKET_TYPE sock)
	{
		if(isSecureNetwork)
		{
			#ifdef USE_OPENSSL
			//call SSL_shutdown and SSL_free
			//assume that the socket will be closed afterwards
			SSL* conn = getSSLFromSocket(sock);
			if(conn != nullptr)
			{
				SSL_shutdown(conn);
				SSL_free(conn);
			}
			
			//remove from mapping
			sslConnectionMapping.erase(sock);
			#endif
		}
	}

	#ifdef USE_OPENSSL
	SSL* Network::getSSLFromSocket(SOCKET_TYPE s)
	{
        auto it = sslConnectionMapping.find(s);
        auto endPoint = sslConnectionMapping.end();

        if(it != endPoint)
            return it->second;
        return nullptr;
	}
	#endif
	
	void Network::obtainLock(bool type)
	{
		if(type == LOCK_TYPE_IMPORTANT)
		{
			networkSemaphore.lock(HybridSpinSemaphore::TYPE_WRITE);
		}
		else
		{
			networkSemaphore.lock(HybridSpinSemaphore::TYPE_READ);
		}
	}

	void Network::releaseLock()
	{
		networkSemaphore.unlock();
	}

	void Network::createSocket(int fam, int sockType, int protocol)
	{
		temporarySocket = socket(fam, sockType, protocol);
	}


	void Network::setupSocket()
	{
		addrinfo hints;
		
		addrinfo* result;
		memset(&hints, 0, sizeof(hints));
		memset(&result, 0, sizeof(result));
		
		hints.ai_family = AF_INET;
		if(config.TCP)
		{
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
		}
		else
		{
			hints.ai_socktype = SOCK_DGRAM;
			hints.ai_protocol = IPPROTO_UDP;
		}
		
		if(getaddrinfo(config.location.c_str(), StringTools::toString(config.port).c_str(), &hints, &result) == 0)
		{
			createSocket(result->ai_family, result->ai_socktype, result->ai_protocol);
			
			if(temporarySocket != INVALID_SOCKET)
			{
				memcpy(&sockAddrInfo, result->ai_addr, sizeof(sockaddr));
				sizeAddress = result->ai_addrlen;
				
				if(config.type == TYPE_SERVER)
				{
					bindSocket();
				}
			}
			
			freeaddrinfo(result);
		}
	}

	bool Network::bindSocket()
	{
		if (config.type == Network::TYPE_SERVER)
		{
			int error = bind(temporarySocket, &sockAddrInfo, sizeAddress);

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
		if (config.type == Network::TYPE_SERVER)
		{
			::listen(mainSocketInfo.socket, SOMAXCONN);
			u_long mode = 1;
			crossPlatformIoctl(mainSocketInfo.socket, FIONBIO, &mode);
			
			// int i=1;
			// setsockopt(mainSocketInfo.socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&i, sizeof(i));
		}
	}

	bool Network::acceptConnection()
	{
		bool valid = false;
		bool secondaryStatus = true;

		if (config.type == Network::TYPE_SERVER)
		{
			SOCKET_TYPE tempSock = INVALID_SOCKET;
			tempSock = accept(mainSocketInfo.socket, &sockAddrInfo, &sizeAddress);
			valid = (tempSock > 0) && (tempSock != INVALID_SOCKET);
			
			if(tempSock == INVALID_SOCKET)
				return false;

			u_long mode = 1;
			crossPlatformIoctl(tempSock, FIONBIO, &mode);
			
			int err = 0;

			SocketInfo* inf = new SocketInfo();
			inf->socket = tempSock;
			inf->id = runningID;
			inf->waitingOnRead = false;
			inf->lastInteractTime = std::chrono::system_clock::now();

			sockaddr_in addr;
			socklen_t len = sizeof(sockaddr_in);
			err = getpeername(tempSock, (sockaddr*)&addr, &len);

			if(err == 0)
			{
				inf->ip = inet_ntoa(addr.sin_addr);
			}

			runningID = (runningID + 1) % (SIZE_MAX-1);
			connections.insert(std::pair<size_t, SocketInfo*>{inf->id, inf});

			
			while(true)
			{
				int secondaryFuncErr = internalOnAccept(inf->socket);
				if(secondaryFuncErr > 0)
				{
					secondaryStatus = true;
					break;
				}
				else if(secondaryFuncErr < 0)
				{
					secondaryStatus = false;
					break;
				}
				else
				{
					///should probably wait a little.
					System::sleep(1, 0, false);
				}
			}

			
			// int i=1;
			// setsockopt(tempSock, IPPROTO_TCP, TCP_NODELAY, (const char*)&i, sizeof(i));
		}

		
		return valid && secondaryStatus;
	}

	void Network::closeSocket()
	{
		obtainLock(LOCK_TYPE_IMPORTANT);

		for(std::pair<const size_t, SocketInfo*>& s : connections)
		{
			internalOnDelete(s.second->socket);
			crossPlatformClose(s.second->socket);

			delete s.second;
			s.second = 0;
		}
		connections.clear();
		
		if(mainSocketInfo.socket != 0)
		{
			internalOnDelete(mainSocketInfo.socket);
			crossPlatformClose(mainSocketInfo.socket);
		}

		temporarySocket = 0;
		mainSocketInfo = SocketInfo();
		isConnected = false;
		timeWaited = 0;
		shouldConnect = false;

		releaseLock();
	}

	bool Network::connect()
	{
		bool wouldConnect = false;
		bool secondaryStatus = true;
		
		if (config.type == Network::TYPE_CLIENT)
		{
			int err = ::connect(mainSocketInfo.socket, &sockAddrInfo, sizeAddress);
			mainSocketInfo.lastInteractTime = std::chrono::system_clock::now();

			int lastError = crossPlatformGetLastError();
			wouldConnect = (lastError == crossPlatform_ConnectError || lastError == crossPlatform_NoError);

			while(true)
			{
				int secondaryFuncErr = internalOnConnect(mainSocketInfo.socket);
				if(secondaryFuncErr > 0)
				{
					secondaryStatus = true;
					break;
				}
				else if(secondaryFuncErr < 0)
				{
					secondaryStatus = false;
					break;
				}
				else
				{
					///should probably wait a little.
					System::sleep(1, 0, false);
				}
			}
				
			if(mainSocketInfo.socket != INVALID_SOCKET)
			{
				u_long mode = 1;
				int err = crossPlatformIoctl(mainSocketInfo.socket, FIONBIO, &mode);
			}
		}

		return wouldConnect && secondaryStatus;
	}

	int Network::sendMessage(const std::string& message, size_t id)
	{
		return sendMessage((char*)message.c_str(), message.size()+1, id);
	}
	int Network::sendMessage(std::vector<unsigned char>& message, size_t id)
	{
		return sendMessage((char*)message.data(), message.size(), id);
	}
	int Network::sendMessage(unsigned char* message, int size, size_t id)
	{
		return sendMessage((char*)message, size, id);
	}
	
	int Network::sendMessage(WebRequest& message, size_t id)
	{
		std::string s = message.getRequestAsString();
		return sendMessage(s.data(), s.size(), id);
	}

	int Network::sendMessage(char * message, int messageSize, size_t id)
	{
		//It is okay to do this but makes no sense. No error though.
		if(messageSize == 0)
			return 0;
		
		obtainLock(LOCK_TYPE_NONIMPORTANT);
		SocketInfo* currSockInfo = getSocketInformation(id);

		if(currSockInfo == nullptr)
		{
			releaseLock();
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
			currSocketFD.revents = 0;
			int err = crossPlatformPoll(&currSocketFD, 1, 0);

			if( err == 0 )
			{
				releaseLock();
				//should probably sleep
				obtainLock(LOCK_TYPE_NONIMPORTANT);
				//verify socket is still valid
				SocketInfo* validSocketInfo = getSocketInformation(id);
				if(validSocketInfo == nullptr)
				{
					currSockInfo = nullptr;
					status = SOCKET_ERROR;
					break; //error. Socket deleted while trying to write to it.
				}
				
				continue;
			}
			else if( err < 0)
			{
				StringTools::println("POLL ERROR ON SEND");
				StringTools::println("%d", crossPlatformGetLastError());
				break; //POLL ERROR
			}

			if(currSocketFD.revents & POLLOUT)
			{
				status = internalSend(currSockInfo->socket, message + bytesWritten, messageSize - bytesWritten);
				
				if(status > 0)
					bytesWritten += status;
			}
			else
			{
				StringTools::println("POLLOUT ERROR ON SEND");
				break; //POLLOUT NOT AVAILABLE FOR SOME REASON. PROBABLY WON'T BE EITHER
			}

			if(bytesWritten >= messageSize)
			{
				break; //SENT ALL DATA
			}
			
			if(status < 0)
			{
				StringTools::println("SOCKET ERROR ON SEND");
				break; //SOME SOCKET ERROR
			}
		}

		//update last interact time
		if(currSockInfo != nullptr)
			currSockInfo->lastInteractTime = std::chrono::system_clock::now();
		
		releaseLock();
		
		if(status == 0 || status == SOCKET_ERROR)
		{
			//error, if 0, closed
			StringTools::println("SENDERR: %d", crossPlatformGetLastError());
			return -1;
		}
		return bytesWritten;
	}
	
	int Network::sendFile(char* filename, size_t length, size_t offset, size_t id)
	{
		int64_t bytesSent = length;
		obtainLock(LOCK_TYPE_NONIMPORTANT);
		SocketInfo* currSockInfo = getSocketInformation(id);

		if(currSockInfo == nullptr)
		{
			releaseLock();
			return -1; //SOCKET does not exist
		}

		bytesSent = internalSendfile(currSockInfo->socket, filename, offset, length);

		//update last interact time
		if(currSockInfo != nullptr)
			currSockInfo->lastInteractTime = std::chrono::system_clock::now();
		
		releaseLock();
		
		if(bytesSent < 0)
		{
			//error, if 0, closed
			// StringTools::println("%d", crossPlatformGetLastError());
			return -1;
		}

		return bytesSent;
	}

	int Network::receiveMessage(std::string& message, size_t id, bool flagRead)
	{
		//allocate buffer of x size. read till '\0'
		obtainLock(LOCK_TYPE_NONIMPORTANT);
		SocketInfo* currSockInfo = getSocketInformation(id);

		if(currSockInfo == nullptr)
		{
			releaseLock();
			return -1; //SOCKET IS CLOSED AND SET TO ZERO. CAN'T SEND
		}

		char* tempBuffer = new char[4096];
		std::memset(tempBuffer, 0, 4096);
		bool good = false;

		int status = SOCKET_ERROR;
		int bytesRead = 0;
		while(true)
		{
			status = internalPeek(currSockInfo->socket, tempBuffer, 4096);

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
					status = internalRecv(currSockInfo->socket, tempBuffer, 4096);
				}
				else
				{
					status = internalRecv(currSockInfo->socket, tempBuffer, bytesToRead);

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
		releaseLock();

		delete[] tempBuffer;

		if(bytesRead > 0)
			return bytesRead;
		else if(status < 0)
		{
			if(crossPlatformGetLastError() == crossPlatform_WouldBlockError)
				return 0;
			return -1;
		}
		else 
			return 0;
	}

	int Network::receiveMessage(std::vector<unsigned char>& buffer, size_t id, bool flagRead)
	{
		//allocate buffer of x size. read till status == 0
		
		obtainLock(LOCK_TYPE_NONIMPORTANT);
		SocketInfo* currSockInfo = getSocketInformation(id);

		if(currSockInfo == nullptr)
		{
			releaseLock();
			return -1; //SOCKET IS CLOSED AND SET TO ZERO. CAN'T SEND
		}

		char* tempBuffer = new char[4096];
		std::memset(tempBuffer, 0, 4096);
		bool good = false;

		int status = SOCKET_ERROR;
		int bytesRead = 0;
		while(true)
		{
			status = internalRecv(currSockInfo->socket, tempBuffer, 4096);
			
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
		releaseLock();

		delete[] tempBuffer;

		if(bytesRead > 0)
			return bytesRead;
		else if(status < 0)
		{
			if(crossPlatformGetLastError() == crossPlatform_WouldBlockError)
				return 0;
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
		obtainLock(LOCK_TYPE_NONIMPORTANT);
		SocketInfo* currSockInfo = getSocketInformation(id);

		if(currSockInfo == nullptr)
		{
			releaseLock();
			return -1; //SOCKET IS CLOSED AND SET TO ZERO. CAN'T SEND
		}

		int status = SOCKET_ERROR;
		status = internalRecv(currSockInfo->socket, buffer, bufferSize);
		
		if(flagRead)
			currSockInfo->waitingOnRead = false;
		
		//update last interact time		
		currSockInfo->lastInteractTime = std::chrono::system_clock::now();
		releaseLock();

		if(status > 0)
			return status;
		else if(status == 0)
			return 0;
		else
		{
			if(crossPlatformGetLastError() == crossPlatform_WouldBlockError)
				return 0;
			return -1;
		}
	}
	
	int Network::peek(std::vector<unsigned char>& buffer, int expectedSize, size_t id)
	{
		obtainLock(LOCK_TYPE_NONIMPORTANT);
		SocketInfo* currSockInfo = getSocketInformation(id);

		if(currSockInfo == nullptr)
		{
			releaseLock();
			return -1; //SOCKET IS CLOSED AND SET TO ZERO. CAN'T SEND
		}

		int status = SOCKET_ERROR;
		buffer.resize(expectedSize);
		status = internalPeek(currSockInfo->socket, (char*)buffer.data(), expectedSize);
		
		if(status > 0 && status < expectedSize)
		{
			buffer.resize(status);
		}
		
		//update last interact time		
		currSockInfo->lastInteractTime = std::chrono::system_clock::now();
		releaseLock();

		if(status > 0)
			return status;
		else if(status == 0)
			return 0;
		else
		{
			if(crossPlatformGetLastError() == crossPlatform_WouldBlockError)
				return 0;
			return -1;
		}
	}

	
	int Network::dumpReceiveBytes(int bytesToDump, size_t id)
	{
		obtainLock(LOCK_TYPE_NONIMPORTANT);
		SocketInfo* currSockInfo = getSocketInformation(id);

		if(currSockInfo == nullptr)
		{
			releaseLock();
			return -1; //SOCKET IS CLOSED AND SET TO ZERO. CAN'T SEND
		}

		int status = SOCKET_ERROR;
		const int buffSize = 4096;
		char buffer[buffSize];
		size_t totalBytesLeft = bytesToDump;
		while (totalBytesLeft > 0)
		{
			if(totalBytesLeft > buffSize)
				status = internalRecv(currSockInfo->socket, buffer, buffSize);
			else
				status = internalRecv(currSockInfo->socket, buffer, totalBytesLeft);
			
			if(status <= 0)
				break;

			totalBytesLeft -= status;
		}

		currSockInfo->waitingOnRead = false;
		
		//update last interact time		
		currSockInfo->lastInteractTime = std::chrono::system_clock::now();
		releaseLock();

		if(status > 0)
			return bytesToDump - totalBytesLeft;
		else if(status == 0)
			return bytesToDump - totalBytesLeft;
		else
		{
			if(crossPlatformGetLastError() == crossPlatform_WouldBlockError)
				return bytesToDump - totalBytesLeft;
			return -1;
		}
	}
	
	size_t Network::getReadSizeAvailable(size_t id)
	{
		obtainLock(LOCK_TYPE_NONIMPORTANT);

		SocketInfo* currSockInfo = getSocketInformation(id);

		if(currSockInfo == nullptr)
		{
			releaseLock();
			return 0; //SOCKET IS CLOSED AND SET TO ZERO. CAN'T SEND
		}

		unsigned long amount = 0;
		int err = crossPlatformIoctl(currSockInfo->socket, FIONREAD, &amount);

		releaseLock();

		if(err == 0)
			return amount;
		else
			return 0;
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
		obtainLock(LOCK_TYPE_IMPORTANT);
		shouldConnect = true;
		releaseLock();
	}

	void Network::disconnect()
	{
		obtainLock(LOCK_TYPE_IMPORTANT);
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
		if(config.type == Network::TYPE_CLIENT)
		{
			removeSocketInternal(mainSocketInfo.socket);
			mainSocketInfo = SocketInfo();
		}

		shouldConnect = false;
		isConnected = false;
		timeWaited = 0;
		releaseLock();
	}

	std::atomic_int disconnectCount = 0;
	void Network::disconnect(size_t id)
	{
		disconnectCount++;
		removeSocket(id);
	}

	std::string Network::getIPFromConnection(size_t id)
	{
		std::string s;

		obtainLock(LOCK_TYPE_NONIMPORTANT);
		SocketInfo* inf = getSocketInformation(id);
		if(inf != nullptr)
		{
			s = inf->ip;
		}
		releaseLock();

		return s;
	}

	size_t Network::getIDFromIP(std::string s)
	{
		size_t id = SIZE_MAX;
		obtainLock(LOCK_TYPE_NONIMPORTANT);
		for(std::pair<const size_t, SocketInfo*>& inf : connections)
		{
			if(inf.second->ip == s)
			{
				id = inf.first;
				break;
			}
		}
		releaseLock();
		return id;
	}

	void Network::removeSocket(size_t s)
	{
		obtainLock(LOCK_TYPE_IMPORTANT);

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
		releaseLock();
	}

	//With the emergence of the internalOnDelete() function, it may cause issues with the shutdown sequence.
	//This shutdown sequence can be removed if that is the case since it does not fix the main issues with linux
	//keeping the socket opened after this process.
	void Network::removeSocketInternal(SOCKET_TYPE s)
	{
		if(s != 0)
		{
			internalOnDelete(s);
			
			char junk[2048];
			//shutdown socket for sending
			int err = shutdown(s, crossPlatform_ShutdownSend);
			auto currTime = std::chrono::system_clock::now();
			while(true)
			{
				pollfd newFD = {};
				newFD.fd = s;
				newFD.events = POLLRDNORM;
				newFD.revents = POLLRDNORM;

				err = crossPlatformPoll(&newFD, 1, 1);

				if(err)
					break;
				
				if(newFD.revents | POLLRDNORM)
				{
					int status = internalRecv(s, junk, sizeof(junk));
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
			err = shutdown(s, crossPlatform_ShutdownRecv);
			err = crossPlatformClose(s);
		
		}
	}

	void Network::setOnConnectFunction(std::function<void(size_t)> func)
	{
		obtainLock(LOCK_TYPE_IMPORTANT);
		onConnectFunc = func;
		releaseLock();
	}
	void Network::setOnDataAvailableFunction(std::function<void(size_t)> func)
	{
		obtainLock(LOCK_TYPE_IMPORTANT);
		onDataAvailableFunc = func;
		releaseLock();
	}
	void Network::setOnDisconnectFunction(std::function<void(size_t)> func)
	{
		obtainLock(LOCK_TYPE_IMPORTANT);
		onDisconnectFunc = func;
		releaseLock();
	}

	std::function<void(size_t)> Network::getConnectFunc()
	{
		obtainLock(LOCK_TYPE_NONIMPORTANT);
		std::function<void(size_t)> cpy = onConnectFunc;
		releaseLock();
		return cpy;
	}
	std::function<void(size_t)> Network::getDataAvailableFunc()
	{
		obtainLock(LOCK_TYPE_NONIMPORTANT);
		std::function<void(size_t)> cpy = onDataAvailableFunc;
		releaseLock();
		return cpy;
	}
	std::function<void(size_t)> Network::getDisconnectFunc()
	{
		obtainLock(LOCK_TYPE_NONIMPORTANT);
		std::function<void(size_t)> cpy = onDisconnectFunc;
		releaseLock();
		return cpy;
	}

	void Network::setRunning(bool v)
	{
		obtainLock(LOCK_TYPE_IMPORTANT);
		running = v;
		releaseLock();
	}
	bool Network::getRunning()
	{
		obtainLock(LOCK_TYPE_NONIMPORTANT);
		bool v = running;
		releaseLock();
		return v;
	}

	bool Network::getClientConnected()
	{
		obtainLock(LOCK_TYPE_NONIMPORTANT);
		bool v = isConnected;
		releaseLock();
		return v;
	}
	
	bool Network::getTimeoutOccurred()
	{
		obtainLock(LOCK_TYPE_NONIMPORTANT);
		bool v = timeoutOccurred;
		releaseLock();
		return v;
	}
	
	void Network::setTimeoutLength(long millis)
	{
		obtainLock(LOCK_TYPE_IMPORTANT);
		timeoutTimer = millis;
		releaseLock();
	}
	
	long Network::getTimeoutLength()
	{
		obtainLock(LOCK_TYPE_NONIMPORTANT);
		long retVal = timeoutTimer;
		releaseLock();
		return retVal;
	}

	bool Network::getReconnect()
	{
		obtainLock(LOCK_TYPE_NONIMPORTANT);
		bool v = shouldConnect;
		releaseLock();
		return v;
	}

	void Network::startNetwork()
	{
		obtainLock(LOCK_TYPE_IMPORTANT);
		shouldStart = true;
		sslInit();
		releaseLock();
	}

	void Network::endNetwork()
	{
		closeSocket();
		setRunning(false);
	}
	
	size_t Network::getSocketsConnectedSize()
	{
		obtainLock(LOCK_TYPE_NONIMPORTANT);
		size_t v = connections.size();
		releaseLock();
		return v;
	}
	
	bool Network::isSecure()
	{
		return isSecureNetwork;
	}

	bool Network::getShouldStart()
	{
		obtainLock(LOCK_TYPE_NONIMPORTANT);
		bool v = shouldStart;
		releaseLock();
		return v;
	}

	void Network::setDoneReceiving(size_t id)
	{
		obtainLock(LOCK_TYPE_NONIMPORTANT);
		SocketInfo* inf = getSocketInformation(id);
		if(inf != nullptr)
			inf->waitingOnRead = false;
		releaseLock();
	}
	
	void Network::threadRun()
	{
		initNetwork(); //must now be done here for SSL purposes
		while(!getShouldStart())
		{
			System::sleep(1, 0, false);
		}

		if(config.type == TYPE_SERVER)
			runServer();
		else
			runClient();
	}
	
	void Network::runServer()
	{
		bool init = false;
		if(!init)
		{
			listen();
			init = true;
		}

		int counter = 0;
		while(getRunning())
		{
			//check for timeout
			bool didWork = true;
			std::vector<pollfd> allConnections;
			std::vector<size_t> allConnectionsID;
			std::vector<size_t> removeThese;
			std::vector<int> statusFlag;
			
			std::function<void(size_t)> onAcceptFunc = getConnectFunc();
			std::function<void(size_t)> onDataFunc = getDataAvailableFunc();

			obtainLock(LOCK_TYPE_IMPORTANT);

			pollfd socketFD = {};
			socketFD.fd = mainSocketInfo.socket;
			socketFD.events = POLLIN;
			socketFD.revents = 0;
			allConnections.push_back(socketFD);
			allConnectionsID.push_back(SIZE_MAX); //server socket ID = -1 since it doesn't actually have an ID
			statusFlag.push_back(0);

			for(auto sockInfo : connections)
			{
				if(sockInfo.second != nullptr)
				{
					bool flagForRemoval = false;
					if(timeoutTimer > 0)
					{
						auto t1 = std::chrono::system_clock::now();
						std::chrono::duration<double> elapsedTime = t1 - sockInfo.second->lastInteractTime;
						if(elapsedTime.count() > (double)timeoutTimer/1000)
						{
							flagForRemoval = true;
						}
					}

					pollfd socketFD = {};
					socketFD.fd = sockInfo.second->socket;
					socketFD.events = POLLIN;
					socketFD.revents = 0;
					allConnections.push_back(socketFD);
					allConnectionsID.push_back(sockInfo.first);
					if(!flagForRemoval)
						statusFlag.push_back(0);
					else
						statusFlag.push_back(1);
				}
			}
			
			int err = crossPlatformPoll(allConnections.data(), allConnections.size(), 1);
			releaseLock();

			if(err == 0)
			{
				//nothing ready
				didWork = false;
				continue;
			}
			else if(err < 0)
			{
				//error
				disconnect(); 
			}

			obtainLock(LOCK_TYPE_IMPORTANT);

			size_t currentConnectionIndex = 0;
			while(currentConnectionIndex < allConnections.size())
			{
				if(statusFlag[currentConnectionIndex] == 1) //already flagged for disconnect. skip
				{
					currentConnectionIndex++;
					continue;
				}

				if(allConnections[currentConnectionIndex].revents & POLLIN)
				{
					//something ready
					if(allConnectionsID[currentConnectionIndex] == SIZE_MAX)
					{
						//accept ready
						bool gotConnection = acceptConnection(); //uhhh should use this I guess.
						if(gotConnection)
						{
							if(onAcceptFunc!=nullptr)
								onAcceptFunc(runningID-1);
						}
					}
					else
					{
						//read ready
						SocketInfo* inf = getSocketInformation( allConnectionsID[currentConnectionIndex] );
						if(inf == nullptr)
						{
							currentConnectionIndex++;
							continue;
						}
						bool skip = inf->waitingOnRead == true;

						//check
						unsigned long readAmount = 0;
						int err = crossPlatformIoctl(allConnections[currentConnectionIndex].fd, FIONREAD, &readAmount);

						if(err == 0)
						{
							if(readAmount > 0 && !skip)
							{
								inf->lastInteractTime  = std::chrono::system_clock::now();
								inf->waitingOnRead = true;
								statusFlag[currentConnectionIndex] = 2; //READ AVAILABLE
							}
						}
						else
						{
							statusFlag[currentConnectionIndex] = 1; //Disconnect - POSSIBLE ERROR
						}
					}
				}
				else if(allConnections[currentConnectionIndex].revents & POLLERR || allConnections[currentConnectionIndex].revents & POLLHUP || allConnections[currentConnectionIndex].revents & POLLNVAL)
				{
					//error or disconnect
					statusFlag[currentConnectionIndex] = 1; //disconnect
					if(allConnectionsID[currentConnectionIndex] == SIZE_MAX)
					{
						//big problem. Main socket borken
						break;
					}
				}
				currentConnectionIndex++;
			}

			releaseLock();

			if(statusFlag[0] == 1) //main socket problem
			{
				//error
				disconnect();
				continue; //uhhh not sure what to do here yet
			}

			for(size_t i=1; i<allConnections.size(); i++)
			{
				if(statusFlag[i] == 1)
				{
					//disconnect
					disconnect(allConnectionsID[i]);
				}
				else if(statusFlag[i] == 2)
				{
					//data avail
					if(onDataFunc!=nullptr)
						onDataFunc(allConnectionsID[i]);
				}
			}
		}
	}
	
	void Network::runClient()
	{
		while(getRunning())
		{
			bool wouldConnect = false;
			size_t startConnectTime = System::getCurrentTimeMillis();
			while(getRunning())
			{
				if(getReconnect())
				{
					if(mainSocketInfo.socket == 0)
					{
						initNetwork();
					}

					bool testConnection = connect();
					if(testConnection)
					{
						obtainLock(LOCK_TYPE_IMPORTANT);
						isConnected = true;
						releaseLock();

						std::function<void(size_t)> cpy = getConnectFunc();
						
						if(cpy!=nullptr)
							cpy(0);
						break;
					}
					
					obtainLock(LOCK_TYPE_IMPORTANT);
					timeoutOccurred = false;
					long knownTimeoutLength = timeoutTimer;
					releaseLock();

					size_t timeWaited = (System::getCurrentTimeMillis() - startConnectTime);
					if(timeWaited >= knownTimeoutLength)
					{
						disconnect();
						obtainLock(LOCK_TYPE_IMPORTANT);
						timeoutOccurred = true;
						releaseLock();
					}
				}
				else
				{
					startConnectTime = System::getCurrentTimeMillis();
				}

				System::sleep(1, 0, false);
			}

			//MESSAGE
			while(getRunning())
			{
				bool didWork = true;
				obtainLock(LOCK_TYPE_IMPORTANT);

				pollfd mainSocket = {};
				mainSocket.fd = mainSocketInfo.socket;
				mainSocket.events = POLLIN;
				mainSocket.revents = 0;

				int err = crossPlatformPoll(&mainSocket, 1, 1);
				releaseLock();

				//Note retVal of 0 means timeout
				if(err>0)
				{
					//check for disconnect
					if(mainSocket.revents & POLLERR || mainSocket.revents & POLLHUP || mainSocket.revents & POLLNVAL)
					{
						//error occured or graceful exit
						//disconnected
						std::function<void(size_t)> cpy = getDisconnectFunc();

						int finalErr = crossPlatformGetLastError();
						int errChecking = crossPlatform_WouldBlockError;
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
					else if(mainSocket.revents & POLLIN)
					{
						//check for disconnect
						bool valid = true;
						obtainLock(LOCK_TYPE_IMPORTANT);

						unsigned long amountAvailable = 0;
						crossPlatformIoctl(mainSocket.fd, FIONREAD, &amountAvailable);

						bool skip = mainSocketInfo.waitingOnRead;

						releaseLock();

						if(!skip && amountAvailable > 0)
						{
							obtainLock(LOCK_TYPE_IMPORTANT);
							mainSocketInfo.waitingOnRead = true;
							mainSocketInfo.lastInteractTime = std::chrono::system_clock::now();
							releaseLock();

							//can receive message
							std::function<void(size_t)> cpy = getDataAvailableFunc();
							if(cpy!=nullptr)
								cpy(0);
						}
					
					}
				}
				else if(err<0)
				{
					//error occured
					//disconnected
					std::function<void(size_t)> cpy = getDisconnectFunc();
					
					int finalErr = crossPlatformGetLastError();
					int errChecking = crossPlatform_WouldBlockError;

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
					didWork = false;
				}
				
				System::sleep(1, 0, false);
			}
		}
	}

} //NAMESPACE glib END

#endif