#pragma once
#include "BuildOptions.h"

#ifndef NO_SOCKETS

	#include "WebRequest.h"
	#include "Cryptography.h"
	#include "Network.h"
	#include "LCG.h"
	#include <vector>
	#include <list>

	namespace smpl
	{
		struct WebSocketPacket
		{
			int type = 0;
			std::vector<unsigned char> buffer;
		};

		struct ClientInfo
		{
			static const bool TYPE_PING = false;
			static const bool TYPE_PONG = true;
			
			bool connected = false;
			bool first = false;
			bool buffer = false;

			bool sentKeepAlive = TYPE_PING;
			bool waitingResponse = false;
			size_t timeSinceLastPing = 0;
		};

		class DLL_OPTION WebSocket
		{
		public:
			static const bool TYPE_CLIENT = smpl::Network::TYPE_CLIENT;
			static const bool TYPE_SERVER = smpl::Network::TYPE_SERVER;
			
			static const int TYPE_TEXT = 0;
			static const int TYPE_BINARY = 1;
			static const int TYPE_DISCONNECT = -1;
			static const int TYPE_UNKNOWN = -2;
			static const int TYPE_PING_MSG = 0x09;
			static const int TYPE_PONG_MSG = 0x0A;
			
			
			WebSocket(bool type, int port, std::string location, int amountOfConnectionsAllowed = 64);
			~WebSocket();

			void startNetwork();
			void keepConnectionsAlive();

			/**
			 * @brief Set the total keep alive time.
			 * 		Sends a ping within millis/2 and waits for a response within another millis/2
			 * 			Note that any activity with the client/server will reset the keep alive timer since there is active communication happening.
			 * 
			 * @param millis 
			 */
			void setKeepAliveTime(size_t millis);

			bool sendText(std::string s, size_t id=0);
			bool sendRawData(std::vector<unsigned char> buffer, size_t id=0);
			bool sendRawData(unsigned char* buffer, size_t size, size_t id=0);

			WebSocketPacket receivePacket(size_t id=0);
			WebSocketPacket peekPacket(size_t id=0);

			size_t getConnections();
			size_t getPacketsAvailable(size_t id=0);
			bool getConnected(size_t id=0);
			void disconnect(size_t id=0);

			void setOnConnectFunction(std::function<void(size_t id)> func);
			void setOnDisconnectFunction(std::function<void(size_t id)> func);
			void setNewPacketFunction(std::function<void(size_t id)> func);

		private:
			void firstConnectServer(size_t id);
			void firstConnectClient(size_t id);
			void firstConnectClientPart2(size_t id);
			bool specialRecv(size_t id);
			void specialDisconnect(size_t id);
			uint32_t getRandomMask();
			
			bool sendPing(size_t id);
			bool sendPong(size_t id, WebSocketPacket& packet);

			bool formBuffer(std::vector<unsigned char>& outputBuffer, unsigned char* data, size_t len);

			bool type = TYPE_SERVER;
			smpl::Network* conn = nullptr;
			std::function<void(size_t id)> onConnectFunc;
			std::function<void(size_t id)> onNewPacketFunc;
			std::function<void(size_t id)> onDisconnectFunc;

			std::vector< std::list<WebSocketPacket> > packetQueue;
			std::vector< ClientInfo > clients;
			size_t clientsConnected = 0;
			size_t keepAliveTime = 1000;

			smpl::LCG maskRandom = smpl::LCG();
			HybridSpinLock optMutex;
			std::string location;
			int port;
			std::vector<unsigned int> websocketKey;
		};
	}
	
#endif