#pragma once
#include <queue>
#include "SmartMemory.h"
#include "Network.h"
#include "WebRequest.h"
#include "SimpleJobQueue.h"
#include "File.h"

//A bit on Range Size Limit:
//
//      This is the maximum size of a buffer to be sent. Must be greater than 64KB. A good size is about 16MB as shown below.
//
//      The following is done assuming 16MB size limit.
//      Assuming 10 milliseconds between sends and time for reading and loading a new job is negligable,
//          This hits 1.6 Gigabytes per second or 12.8 Gigabit per second
//      Adding in the other things for usb 2.0 hard drive speeds (about 140 Megabytes per second) and a max of 10 milliseconds between jobs (assuming no real load),
//          110 milliseconds to load data, 10 to get to the job, 10 milliseconds to send so approx = 130 milliseconds per 16MB
//          which is about 123 Megabytes per second or 984 Megabit per second
//          If sends are buffered, as long as you load in enough data into the socket and the time to load the data is fast enough, the network will be saturated.
//              IFF sends are buffered. (Data is not sent directly but put into a buffer for the kernel to send later).
//      Switching to a faster medium like an SATA SSD (about 550 Megabytes per second),
//          30 milliseconds to load data, 10 millis to get to the job, 10 millis to send so approx = 50 milliseconds per 16MB
//          which is about 320 Megabytes per second or 2560 Megabit per second

namespace glib
{
    class HttpServer
    {
    public:
        /**
         * @brief Construct a new Http Server object
         *      A basic HTTP Server that supports both GET and POST request.
         *      Should not be used for secure situations as it does not support HTTPS
         *          or most options HTTP options.
         *          Example: Keep-Alive does not work and always closes connection
         * 
         * @param ip 
         *      Must be the IP of the server being created.
         *      It can be the local IP, external IP, localhost or 127.0.0.1
         * @param port 
         *      The desired port to open the connection on.
         *      80 or 8080 are typical HTTP ports.
         * @param connectionsAllowed 
         *      The maximum simultaneous connections allowed.
         *      This refers to the number of sockets for the server to allow at a given time.
         *          Effectively, this limits the job queue since each request waits for a response before closing.
         *      A good default could be 1000 or a multiple of the number of threads.
         * @param threads
         *      The number of threads to use in the job queue.
         *      This does not have to be the same as the number of connections allowed and may be limited
         *          to the number of threads the system has.
         *      
         */
        HttpServer(std::string ip, int port, unsigned int connectionsAllowed, int threads);

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
        bool defaultGetFunction(WebRequest& req, size_t id);

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
        bool defaultGetFunction(WebRequest& req, File f, size_t id);

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
         *          size_t                      - The ID of the socket that sent the request. Used for sending data over the socket.
         * 
         * @param func 
         */
        void setGetFuncMapper(std::function<bool(HttpServer*, WebRequest&, std::vector<unsigned char>&, size_t)> func);

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
        void setPostFuncMapper(std::function<bool(HttpServer*, WebRequest&, std::vector<unsigned char>&, size_t)> func);

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
        
    private:
        SimpleJobQueue* jobQueue = nullptr;
        Network* conn = nullptr;
        std::mutex logMutex;
        std::mutex jobMutex;

        std::vector<SmartMemory<SLinkNode<std::function<void()>>>> jobPointers;
        std::vector<SmartMemory<SLinkNode<std::function<void()>>>> jobSendPointers;
        
        bool logInfo = false;
        bool redirectToIndex = true;
        bool assumeHtml = true;
        bool allowKeepAlive = true;

        int knownTimeout = 5000;
        unsigned int maxAgeCache = 300; //In seconds. Will be converted to string

        int rangeLimit = 0x1000000; //16MB - Should saturate any network thrown at it unless network speed is like more than 10 Gigabit

        void onConnection(size_t id);
        void onDataArrived(size_t id);
        void onDisconnection(size_t id);

        std::function<bool(HttpServer*, WebRequest&, std::vector<unsigned char>&, size_t)> getFuncMapper;
        std::function<bool(HttpServer*, WebRequest&, std::vector<unsigned char>&, size_t)> postFuncMapper;

        std::unordered_map<std::string, std::pair<unsigned int, std::string>> urlsAndAllowedMethodsAndHeaders;

        bool handleRecv(WebRequest& req, std::vector<unsigned char>& body, size_t id);
        bool sendFile(WebRequest& req, size_t id);
        bool sendFile(WebRequest& req, File f, size_t id);

        bool handleOptions(WebRequest& req, size_t id);

        void staggeredSend(size_t id, File f, size_t startPoint, size_t endPoint);
    };
}