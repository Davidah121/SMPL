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
		
		conn->setOnConnectFunction([this](int id) ->void{
			optMutex.lock();
			//clear previous packet queue.
			this->packetQueue[id].clear();
			this->clients[id].connected = false;
			this->clients[id].buffer = false;
			this->clients[id].first = true;

			if(this->type == TYPE_CLIENT)
			{
				//send upgrade request
				this->firstConnectClient(id);
			}
			
			//wait on first message received then call on the firstConnect
			optMutex.unlock();
		});

		conn->setOnDataAvailableFunction([this](int id) ->void{
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
				}
				else
				{
					//first message is a confirmation of the upgrade or refusal
					this->firstConnectClientPart2(id);
					this->clients[id].connected = true;
					this->clients[id].first = false;
					this->clientsConnected++;
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
			optMutex.unlock();
			
			this->onNewPacketFunc(id);
		});

		conn->setOnDisconnectFunction([this](int id) ->void{
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

	bool WebSocket::sendText(std::string s, int id)
	{
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
		
		//send as a single and not fragmented cause I'm lazy
		std::vector<unsigned char> sendBuffer;
		unsigned int mask = 0;
		sendBuffer.push_back(0b10000001); //last message and is text

		unsigned char tmp = 0; //No mask
		if(type == TYPE_CLIENT)
		{
			mask = maskRandom.get();
			tmp = 128;
		}
		
		size_t len = s.size();
		if(len <= 125)
		{
			tmp |= (unsigned char)(len & 0xFF);
			sendBuffer.push_back(tmp);
		}
		else if(len <= 65535)
		{
			tmp |= 126;
			sendBuffer.push_back(tmp);
			sendBuffer.push_back((len>>8) & 0xFF);
			sendBuffer.push_back(len & 0xFF);
		}
		else if(len <= 0xFFFFFFFF)
		{
			tmp |= 127;
			sendBuffer.push_back(tmp);
			sendBuffer.push_back((len>>24) & 0xFF);
			sendBuffer.push_back((len>>16) & 0xFF);
			sendBuffer.push_back((len>>8) & 0xFF);
			sendBuffer.push_back(len & 0xFF);
		}
		else
		{
			optMutex.unlock();
			return false; //unsupported size
		}

		if(type == TYPE_CLIENT)
		{
			sendBuffer.push_back((mask >> 24) & 0xFF);
			sendBuffer.push_back((mask >> 16) & 0xFF);
			sendBuffer.push_back((mask >> 8) & 0xFF);
			sendBuffer.push_back((mask >> 0) & 0xFF);
		}

		unsigned char* maskAsBytes = (unsigned char*)&mask;
		int maskIndex = 3;
		for(char c : s)
		{
			sendBuffer.push_back( (unsigned char)c ^ maskAsBytes[maskIndex] );
			maskIndex -= 1;
			if(maskIndex < 0)
				maskIndex = 3;
		}

		bool status = conn->sendMessage(sendBuffer, id);
		optMutex.unlock();
		return status;
	}

	bool WebSocket::sendRawData(std::vector<unsigned char> buffer, int id)
	{
		return sendRawData(buffer.data(), buffer.size(), id);
	}

	bool WebSocket::sendRawData(unsigned char* buffer, size_t size, int id)
	{	
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

		//send as a single and not fragmented cause I'm lazy
		std::vector<unsigned char> sendBuffer;
		unsigned int mask = 0;
		sendBuffer.push_back(0b10000010); //last message and is text

		unsigned char tmp = 0; //No mask
		if(type == TYPE_CLIENT)
		{
			mask = maskRandom.get();
			tmp = 128;
		}

		size_t len = size;
		if(len <= 125)
		{
			tmp |= (unsigned char)(len & 0xFF);
			sendBuffer.push_back(tmp);
		}
		else if(len <= 65535)
		{
			tmp |= 126;
			sendBuffer.push_back(tmp);
			sendBuffer.push_back((len>>8) & 0xFF);
			sendBuffer.push_back(len & 0xFF);
		}
		else if(len <= 0xFFFFFFFF)
		{
			tmp |= 127;
			sendBuffer.push_back(tmp);
			sendBuffer.push_back((len>>24) & 0xFF);
			sendBuffer.push_back((len>>16) & 0xFF);
			sendBuffer.push_back((len>>8) & 0xFF);
			sendBuffer.push_back(len & 0xFF);
		}
		else
		{
			optMutex.unlock();
			return false; //unsupported size
		}

		if(type == TYPE_CLIENT)
		{
			sendBuffer.push_back((mask >> 24) & 0xFF);
			sendBuffer.push_back((mask >> 16) & 0xFF);
			sendBuffer.push_back((mask >> 8) & 0xFF);
			sendBuffer.push_back((mask >> 0) & 0xFF);
		}

		unsigned char* maskAsBytes = (unsigned char*)&mask;
		int maskIndex = 3;

		for(size_t i=0; i<size; i++)
		{
			sendBuffer.push_back( (unsigned char)buffer[i] ^ maskAsBytes[maskIndex] );
			maskIndex -= 1;
			if(maskIndex < 0)
				maskIndex = 3;
		}

		bool status = conn->sendMessage(buffer, id);
		optMutex.unlock();
		return status;
	}

	WebSocketPacket WebSocket::receivePacket(int id)
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

	WebSocketPacket WebSocket::peekPacket(int id)
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

	void WebSocket::setOnConnectFunction(std::function<void(int a)> func)
	{
		optMutex.lock();
		this->onConnectFunc = func;
		optMutex.unlock();
	}
	void WebSocket::setOnDisconnectFunction(std::function<void(int a)> func)
	{
		optMutex.lock();
		this->onDisconnectFunc = func;
		optMutex.unlock();
	}
	void WebSocket::setNewPacketFunction(std::function<void(int a)> func)
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

	size_t WebSocket::getPacketsAvailable(int id)
	{
		size_t c;

		optMutex.lock();
		if(id >=0 && id < clients.size())
			c = packetQueue[id].size();
		optMutex.unlock();

		return c;
	}

	bool WebSocket::getConnected(int id)
	{	
		bool c = false;

		optMutex.lock();
		if(id >=0 && id < clients.size())
			c = clients[id].connected;
		optMutex.unlock();
		return c;
	}

	void WebSocket::firstConnectServer(int id)
	{
		if(conn == nullptr)
			return; //error

		//Get their HTTP GET Request
		std::vector<unsigned char> buffer = std::vector<unsigned char>();
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
		smpl::StringTools::println("Base64Key: ", base64Key.c_str());
		base64Key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

		std::vector<uint32_t> newKey = Cryptography::SHA1((unsigned char*)base64Key.data(), base64Key.size());
		//The order of the bytes needs to be least to greatest so 0x00000001 to 0x01000000. A byte swap on each int.
		for(int i=0; i<5; i++)
			newKey[i] = StringTools::byteSwap(newKey[i]);
		
		std::string base64NewKey = smpl::StringTools::base64Encode((unsigned char*)newKey.data(), newKey.size()*4, false);

		response.addKeyValue("Sec-WebSocket-Accept", base64NewKey);
		
		std::string responseStr = response.getRequestAsString();

		this->conn->sendMessage((unsigned char*)responseStr.data(), responseStr.size(), id);
	}

	void WebSocket::firstConnectClient(int id)
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

	void WebSocket::firstConnectClientPart2(int id)
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

	bool WebSocket::specialRecv(int id)
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

	void WebSocket::disconnect(int id)
	{
		optMutex.lock();
		specialDisconnect(id);
		optMutex.unlock();
	}

	void WebSocket::specialDisconnect(int id)
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