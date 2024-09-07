#pragma once
#include "BuildOptions.h"
#include "Network.h"
#include "WebRequest.h"

namespace smpl
{
    class DLL_OPTION WebClient
    {
    public:
        static const int TYPE_NO_ERROR = 0;
        static const int TYPE_HEADER_TOO_LARGE_ERROR = -1;
        static const int TYPE_RECEIVE_ERROR = -2;
        static const int TYPE_SEND_ERROR = -3;

        WebClient(std::string location);
        ~WebClient();

        bool isValid();
        bool getTimeoutOccurred();
        void setMaxBufferSize(int size);
        void setMaxRequestHeaderSize(int size);
        void setUserAgent(std::string str);

        void allowRedirection(bool v);
        void alwaysSendKeepAlive(bool v);
        void setRedirectLimit(int v);

        int getLastResponseCode();
        std::string getHost();
        std::string getWebname();

        void start();
        void reconnect();
        void disconnect();

        bool sendRequest(WebRequest& req);
        
        void setOnConnectFunc(std::function<void(WebClient*)> func);
        void setOnDisconnectFunc(std::function<void(WebClient*)> func);
        void setOnBufferChangedFunc(std::function<void(WebClient*, WebRequest& response, unsigned char*, size_t)> func);
        
        void handleReceivedResponse(WebRequest& response);

        /**
         * @brief Determines whether the WebClient needs to do an absolute redirect.
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
        void handleRedirect(WebRequest& response);
        bool chunkedPreProcess(WebRequest& response, unsigned char* bodyBuffer, size_t size);

        void internalOnDataAvailable();
        void setupNewConfig(std::string location);
        void internalConnect();
        
        void resendRequest();
        
        std::function<void(WebClient*)> onConnectionFunction;
        std::function<void(WebClient*)> onDisconnectionFunction;
        std::function<void(WebClient*, WebRequest& response, unsigned char*, size_t)> onBufferChanged;
        
        NetworkConfig config;

        Network* network = nullptr;
        bool started = false;
        std::string userAgent = "SMPL C++ WebClient";

        WebRequest response;
        WebRequest sentRequest;

        std::string hostStr = "";
        std::string webName = "";
        bool validRequest = false;
        bool shouldResendRequest = false;
        
        bool canRedirect = false;
        bool alwaysKeepAlive = false;
        int maxBufferSize = 0x10000;
        int maxHeaderSize = 0x2000;
        int redirectLimit = 12;
        int responseStatusCode = -1;

        std::atomic_bool waitingOnRedirect = false;
        bool internalTimeout = false;

        int temporaryChunkSize = 0;
        int currentRedirectCount = 0;
        std::string chunkSizeAsString = "";

        int errorStatus = TYPE_NO_ERROR;
        std::vector<unsigned char> existingBuffer;
    };
} // namespace smpl

