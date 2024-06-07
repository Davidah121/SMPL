#include "WebClient.h"
#include "StringTools.h"
#include "System.h"

namespace smpl
{
    WebClient::WebClient(std::string location)
    {
        setupNewConfig(location);
        internalConnect();
    }
    
    void WebClient::setupNewConfig(std::string location)
    {
        config = {}; 
        config.amountOfConnectionsAllowed = 1;
        config.location = location;
        config.port = -1;
        config.type = Network::TYPE_CLIENT;
        config.TCP = true;

        size_t indexOfExtraStuff = location.find('/', 8);
        webName = location.substr(0, indexOfExtraStuff); //every thing up to the 3rd slash (not including the third slash)
        size_t locationOfPort = webName.find(':', 6); //must be after http: or https: Assume the longer one

        config.location = webName.substr(0, locationOfPort);
        if(locationOfPort < webName.size())
            config.port = StringTools::toInt( webName.substr(locationOfPort+1) );
    }

    void WebClient::internalConnect()
    {
        bool valid = false;
        if(config.location.substr(0, 4) != "http")
        {
            //can't do it
            return;
        }
        else
        {
            valid = true;
            if(config.location[4] == 's')
            {
                if(config.port < 0)
                    config.port = 443; //try both port 443 and 4443
                
                //can only do if OPENSSL is defined
                //remove https://
                hostStr = config.location.substr(8);
                config.secure = true;
                config.location = hostStr;
            }
            else
            {
                if(config.port < 0)
                    config.port = 80; //try both port 80 and 8080
                
                //remove http://
                hostStr = config.location.substr(7);
                config.location = hostStr;
            }
        }

        if(valid)
            network = new Network(config);
        
        if(network != nullptr)
        {
            network->setOnConnectFunction( [this](size_t id) ->void{
                if(this->onConnectionFunction != nullptr)
                    this->onConnectionFunction(this);
                if(shouldResendRequest)
                    resendRequest();
            });
            network->setOnDisconnectFunction( [this](size_t id) ->void{
                if(this->onDisconnectionFunction != nullptr)
                    this->onDisconnectionFunction(this);
            });
            network->setOnDataAvailableFunction( [this](size_t id) ->void{
                this->internalOnDataAvailable();
            });
        }
        
        if(started == true)
        {
            start();
        }
    }

    WebClient::~WebClient()
    {
        if(network != nullptr)
            delete network;
        
        network = nullptr;
    }

    
    bool WebClient::isValid()
    {
        return network == nullptr;
    }

    void WebClient::setMaxBufferSize(int size)
    {
        maxBufferSize = size;
    }
    void WebClient::setMaxRequestHeaderSize(int size)
    {
        maxHeaderSize = size;
    }

    void WebClient::setUserAgent(std::string str)
    {
        userAgent = str;
    }

    void WebClient::allowRedirection(bool v)
    {
        canRedirect = v;
    }
    
    void WebClient::alwaysSendKeepAlive(bool v)
    {
        alwaysKeepAlive = v;
    }

    void WebClient::setRedirectLimit(int v)
    {
        redirectLimit = v;
    }

    int WebClient::getLastResponseCode()
    {
        return responseStatusCode;
    }
    
    std::string WebClient::getHost()
    {
        return hostStr;
    }

    std::string WebClient::getWebname()
    {
        return webName;
    }

    bool WebClient::getTimeoutOccurred()
    {
        return internalTimeout || network->getTimeoutOccurred();
    }

    void WebClient::start()
    {
        started = true;
        network->startNetwork();
    }
    void WebClient::reconnect()
    {
        network->reconnect();
    }
    void WebClient::disconnect()
    {
        if(network != nullptr)
            network->disconnect();
    }
    
    void WebClient::internalOnDataAvailable()
    {
        //try to grab data till there is none left + make sure its a full request
        internalTimeout = false;
        int amountRead = network->peek(existingBuffer, maxHeaderSize, 0);
        if(amountRead < 0)
        {
            validRequest = false;
            errorStatus = TYPE_RECEIVE_ERROR;
            response = WebRequest();
            existingBuffer.clear();
            return;
        }

        //try to put into a webrequest (Note that the body is not included here)
        response = WebRequest();
        size_t bytesRead = 0;
        validRequest = response.init(existingBuffer.data(), existingBuffer.size(), &bytesRead);

        if(bytesRead > maxHeaderSize)
        {
            network->dumpReceiveBytes(bytesRead, 0); //assume properly dumped
            validRequest = false;
            errorStatus = TYPE_HEADER_TOO_LARGE_ERROR;
            response = WebRequest();
            existingBuffer.clear();
            return;
        }

        if(validRequest)
        {
            network->dumpReceiveBytes(bytesRead, 0); //assume properly dumped
            existingBuffer.clear();
            
            handleReceivedResponse(response);
        }
        //wait otherwise
    }

    bool WebClient::sendRequest(WebRequest& req)
    {
        if(network != nullptr)
        {
            internalTimeout = false;
            sentRequest = WebRequest(req); //make a copy of the request
            sentRequest.addKeyValue("User-Agent", userAgent); //assuming it doesn't exist already
            if(alwaysKeepAlive)
                sentRequest.addKeyValue("Connection", "keep-alive");
            sentRequest.addKeyValue("Cache-Control", "no-cache");
            sentRequest.addKeyValue("Pragma", "no-cache");
            sentRequest.addKeyValue("Host", hostStr);

            
            int sentAmount = network->sendMessage(sentRequest, 0);
            if(sentAmount != req.getBytesInRequest())
                return false;
            else
                return true;
        }
        return false;
    }
    
    void WebClient::resendRequest()
    {
        if(network != nullptr)
        {
            internalTimeout = false;
            int sentAmount = network->sendMessage(sentRequest, 0);
        }
        shouldResendRequest = false;
    }

    void WebClient::setOnConnectFunc(std::function<void(WebClient*)> func)
    {
        onConnectionFunction = func;
    }
    void WebClient::setOnDisconnectFunc(std::function<void(WebClient*)> func)
    {
        onDisconnectionFunction = func;
    }
    void WebClient::setOnBufferChangedFunc(std::function<void(WebClient*, WebRequest& response, unsigned char*, size_t)> func)
    {
        onBufferChanged = func;
    }
    
    void WebClient::handleReceivedResponse(WebRequest& response)
    {
        //Success = 200, 201, 206
        //Redirection = 300, 301, 302, 307, 308
        //Client Error = 400, 401, 403, 404, 405, 408, 410, 411, 414, 415, 416, 426, 429
        //Server Error = 500, 501, 502, 503, 504, 505
        
        std::vector<std::string> splitHeader = StringTools::splitString(response.getHeader(), ' ');
        responseStatusCode = -1;
        if(splitHeader.size() >= 2)
        {
            responseStatusCode = StringTools::toInt(splitHeader[1]);
        }

        //always try to get the body of the response
        unsigned char* bodyBuffer = new unsigned char[maxBufferSize];
        size_t totalRead = 0;
        size_t totalExpected = 0;

        std::string contentLengthAsString = response.readKeyValue("Content-Length");
        if(!contentLengthAsString.empty())
            totalExpected = StringTools::toULongLong(contentLengthAsString);

        
        std::string transferEncoding = response.readKeyValue("Transfer-Encoding");
        bool chunkedEncoding = StringTools::equalsIgnoreCase<char>(transferEncoding, "chunked");

        
        bool end = false;
        size_t timeoutStart = System::getCurrentTimeMillis();

        while(true)
        {
            int bytesActuallyRead = network->receiveMessage(bodyBuffer, maxBufferSize, 0, true);
            if(bytesActuallyRead > 0)
            {
                totalRead += bytesActuallyRead;
                if(chunkedEncoding)
                {
                    if(chunkedPreProcess(response, bodyBuffer, bytesActuallyRead))
                        break;
                }
                else
                {
                    if(onBufferChanged != nullptr)
                        onBufferChanged(this, response, bodyBuffer, bytesActuallyRead);
                    
                    if(totalRead >= totalExpected)
                        break;
                }
            }
            else if(bytesActuallyRead == 0)
            {
                if(!chunkedEncoding)
                {
                    if(totalRead >= totalExpected)
                        break;
                }
                
                //maybe its the end of the message.
                if((System::getCurrentTimeMillis() - timeoutStart) > network->getTimeoutLength())
                {
                    internalTimeout = true;
                    break;
                }
            }
            else
                break;
            
            System::sleep(1, 0, false);
        }
        
        if(totalRead == 0)
        {
            //never called onBufferChanged.
            if(onBufferChanged != nullptr)
                onBufferChanged(this, response, nullptr, 0);
        }

        if(bodyBuffer != nullptr)
            delete[] bodyBuffer;
        bodyBuffer = nullptr;

        if(responseStatusCode >= 300 && responseStatusCode < 400)
        {
            handleRedirect(response);
            return;
        }
        else
        {
            currentRedirectCount = 0;
        }
        if(responseStatusCode > 400)
        {
            network->disconnect();
        }
    }
    
    bool WebClient::chunkedPreProcess(WebRequest& response, unsigned char* bodyBuffer, size_t size)
    {
        //read chunk size
        size_t index = 0;
        while(index < size)
        {
            if(temporaryChunkSize <= 0)
            {
                bool validStr = false;
                while(index < size)
                {
                    unsigned char c = bodyBuffer[index];
                    index++;
                    if(c == '\n')
                    {
                        validStr = true;
                        break;
                    }
                    if(c >= ' ')
                        chunkSizeAsString += c;
                }

                if(!validStr)
                {
                    //early end. Never hit \r\n. Quit and get more data.
                    break;
                }

                temporaryChunkSize = StringTools::fromHexString(chunkSizeAsString);
                chunkSizeAsString = "";
                
                if(temporaryChunkSize == 0)
                {
                    //just read 0, end of chunked processing
                    return true;
                }
            }
            else
            {
                size_t actualAllowedSize = temporaryChunkSize;
                if(temporaryChunkSize > (size-index))
                    actualAllowedSize = (size-index);
                
                if(onBufferChanged != nullptr)
                    onBufferChanged(this, response, &bodyBuffer[index], actualAllowedSize);

                temporaryChunkSize -= actualAllowedSize;
                index += actualAllowedSize;
            }
        }
        return false;
    }

    void WebClient::handleRedirect(WebRequest& response)
    {
        currentRedirectCount++;
        std::string newLocation = response.readKeyValue("Location");
        //change the request and resend.
        if(newLocation.front() == '/')
        {
            //local location redirect
            sentRequest.setHeader(sentRequest.getType(), newLocation, true);
            sendRequest(sentRequest);
        }
        else
        {
            //absolute location redirect. //can't do this here. Must be on a different thread
            //delegate back to the main thread. Require that the user handle redirects (a simple complete redirect function)
            //also, its saying to send the same request that you just sent but to this location.
            setupNewConfig(newLocation);

            //adjust the sent request location.
            std::string newResourceRequested = newLocation.substr(webName.size());
            sentRequest.setHeader(sentRequest.getType(), newResourceRequested, true);

            waitingOnRedirect = true;
            shouldResendRequest = true;
        }
    }
    
    bool WebClient::getShouldRedirect()
    {
        return waitingOnRedirect;
    }

    void WebClient::completeRedirect()
    {
        if(waitingOnRedirect)
            internalConnect();
        waitingOnRedirect = false;
    }
}