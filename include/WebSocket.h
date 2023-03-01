#pragma once
#include "WebRequest.h"
#include "Cryptography.h"
#include "Network.h"
#include "LCG.h"

namespace glib
{
	struct WebSocketPacket
	{
		int type = 0;
		std::vector<unsigned char> buffer;
	};

	struct ClientInfo
	{
		bool connected = false;
		bool first = false;
		bool buffer = false;
	};

	class WebSocket
	{
	public:
		static const bool TYPE_CLIENT = glib::Network::TYPE_CLIENT;
		static const bool TYPE_SERVER = glib::Network::TYPE_SERVER;
		
		static const int TYPE_TEXT = 0;
		static const int TYPE_BINARY = 1;
		static const int TYPE_DISCONNECT = -1;
		static const int TYPE_UNKNOWN = -2;
		
		WebSocket(bool type, int port, std::string location, int amountOfConnectionsAllowed = 64);
		~WebSocket();

		void startNetwork();

		bool sendText(std::string s, int id=0);
		bool sendRawData(std::vector<unsigned char> buffer, int id=0);
		bool sendRawData(unsigned char* buffer, size_t size, int id=0);

		WebSocketPacket receivePacket(int id=0);
		WebSocketPacket peekPacket(int id=0);

		size_t getConnections();
		size_t getPacketsAvailable(int id=0);
		bool getConnected(int id=0);
		void disconnect(int id=0);

		void setOnConnectFunction(std::function<void(int a)> func);
		void setOnDisconnectFunction(std::function<void(int a)> func);
		void setNewPacketFunction(std::function<void(int a)> func);

	private:
		void firstConnect(int a);
		void specialRecv(int a);
		void specialDisconnect(int id);
		uint32_t getRandomMask();
		
		glib::Network* conn = nullptr;
		std::function<void(int a)> onConnectFunc;
		std::function<void(int a)> onNewPacketFunc;
		std::function<void(int a)> onDisconnectFunc;

		std::vector< std::list<WebSocketPacket> > packetQueue;
		std::vector< ClientInfo > clients;
		size_t clientsConnected = 0;
		glib::LCG maskRandom = glib::LCG();
		std::mutex optMutex;
	};
}