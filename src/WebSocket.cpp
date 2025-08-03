#include "WebSocket.h"
#include "StringTools.h"
#include "System.h"


#ifndef NO_SOCKETS

namespace smpl
{
	WebSocket::WebSocket(bool type, int port, std::string location, int amountOfConnectionsAllowed)
	{
		this->type = type;
		this->location = location;
		this->port = port;

		NetworkConfig config;
		config.amountOfConnectionsAllowed = amountOfConnectionsAllowed;
		config.location = location;
		config.port = port;
		config.type = type;
		config.TCP = true;
		conn = new smpl::Network(config);
		
		srand(time(0));
		maskRandom = smpl::LCG(rand());

		packetQueue = std::vector<std::list<WebSocketPacket>>(amountOfConnectionsAllowed);
		clients = std::vector<ClientInfo>(amountOfConnectionsAllowed);
		
		conn->setOnConnectFunction([this](size_t id) ->void{
			optMutex.lock();
			//clear previous packet queue.
			this->packetQueue[id].clear();
			this->clients[id].connected = false;
			this->clients[id].buffer = false;
			this->clients[id].first = true;
			this->clients[id].timeSinceLastPing = System::getCurrentTimeMillis();
			this->clients[id].waitingResponse = false;
			this->clients[id].sentKeepAlive = false;

			if(this->type == TYPE_CLIENT)
			{
				//send upgrade request
				this->firstConnectClient(id);
			}
			
			//wait on first message received then call on the firstConnect
			optMutex.unlock();
		});

		conn->setOnDataAvailableFunction([this](size_t id) ->void{
			
			bool shouldCall = false;
			optMutex.lock();
			//check if it is the first message
			if(this->clients[id].first)
			{
				if(this->type == TYPE_SERVER)
				{
					//first message is a connection message requesting upgrade
					this->firstConnectServer(id);
					this->clients[id].connected = true;
					this->clients[id].first = false;
					this->clientsConnected++;

					if(this->onConnectFunc != nullptr)
						this->onConnectFunc(id);
				}
				else
				{
					//first message is a confirmation of the upgrade or refusal
					this->firstConnectClientPart2(id);
					this->clients[id].connected = true;
					this->clients[id].first = false;
					this->clientsConnected++;
					
					if(this->onConnectFunc != nullptr)
						this->onConnectFunc(id);
				}
			}
			else
			{
				//process the message into a WebSocketPacket.
				//calls packetArrivedFunction when neccessary.
				shouldCall = this->specialRecv(id);
				if(shouldCall == true)
					shouldCall = this->onNewPacketFunc != nullptr; //Must have a valid callback
			}
			clients[id].timeSinceLastPing = System::getCurrentTimeMillis();
			optMutex.unlock();
			
			if(shouldCall)
				this->onNewPacketFunc(id);
		});

		conn->setOnDisconnectFunction([this](size_t id) ->void{
			optMutex.lock();
			//Shouldn't have to do anything special here
			this->packetQueue[id].clear();
			this->clients[id].connected = false;
			this->clients[id].buffer = false;
			this->clients[id].first = true;
			this->clientsConnected--;

			if(this->onDisconnectFunc != nullptr)
				this->onDisconnectFunc(id);
			
			optMutex.unlock();
		});
	}

	WebSocket::~WebSocket()
	{
		if(conn != nullptr)
		{
			for(int i=0; i<clients.size(); i++)
			{
				if(clients[i].connected)
					conn->disconnect(i); //attempt graceful disconnect
			}

			conn->endNetwork();
			delete conn;
			conn = nullptr;

			packetQueue.clear();
			clients.clear();
		}
	}

	void WebSocket::startNetwork()
	{
		if(conn != nullptr)
			conn->startNetwork();
	}

	void WebSocket::setKeepAliveTime(size_t millis)
	{
		keepAliveTime = millis/2;
	}
	
	void WebSocket::keepConnectionsAlive()
	{
		if(conn == nullptr)
			return;
		
		optMutex.lock();
		for(size_t i=0; i<clients.size(); i++)
		{
			if(!clients[i].connected)
				continue;
			
			size_t timeWaited = System::getCurrentTimeMillis() - clients[i].timeSinceLastPing;
			//send ping or if they recently sent a ping, send pong
			if(clients[i].waitingResponse == false && timeWaited > keepAliveTime)
				sendPing(i);
			
			if(clients[i].waitingResponse == true)
			{
				timeWaited = System::getCurrentTimeMillis() - clients[i].timeSinceLastPing;
				if(timeWaited > keepAliveTime) //make adjustable
				{
					disconnect(i);
				}
			}
		}
		optMutex.unlock();
	}

	uint32_t WebSocket::getRandomMask()
	{
		//May not need this
		optMutex.lock();
		uint32_t word;
		uint8_t* bytes = (uint8_t*)&word;

		bytes[0] = maskRandom.get() % 255;
		bytes[1] = maskRandom.get() % 255;
		bytes[2] = maskRandom.get() % 255;
		bytes[3] = maskRandom.get() % 255;
		
		if(bytes[0]==0)
			bytes[0] += 7;
		if(bytes[1]==0)
			bytes[1] += 13;
		if(bytes[2]==0)
			bytes[2] += 17;
		if(bytes[3]==0)
			bytes[3] += 23;

		optMutex.unlock();
		return word;
	}
	
	bool WebSocket::sendPing(size_t id)
	{
		bool status = false;
		WebSocketPacket pingPacket;
		pingPacket.type = WebSocket::TYPE_PING_MSG;

		pingPacket.buffer.push_back(0b10001001); //last message and is a ping msg
		unsigned char tempBuffer = 'A';
		bool noErr = formBuffer(pingPacket.buffer, &tempBuffer, 1);
		if(noErr)
		{
			status = conn->sendMessage(pingPacket.buffer, id);
		}

		clients[id].sentKeepAlive = ClientInfo::TYPE_PING;
		clients[id].waitingResponse = true;
		clients[id].timeSinceLastPing = System::getCurrentTimeMillis();
		return status;
	}
	bool WebSocket::sendPong(size_t id, WebSocketPacket& p)
	{
		bool status = false;
		//p.buffer only contains application data
		WebSocketPacket pongPacket;
		pongPacket.type = WebSocket::TYPE_PONG_MSG;

		pongPacket.buffer.push_back(0b10001010); //last message and is a pong msg
		bool noErr = formBuffer(pongPacket.buffer, p.buffer.data(), p.buffer.size());
		if(noErr)
		{
			status = conn->sendMessage(pongPacket.buffer, id);
		}

		clients[id].sentKeepAlive = ClientInfo::TYPE_PONG;
		clients[id].waitingResponse = false;
		clients[id].timeSinceLastPing = System::getCurrentTimeMillis();
		return status;
	}

	
	bool WebSocket::formBuffer(std::vector<unsigned char>& outputBuffer, unsigned char* data, size_t len)
	{
		//send as a single and not fragmented cause I'm lazy
		unsigned int mask = 0;
		unsigned char tmp = 0; //No mask
		if(type == TYPE_CLIENT)
		{
			mask = maskRandom.get();
			tmp = 128;
		}

		if(len <= 125)
		{
			tmp |= (unsigned char)(len & 0xFF);
			outputBuffer.push_back(tmp);
		}
		else if(len <= 65535)
		{
			tmp |= 126;
			outputBuffer.push_back(tmp);
			outputBuffer.push_back((len>>8) & 0xFF);
			outputBuffer.push_back(len & 0xFF);
		}
		else if(len <= 0xFFFFFFFF)
		{
			tmp |= 127;
			outputBuffer.push_back(tmp);
			outputBuffer.push_back((len>>24) & 0xFF);
			outputBuffer.push_back((len>>16) & 0xFF);
			outputBuffer.push_back((len>>8) & 0xFF);
			outputBuffer.push_back(len & 0xFF);
		}
		else
		{
			return false; //unsupported size
		}

		if(type == TYPE_CLIENT)
		{
			outputBuffer.push_back((mask >> 24) & 0xFF);
			outputBuffer.push_back((mask >> 16) & 0xFF);
			outputBuffer.push_back((mask >> 8) & 0xFF);
			outputBuffer.push_back((mask >> 0) & 0xFF);
		}

		unsigned char* maskAsBytes = (unsigned char*)&mask;
		int maskIndex = 3;

		if(data != nullptr)
		{
			for(size_t i=0; i<len; i++)
			{
				outputBuffer.push_back( data[i] ^ maskAsBytes[maskIndex] );
				maskIndex -= 1;
				if(maskIndex < 0)
					maskIndex = 3;
			}
		}

		return true;
	}

	bool WebSocket::sendText(std::string s, size_t id)
	{
		//send as a single and not fragmented cause I'm lazy
		std::vector<unsigned char> sendBuffer;
		unsigned int mask = 0;
		bool status = false;

		optMutex.lock();
		if(conn == nullptr)
		{
			optMutex.unlock();
			return false;
		}
		if(id < 0 || id > clients.size())
		{
			optMutex.unlock();
			return false;
		}
		if(clients[id].connected == false)
		{
			optMutex.unlock();
			return false;
		}
		
		sendBuffer.push_back(0b10000001); //last message and is text
		bool err = formBuffer(sendBuffer, (unsigned char*)s.data(), s.size());

		if(!err)
			status = conn->sendMessage(sendBuffer, id);
		
		optMutex.unlock();
		return status;
	}

	bool WebSocket::sendRawData(std::vector<unsigned char> buffer, size_t id)
	{
		return sendRawData(buffer.data(), buffer.size(), id);
	}

	bool WebSocket::sendRawData(unsigned char* buffer, size_t size, size_t id)
	{	
		//send as a single and not fragmented cause I'm lazy
		std::vector<unsigned char> sendBuffer;
		unsigned int mask = 0;
		bool status = false;

		optMutex.lock();
		if(conn == nullptr)
		{
			optMutex.unlock();
			return false;
		}
		if(id < 0 || id > clients.size())
		{
			optMutex.unlock();
			return false;
		}
		if(clients[id].connected == false)
		{
			optMutex.unlock();
			return false;
		}

		sendBuffer.push_back(0b10000010); //last message and is text
		bool err = formBuffer(sendBuffer, buffer, size);

		if(!err)
			status = conn->sendMessage(sendBuffer, id);
		
		optMutex.unlock();
		return status;
	}

	WebSocketPacket WebSocket::receivePacket(size_t id)
	{
		optMutex.lock();
		if(id >= 0 && id < packetQueue.size())
		{
			if(packetQueue[id].size() > 0)
			{
				WebSocketPacket p = packetQueue[id].front();
				packetQueue[id].pop_front();
				
				optMutex.unlock();
				return p;
			}
		}
		optMutex.unlock();
		return { TYPE_UNKNOWN, {} };
	}

	WebSocketPacket WebSocket::peekPacket(size_t id)
	{
		optMutex.lock();
		if(id >= 0 && id < packetQueue.size())
		{
			if(packetQueue[id].size() > 0)
			{
				WebSocketPacket p = packetQueue[id].front();
				optMutex.unlock();
				return p;
			}
		}
		optMutex.unlock();
		return { TYPE_UNKNOWN, {} };
	}

	void WebSocket::setOnConnectFunction(std::function<void(size_t id)> func)
	{
		optMutex.lock();
		this->onConnectFunc = func;
		optMutex.unlock();
	}
	void WebSocket::setOnDisconnectFunction(std::function<void(size_t id)> func)
	{
		optMutex.lock();
		this->onDisconnectFunc = func;
		optMutex.unlock();
	}
	void WebSocket::setNewPacketFunction(std::function<void(size_t id)> func)
	{
		optMutex.lock();
		this->onNewPacketFunc = func;
		optMutex.unlock();
	}

	size_t WebSocket::getConnections()
	{
		optMutex.lock();
		size_t c = clientsConnected;
		optMutex.unlock();
		return c;
	}

	size_t WebSocket::getPacketsAvailable(size_t id)
	{
		size_t c;

		optMutex.lock();
		if(id >=0 && id < clients.size())
			c = packetQueue[id].size();
		optMutex.unlock();

		return c;
	}

	bool WebSocket::getConnected(size_t id)
	{	
		bool c = false;

		optMutex.lock();
		if(id >=0 && id < clients.size())
			c = clients[id].connected;
		optMutex.unlock();
		return c;
	}

	void WebSocket::firstConnectServer(size_t id)
	{
		if(conn == nullptr)
			return; //error
		//Get their HTTP GET Request
		std::vector<unsigned char> buffer = std::vector<unsigned char>(8192);
		bool err = this->conn->receiveMessage(buffer, id) <= 0;

		if(err)
			return; //error
		
		//Parse as WebRequest
		WebRequest rec = WebRequest(buffer);
		if(rec.getType() != WebRequest::TYPE_GET)
			return; //error
		
		//Form HTTP Response
		WebRequest response = WebRequest();
		response.setHeader(WebRequest::TYPE_SERVER, "HTTP/1.1 101 Switching Protocols", false);
		response.addKeyValue("Connection", "Upgrade");
		response.addKeyValue("Upgrade", "websocket");

		//MAGIC-STRING: 258EAFA5-E914-47DA-95CA-C5AB0DC85B11
		std::string base64Key = rec.readKeyValue("Sec-WebSocket-Key");
		base64Key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

		std::vector<uint32_t> newKey = Cryptography::SHA1((unsigned char*)base64Key.data(), base64Key.size());
		//The order of the bytes needs to be least to greatest so 0x00000001 to 0x01000000. A byte swap on each int.
		for(int i=0; i<5; i++)
			newKey[i] = StringTools::byteSwap(newKey[i]);
		
		std::string base64NewKey = smpl::StringTools::base64Encode((unsigned char*)newKey.data(), newKey.size()*4, false);

		response.addKeyValue("Sec-WebSocket-Accept", base64NewKey);
		int sentStuff = this->conn->sendMessage(response, id);
	}

	void WebSocket::firstConnectClient(size_t id)
	{
		if(conn == nullptr)
			return; //error

		//Make an HTTP GET Request

		//Parse as WebRequest
		WebRequest rec = WebRequest();
		rec.setHeader(WebRequest::TYPE_GET, "/", true);
		rec.addKeyValue("Host", location+":"+StringTools::toString(port));
		rec.addKeyValue("Connection", "keep-alive, Upgrade");
		rec.addKeyValue("Upgrade", "websocket");
		rec.addKeyValue("Accept", "*/*");
		rec.addKeyValue("Sec-Fetch-Dest", "websocket"); //May not be required
		rec.addKeyValue("Sec-Fetch-Mode", "websocket"); //May not be required
		rec.addKeyValue("Sec-Fetch-Site", "cross-site"); //May not be required
		rec.addKeyValue("Sec-WebSocket-Version", "13"); //Not sure if needed

		//generate 16 random bytes
		websocketKey.push_back(maskRandom.get());
		websocketKey.push_back(maskRandom.get());
		websocketKey.push_back(maskRandom.get());
		websocketKey.push_back(maskRandom.get());
		
		//encode into base64
		std::string base64 = StringTools::base64Encode((unsigned char*)websocketKey.data(), websocketKey.size()*4, false);

		rec.addKeyValue("Sec-WebSocket-Key", base64); //Required. Fix later
		this->conn->sendMessage(rec, id);
	}

	void WebSocket::firstConnectClientPart2(size_t id)
	{
		if(conn == nullptr)
			return; //error

		//recv from server, check if the message is correct. If not, close
		//gonna be lazy and just consider it fine
		
		//Get their HTTP GET Request. Have to get it to remove from the queue of things.
		std::vector<unsigned char> buffer = std::vector<unsigned char>();
		bool err = this->conn->receiveMessage(buffer, id) <= 0;
		
		if(err)
		{
			conn->disconnect();
			return; //error
		}
	}

	bool WebSocket::specialRecv(size_t id)
	{
		if(conn == nullptr)
			return false; //error
		
		//read the message in its entirety
		bool newPacket = true;
		WebSocketPacket p;
		p.type = TYPE_UNKNOWN;
		p.buffer = std::vector<unsigned char>();

		uint8_t header[2];
		this->conn->receiveMessage(header, 2, id, true);
		
		//parse
		bool fin = (header[0] >> 7) & 0x01; //1 bit
		int opCode = (header[0]) & 0x0F; // 4 bits (skipped 3 bits)
		bool mask = (header[1] >> 7) & 0x01; //1 bit
		uint32_t len = (header[1]) & 0b01111111; //7 bits

		uint8_t maskBytes[4];

		if(fin == false)
			this->clients[id].buffer = true;
		else
			this->clients[id].buffer = false;
		
		if(opCode == 0x00)
		{
			p = this->packetQueue[id].back();
			newPacket = false;
		}
		else if(opCode == 0x01)
		{
			p.type = TYPE_TEXT;
		}
		else if(opCode == 0x02)
		{
			p.type = TYPE_BINARY;
		}
		else if(opCode == 0x08)
		{
			p.type = TYPE_DISCONNECT;
			specialDisconnect(id);
			return false;
		}
		else if(opCode == 0x09)
		{
			p.type = TYPE_PING_MSG;
		}
		else if(opCode == 0x0A)
		{
			p.type = TYPE_PONG_MSG;
		}
		else
		{
			p.type = TYPE_UNKNOWN;
		}

		if(len == 126)
		{
			//read 2 more bytes then set length
			uint16_t extLen = 0;
			this->conn->receiveMessage((unsigned char*)&extLen, 2, id, true);
			len = extLen;
		}
		else if(len == 127)
		{
			//read 4 more bytes then set length
			this->conn->receiveMessage((unsigned char*)&len, 4, id, true);
		}

		if(mask)
		{
			this->conn->receiveMessage(maskBytes, 4, id, true);
		}

		size_t oldSize = p.buffer.size();
		p.buffer.resize( p.buffer.size() + len );
		this->conn->receiveMessage( (unsigned char*)&p.buffer[oldSize], len, id, true);
		if(mask)
		{
			int maskNum = 0;
			for(size_t i=oldSize; i<p.buffer.size(); i++)
			{
				p.buffer[i] = p.buffer[i] ^ maskBytes[maskNum];
				maskNum = (maskNum + 1) % 4;
			}
		}

		if(p.type == TYPE_PING_MSG)
		{
			sendPong(id, p);
			return false;
		}
		else if(p.type == TYPE_PONG_MSG)
		{
			clients[id].waitingResponse = false;
			return false;
		}

		if(newPacket)
		{
			this->packetQueue[id].push_back(p);
		}

		if(fin)
		{
			if(p.type == TYPE_TEXT || p.type == TYPE_BINARY)
			{
				return true;
			}
		}
		return false;
	}

	void WebSocket::disconnect(size_t id)
	{
		optMutex.lock();
		specialDisconnect(id);
		optMutex.unlock();
	}

	void WebSocket::specialDisconnect(size_t id)
	{
		if(conn == nullptr)
			return; //error
		
		std::vector<unsigned char> sendBuffer;
		sendBuffer.push_back(0b10001000); //final frame and close opcode
		sendBuffer.push_back(0b00000000); //No Mask (Error if mask is used.)
		this->conn->sendMessage(sendBuffer, id);
		
		//should wait to get the response.
		this->conn->disconnect(id);
	}
}

#endif