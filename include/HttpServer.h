#pragma once
#include <queue>
#include "SmartMemory.h"
#include "Network.h"
#include "WebRequest.h"

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
         *      Since the server closes connections after satisfying them, this effectively 
         *          refers to the size of the request queue.
         *      A good default could be 1000 - 10000.
         */
        HttpServer(std::string ip, int port, int connectionsAllowed);

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
         */
        void defaultGetFunction(WebRequest& req, int id);

        /**
         * @brief Sends a 404 ERROR. No webpage attached.
         * 
         * @param id 
         */
        void send404Error(int id);

        /**
         * @brief Sends a 403 ERROR. No webpage attached.
         * 
         * @param id 
         */
        void send403Error(int id);

        /**
         * @brief Sends a 416 ERROR. Also sends the invalid range back.
         * 
         * @param id 
         * @param range 
         */
        void sendRangeError(int id, std::string range);

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
         * @param func 
         */
        void setGetFuncMapper(std::function<void(HttpServer*, WebRequest&, int)> func);

        /**
         * @brief Set the POST Function.
         *      Used for sending data to the server.
         * 
         * @param func 
         */
        void setPostFuncMapper(std::function<void(HttpServer*, WebRequest&, int)> func);

        /**
         * @brief Set the Range Request Limit.
         *      This should be large enough to reduce the total number of request needed
         *          but small enough to reduce the time to complete a request.
         *      The default is 0x400000 bytes or about 4MB.
         *          This may not be the best for every scenario and can be adjusted while
         *          the server is running.
         * 
         * @param bytes 
         */
        void setRangeRequestLimit(int bytes);

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
        
    private:
        Network* conn = nullptr;
        bool logInfo = false;
        bool redirectToIndex = true;
        int rangeLimit = 0x400000; //4MB

        std::function<void(HttpServer*, WebRequest&, int)> getFuncMapper;
        std::function<void(HttpServer*, WebRequest&, int)> postFuncMapper;

        void handleRecv(WebRequest& req, int id);
        void sendFile(WebRequest& req, int id);

    };
}