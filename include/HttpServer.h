#pragma once
#include "BuildOptions.h"
#include "StandardTypes.h"
#include <queue>
#include "Network.h"
#include "SmartMemory.h"
#include "WebRequest.h"
#include "SimpleJobQueue.h"
#include "File.h"
#include "SimpleHashTable.h"


#ifndef NO_SOCKETS

namespace smpl
{
	class DLL_OPTION HttpServer
	{
	public:
		static const char STATUS_ERROR = 0;
		static const char STATUS_DONE = 1;
		static const char STATUS_CONTINUE = 2;

		/**
		* @brief Construct a new Http Server object
		*      A basic HTTP Server that supports GET, POST, HEAD, and OPTION requests.
		*      Can be used with HTTPS but be advised that it is not the most secure server possible.
		* 
		*      Max HTTP Header Size (Not the body) is 8192 bytes (8KB).
		*          Note that if the HTTP Header is larger than 8KB, a 413 error will be sent back to the client.
		*          Note that if the body does not match requested body size (is smaller) an error is logged but everything will continue as normal.
		* 
		* @param configuration
		*      The network configuration used for the TCP socket. 
		*      Must be TCP.
		*          Will force TCP otherwise.
		*      Must be Server.
		*          Will force Server otherwise.
		* @param workerThreads
		*      The number of threads to use in the job queue.
		*      This does not have to be the same as the number of connections allowed and may be limited
		*          to the number of threads the system has.
		* @param https
		*      Determines whether to create an SSL server or just a normal socket server.
		*          USE_OPENSSL must be defined in order for this option to work. It will fail otherwise.
		* @param certificateFile
		*      If you are creating an HTTPS server, this must be provided.
		*          This is the certificate which must be signed (perferrably signed by a CA) that will be sent on connection.
		*          This is the public key and may have a warning associated with it if it was not signed by a CA.
		* @param keyFile
		*      If you are creating an HTTPS server, this must be provided.
		*          This is the private key which must never be leaked. It is used for decryption.
		*/
		HttpServer(NetworkConfig configuration, int threads, bool useHTTPS = false, std::string certificateFile = "", std::string keyFile = "");

		/**
		* @brief Destroy the Http Server object
		* 
		*/
		~HttpServer();

		/**
		* @brief The default GET Request function.
		*      Handles Range Request and HEAD Request as well.
		*      This sends any file local to the server.
		*          403 ERROR for any file not local to the server.
		*          404 ERROR for any file that does not exist.
		* 
		*      Range Request are limited to a size specified by the RangeLimit.
		*          This does not affect normal GET request.
		*      Called if the GET function is not overriden.
		*      Can be called by overriden GET functions.
		* @param req 
		* @param id 
		* @return true 
		* @return false 
		*/
		char defaultGetFunction(WebRequest& req, std::string ip, size_t id);

		/**
		* @brief The default GET Request function.
		*      Handles Range Request and HEAD Request as well.
		*      This sends any file local to the server.
		*          403 ERROR for any file not local to the server.
		*          404 ERROR for any file that does not exist.
		* 
		*      Range Request are limited to a size specified by the RangeLimit.
		*          This does not affect normal GET request.
		*      Called if the GET function is not overriden.
		*      Can be called by overriden GET functions.
		* 
		* @param req 
		* @param f 
		*      The alternate file to use.
		*      This is used in place of whatever the user requested.
		*      Good for redirects that happen on the server side.
		*          Ex: Client asks for file /Storage/file1
		*              Server gets from C:/Documents/Storage/file1
		*      
		* @param id 
		* @return true 
		* @return false 
		*/
		char defaultGetFunction(WebRequest& req, File f, std::string ip, size_t id);

		/**
		* @brief Sends a 404 ERROR. 
		*      (Not Found Error)
		*      No webpage attached.
		* 
		* @param id 
		*/
		void send404Error(size_t id);

		/**
		* @brief Sends a 403 ERROR.
		*      (Forbidden Error)
		*      No webpage attached.
		* 
		* @param id 
		*/
		void send403Error(size_t id);

		/**
		* @brief Sends a 400 ERROR. 
		*      (Bad Request Error)
		*      No webpage attached.
		* 
		* @param id 
		*/
		void send400Error(size_t id);

		/**
		* @brief Sends a 413 ERROR.
		*      (Entity Too Large)
		*      No webpage attached.
		* 
		* @param id 
		*/
		void send413Error(size_t id);

		/**
		* @brief Sends a 416 ERROR. Also sends the invalid range back.
		* 
		* @param id 
		* @param range 
		*/
		void sendRangeError(size_t id, std::string range);

		/**
		* @brief Returns the Network object used for the socket connections.
		* 
		* @return Network* 
		*/
		Network* getNetworkConnection();

		/**
		* @brief Determines if the server is using HTTPS.
		*      It is HTTPS if the underlying socket server is SSL.
		* 
		* @return true 
		* @return false 
		*/
		bool isHTTPS();

		/**
		* @brief Starts the server.
		*      This is required so that the GET and POST functions can be
		*      overriden.
		* 
		*/
		void start();

		/**
		* @brief Gets whether the server is running
		* 
		* @return true 
		* @return false 
		*/
		bool getRunning();

		/**
		* @brief Sets if some information about the server should be logged
		*      into the console.
		*      Things like GET and POST request are logged along with the IP.
		* 
		* @param l 
		*/
		void setLogInfo(bool l);

		/**
		* @brief Gets whether information about the server is being logged.
		* 
		* @return true 
		* @return false 
		*/
		bool getLoggingInfo();

		/**
		* @brief Set the GET Function.
		*      This overrides the default function allowing things like redirects,
		*          or denying access to certain files even if they are local to the server.
		* 
		*      The return of the function should specify whether it was successful or not.
		* 
		*      The arguments for the function are as follows:
		*          HttpServer*                 - the server that called this function
		*          WebRequest&                 - The request that the server got
		*          std::vector<unsigned char>& - The body for the request that the server got. (May be empty)
		*		   std::string				   - 
		*          size_t                      - The ID of the socket that sent the request. Used for sending data over the socket.
		* 
		* @param func 
		*/
		void setGetFuncMapper(std::function<char(HttpServer*, const WebRequest&, const std::vector<unsigned char>&, const std::string, const size_t)> func);

		/**
		* @brief Set the POST Function.
		*      Used for sending data to the server.
		* 
		*      The return of the function should specify whether it was successful or not.
		* 
		*      The arguments for the function are as follows:
		*          HttpServer*                 - the server that called this function
		*          WebRequest&                 - The request that the server got
		*          std::vector<unsigned char>& - The body for the request that the server got. (May be empty)
		*          size_t                      - The ID of the socket that sent the request. Used for sending data over the socket.
		* 
		* @param func 
		*/
		void setPostFuncMapper(std::function<char(HttpServer*, const WebRequest&, const std::vector<unsigned char>&, const std::string, const size_t)> func);

		/**
		* @brief Sets a function that will map all other possible request types.
		*		GET, HEAD, POST, OPTION all are handled and have a corresponding function to map those to custom implementations.
		*		This allows the use of CREATE, DELETE, etc. along with potentially custom header types.
		*  
		*		The return of the function should specify whether it was successful or not.
		* 
		*		The arguments for the function are as follows:
		*			HttpServer*					- the server that called this function
		*			WebRequest&					- The request that the server got
		*			std::vector<unsigned char>&	- The body for the request that the server got. (May be empty)
		*			std::string					- The IP/web address for the request. (useful for blocking or special processing through logins as ID isn't enough)
		*			size_t						- The ID of the socket that sent the request. Used for sending data over the socket.
		* 
		* @param func 
		*/
		void setExtraRequestHandlerFuncMapper(std::function<char(HttpServer*, const WebRequest&, const std::vector<unsigned char>&, const std::string, const size_t)> func);
		
		/**
		* @brief Sets a function that will be called on WebRequest generated by the default functions that allows modification
		*      of the webrequest.
		*      Set things like Cookies here.
		*      
		* 
		*      The arguments for the function are as follows:
		*          HttpServer*                 - the server that called this function
		*          WebRequest&                 - The request that the server got
		*          size_t                      - The ID of the socket that sent the request. Used for sending data over the socket.
		*          WebRequest&                 - The request the server is going to send (modify this)
		* 
		* @param func 
		*/
		void setResponseHandlerFuncMapper(std::function<void(HttpServer*, const WebRequest&, const std::string, const size_t, WebRequest&)> func);

		/**
		* @brief Set the Options For a given URL.
		*      This notifies clients of what is or isn't allowed for a given URL.
		*      Required for browsers when using HTTP POST request when they are cross-origin.
		*          Especially if they use custom headers.
		*      
		*      The options for a URL that hasn't been specified are
		*          Access-Control-Allow-Methods: GET, HEAD, OPTIONS
		*      
		*      Cached for 5 minutes or 300 seconds.
		* 
		* @param url 
		* @param allowedRequestFlags 
		* @param allowedCustomHeaders 
		*/
		void setOptionsForURL(std::string url, unsigned int allowedRequestFlags, std::string allowedCustomHeaders);

		/**
		* @brief Set the Range Request Limit.
		*      This should be large enough to reduce the total number of request needed
		*          but small enough to reduce the time to complete a request.
		*      The default is 0x1000000 bytes or about 16MB.
		*          This may not be the best for every scenario and can be adjusted before running the server.
		*          Changing this while the server is running is not thread safe
		* 
		* @param bytes 
		*/
		void setRangeRequestLimit(int bytes);

		/**
		* @brief Sets whether the server assumes that .html or .htm are the extension for
		*      files without an extension.
		*      This is default behavior and expected behavior typically.
		* 
		* @param b 
		*/
		void setAssumeHTML(bool b);
		
		/**
		* @brief Gets whether the server assumes that a file without an extension is
		*      an .html or .htm file.
		*      This is default behavior
		* 
		* @return true 
		* @return false 
		*/
		bool getAssumeHTML();

		/**
		* @brief Sets the timeout length for the network.
		*      The specifics on implementation are in the Network class.
		* 
		*      If not greater than or equal to 0, persistent connections are not allowed.
		*          Meaning that each connection is closed after fulfilling its request.
		* 
		* @param millis 
		* 		Default is 5000
		* 			or 5 seconds
		*/
		void setTimeoutLength(long millis);

		/**
		* @brief Sets whether to Allow Persistent Connections
		*      This is the Keep-Alive option in HTTP request.
		* 
		*      All Keep-Alive request are subject to the timeout length.
		* 
		* @param b 
		*/
		void setAllowPersistentConnection(bool b);

		/**
		* @brief Sets whether to Allow Persistent Connections
		*      This is the Keep-Alive option in HTTP request.
		* 
		*      Useful to reduce the number of connections needed and have faster response times.
		* 
		* @return true 
		* @return false 
		*/
		bool getAllowPersistentConnection();

		/**
		* @brief Sets whether an empty GET request goes to index.html
		*      This is default and expected behavior typically.
		* 
		* @param b 
		*/
		void setRedirectToIndex(bool b);

		/**
		* @brief Gets whether an empty GET request goes to index.html
		* 
		* @return true 
		* @return false 
		*/
		bool getRedirectToIndex();

		/**
		* @brief Get the Range Request Limit
		*      Returns the maximum number of bytes that should be sent.
		*      This can be ignored but the default GET function does acknowledge this.
		* 
		* @return int 
		*/
		int getRangeRequestLimit();

		/**
		* @brief Fills a web request with some default parameters.
		*      Things like Keep-Alive, Date, etc.
		* 
		*/
		void fillGetResponseHeaders(WebRequest& req);
		
		
		//WRAPPERS AROUND NETWORK FOR EASIER OPERATION

		/**
		* @brief A wrapper around network to send a response header.
		*      Will apply user headers provided from the ExtraResponseHandler function.
		* 
		* @param response 
		* @param id 
		* @param req 
		* @return int 
		*/
		int sendResponse(WebRequest& response, const std::string ip, const size_t id, const WebRequest& req);

		/**
		* @brief A wrapper around network to send a response header.
		*      Skips any extra headers as its just an error. If those are required/needed, use the other
		*          function sendResponse()
		* 
		* @param response 
		* @param id 
		* @return int 
		*/
		int sendError(const WebRequest& response, size_t id);

		/**
		* @brief A wrapper around network to receive message data.
		* 
		* @param message 
		* @param id 
		* @return int 
		*/
		int receiveMessage(std::vector<unsigned char>& message, size_t id);

		/**
		* @brief A wrapper around network to receive message data.
		* 
		* @param msg 
		* @param id 
		* @return int 
		*/
		int receiveMessage(const std::string& msg, size_t id);


		/**
		* @brief Attempts to receive a generic type. Gets it as raw bytes.
		*      Equivalent to receiveMessage((char*)&msg, sizeof(T), id)
		* 
		* @tparam T 
		* @param msg 
		* @param id 
		* @return int 
		*/
		template<typename T>
		int receiveMessage(const T msg, size_t id)
		{
			//send raw bytes
			if(conn != nullptr)
			{
				return conn->receiveMessage((char*)&msg, sizeof(T), id);
			}
			return -1;
		}

		/**
		* @brief Attempts to receive a pointer to a generic type. Gets it as raw bytes.
		*      Equivalent to receiveMessage((char*)msg, sizeof(T)*elements, id)
		* 
		* @tparam T 
		* @param msg 
		* @param elements 
		* @param id 
		* @return int 
		*/
		template<typename T>
		int receiveMessage(const T* msg, int elements, size_t id)
		{
			//send raw bytes
			if(conn != nullptr)
			{
				return conn->receiveMessage((char*)msg, sizeof(T)*elements, id);
			}
			return -1;
		}

		/**
		* @brief A wrapper around network to peek at the network buffer.
		* 
		* @param buffer 
		* @param expectedSize 
		* @param id 
		* @return int 
		*/
		int peek(std::vector<unsigned char>& buffer, int expectedSize, size_t id);

		/**
		* @brief A wrapper around network to dump bytes that are in the received buffer.
		*      Used when the bytes aren't needed
		* 
		* @param bytesToDump 
		* @param id 
		* @return int 
		*/
		int dumpReceiveBytes(int bytesToDump, size_t id);

		/**
		* @brief A wrapper around network to check how many bytes are available to read.
		* 
		* @param id 
		* @return size_t 
		*/
		size_t getReadSizeAvailable(size_t id);

		/**
		 * @brief Attempts to send data with respect to range request and with respect to send limits.
		 *		This method will yield to other jobs running when necessary for fairness.
		 *			Assume it will yield after every limited send till it sends the entirety of the buffer.
		 * 		Upon return, returns a status value which could be STATUS_ERROR or STATUS_DONE
		 *			If an error occured, the client may have disconnected during the send.
		 *		Return that status value back to the GET handler which will handle disconnections
		 *
		 *		NOTE. It is HIGHLY important that startPoint and endPoint are valid ranges of the buffer.
		 *			These MUST be clamped to the actual size of the buffer.
		 *			endPoint is the index of the last valid value in the buffer.
		 *				A buffer of size 1 has a start point at 0 and end point at 0.
		 * @param id 
		 * @param buffer 
		 * @param startPoint 
		 * @param endPoint 
		 * @return char 
		 */
		char staggeredSend(size_t id, unsigned char* buffer, size_t startPoint, size_t endPoint);

		/**
		 * @brief Attempts to send a file with respect to range request and with respect to send limits.
		 *		The file method allows efficient sending of files specifically when available not having to
		 *			read a file into user space to then be sent. If its not available, this will be the same as staggeredSend.
		 *		This method will yield to other jobs running when necessary for fairness.
		 *			Assume it will yield after every limited send till it sends the entirety of the file.
		 * 		Upon return, returns a status value which could be STATUS_ERROR or STATUS_DONE
		 *			If an error occured, the client may have disconnected during the send.
		 *		Return that status value back to the GET handler which will handle disconnections
		 *
		 *		NOTE. It is HIGHLY important that startPoint and endPoint are valid ranges of the file.
		 *			These MUST be clamped to the actual size of the file.
		 * @param id 
		 * @param f 
		 * @param startPoint 
		 * @param endPoint 
		 * @return char 
		 */
		char staggeredSendFile(size_t id, File f, size_t startPoint, size_t endPoint);

	private:
		SimpleJobQueue* jobQueue = nullptr;
		Network* conn = nullptr;
		HybridSpinLock logMutex;
		HybridSpinLock jobMutex;

		SimpleHashMap< size_t, size_t > jobPointers;
		SimpleHashMap< size_t, bool > readAvailable;
		
		bool logInfo = false;
		bool redirectToIndex = true;
		bool assumeHtml = true;
		bool allowKeepAlive = true;

		int knownTimeout = 5000;
		unsigned int maxAgeCache = 300; //In seconds. Will be converted to string

		int rangeLimit = 0x1000000; //16MB - Should saturate any network thrown at it unless network speed is like more than 10 Gigabit

		void onConnection(std::string ip, size_t id);
		void onDataArrived(std::string ip, size_t id);
		void onDisconnection(std::string ip, size_t id);

		void removeJobInfo(size_t id);

		std::function<char(HttpServer*, const WebRequest&, const std::vector<unsigned char>&, const std::string, const size_t)> getFuncMapper;
		std::function<char(HttpServer*, const WebRequest&, const std::vector<unsigned char>&, const std::string, const size_t)> postFuncMapper;
		std::function<char(HttpServer*, const WebRequest&, const std::vector<unsigned char>&, const std::string, const size_t)> extraReqHandler;
		std::function<void(HttpServer*, const WebRequest&, const std::string, const size_t, WebRequest&)> extraResponseHandler;

		SimpleHashMap<std::string, std::pair<unsigned int, std::string>> urlsAndAllowedMethodsAndHeaders;

		char handleRecv(WebRequest& req, std::vector<unsigned char>& body, std::string ip, size_t id);
		char internalSendFile(WebRequest& req, std::string ip, size_t id);
		char internalSendFile(WebRequest& req, File f, std::string ip, size_t id);

		char handleOptions(WebRequest& req, std::string ip, size_t id);

	};
}

#endif