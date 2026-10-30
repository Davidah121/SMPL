#pragma once
#include "BuildOptions.h"
#include "StandardTypes.h"
#include "Network.h"
#include "WebRequest.h"
#include "URL.h"

namespace smpl
{
	class DLL_OPTION HttpClient
	{
	public:
		static const int TYPE_NO_ERROR = 0;
		static const int TYPE_HEADER_TOO_LARGE_ERROR = -1;
		static const int TYPE_RECEIVE_ERROR = -2;
		static const int TYPE_SEND_ERROR = -3;

		HttpClient(const URL& url);
		~HttpClient();

		bool isValid();
		bool getTimeoutOccurred();
		void setMaxBufferSize(unsigned int size);
		void setMaxRequestHeaderSize(unsigned int size);
		void setUserAgent(const std::string& str);

		void allowRedirection(bool v);
		void alwaysSendKeepAlive(bool v);
		void setRedirectLimit(unsigned int v);

		const URL& getURL();
		CookieManager& getCookies();

		void reconnect();
		void disconnect();

		/**
		* @brief A wrapper around network to send message data.
		* 
		* @param message 
		* @param id 
		* @return int 
		*/
		int sendMessage(const std::vector<unsigned char>& message);

		/**
		* @brief A wrapper around network to send message data.
		* 
		* @param msg 
		* @param id 
		* @return int 
		*/
		int sendMessage(const std::string& msg);

		/**
		* @brief A wrapper around network to send message data.
		*      Specifically for sending files which (if available) uses the faster
		*          sendfile function for sockets.
		* 
		* @param filename 
		* @param length 
		* @param offset 
		* @param id 
		* @return int 
		*/
		int sendFile(const File& filename, size_t length, size_t offset);

		/**
		* @brief Attempts to send a generic type. Sends it over as raw bytes.
		*      Equivalent to sendMessage((char*)&msg, sizeof(T), id)
		* 
		* @tparam T 
		* @param msg 
		* @param id 
		* @return int 
		*/
		template<typename T>
		int sendMessage(const T& msg)
		{
			//send raw bytes
			if(network != nullptr)
			{
				return network->sendMessage(msg, 0);
			}
			return -1;
		}

		/**
		* @brief Attempts to send a pointer to a generic type. Sends it over as raw bytes.
		*      Equivalent to sendMessage((char*)msg, sizeof(T)*elements, id)
		* 
		* @tparam T 
		* @param msg 
		* @param elements 
		* @param id 
		* @return int 
		*/
		template<typename T>
		int sendMessage(const T* msg, int elements)
		{
			//send raw bytes
			if(network != nullptr)
			{
				return network->sendMessage((char*)msg, sizeof(T)*elements, 0);
			}
			return -1;
		}
		

		
		bool sendRequest(const WebRequest& req);
		void waitOnConnect();
		bool waitOnDataReceived(bool handleRedirectsAutomatically = true);
		
		const WebRequest& getResponse();
		const std::vector<unsigned char>& getResponseBody();

		/**
		* @brief Determines whether the HttpClient needs to do an absolute redirect.
		*      These are not automatically handled like relative redirects and must be handled
		*      by the user.
		*      Absolute redirects go to a different webpage typically (though not required).
		*          Needed since https requires a different class.
		* 
		* @return true 
		* @return false 
		*/
		bool getShouldRedirect();

		/**
		* @brief Completes an absolute redirect.
		*      Upon completion, the request previously sent will be re-sent.
		*          The request re-sent may undergo some modifications.
		*          Specifically, the location may change and not just the host.
		* 
		*/
		void completeRedirect();

	private:
	
		void handleReceivedResponse(const WebRequest& response);

		void handleRedirect(const WebRequest& response);

		/**
		* @brief Chunked Transfer Encoding. This returns true if the end of all data has been reached (meaning no more data will be sent)
		*		If it is false, there is more data coming later or something went wrong.
		* 		It is NOT required that all bytes from the bodyBuffer are read and processed. It is possible that in order to properly read enough bytes, you need to wait
		*			for more bytes. This is the other scenario where the function may return false
		*
		* @param response 
		* @param bodyBuffer 
		* @param size 
		* @param readBytes
		* @return bool
		*/
		bool chunkedPreProcess(const WebRequest& response, unsigned char* bodyBuffer, size_t size);

		void internalOnDataAvailable();
		void setupNewConfig(const URL& url);
		void internalConnect();
		
		void resendRequest();
		
		NetworkConfig config;
		Network* network = nullptr;
		std::string userAgent = "SMPL C++ HttpClient";

		
		URL url;
		WebRequest response; //probably don't need this
		WebRequest sentRequest;
		CookieManager currentCookies;
		std::vector<unsigned char> responseBody;
		

		bool validRequest = false;
		bool shouldResendRequest = false;
		
		bool canRedirect = false;
		bool alwaysKeepAlive = false;
		unsigned int maxBufferSize = 0x10000;
		unsigned int maxHeaderSize = 0x2000;
		unsigned int redirectLimit = 12;
		int responseStatusCode = -1;

		std::atomic_bool waitingOnRedirect = false;
		std::atomic_bool doneReceiving = false;

		bool internalTimeout = false;

		int temporaryChunkSize = 0;
		int currentRedirectCount = 0;
		std::string chunkSizeAsString = "";

		int errorStatus = TYPE_NO_ERROR;
		std::vector<unsigned char> existingBuffer;
	};
} // namespace smpl

