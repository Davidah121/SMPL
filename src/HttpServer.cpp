#include "HttpServer.h"
#include "SimpleFile.h"
#include "SimpleDir.h"
#include "System.h"

#ifndef __min
#define __min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef __max
#define __max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef NO_SOCKETS

namespace smpl
{
    std::map<size_t, size_t> timingPerRequest;
    
    HttpServer::HttpServer(NetworkConfig config, int threads, bool useHTTPS, std::string certificateFile, std::string keyFile)
    {
        //create new job queue
        jobQueue = new SimpleJobQueue(threads);

        //create network connection
        config.type = Network::TYPE_SERVER;
        config.TCP = true;
        config.secure = useHTTPS;
        conn = new Network(config, certificateFile, keyFile);
        conn->setTimeoutLength(knownTimeout);

        jobPointers = std::map<size_t, size_t>();
        jobSendPointers = std::map<size_t, size_t>();

        //setup connection functions
        conn->setOnConnectFunction([this](std::string ip, size_t id) -> void {
            this->onConnection(ip, id);
        });

        conn->setOnDataAvailableFunction([this](std::string ip, size_t id) -> void {
            //put the whole function in the job queue. Return immediately
            this->jobMutex.lock();
            this->jobPointers[id] = this->jobQueue->addJob( [this, ip, id]() ->void { this->onDataArrived(ip, id); });
            this->jobMutex.unlock();
        });

        conn->setOnDisconnectFunction([this](std::string ip, size_t id) -> void {
            this->onDisconnection(ip, id);
        });
    }

    HttpServer::~HttpServer()
    {
        this->jobMutex.lock();

        this->jobPointers.clear();
        this->jobSendPointers.clear();
        this->jobQueue->removeAllJobs();

        this->jobMutex.unlock();

        if(conn != nullptr)
        {
            conn->endNetwork();
            delete conn;
        }
    }

    void HttpServer::onConnection(std::string ip, size_t id)
    {
        //does not need to go into the job queue
        if(this->getLoggingInfo())
        {
            logMutex.lock();
            int portNum = this->getNetworkConnection()->getPort();
            StringTools::println("CONNECTION ESTABLISHED WITH %s on port %d as ID = %llu\n", ip.c_str(), portNum, id);
            logMutex.unlock();
        }
    }

    void HttpServer::onDataArrived(std::string ip, size_t id)
    {
        //Recv WebRequest
        this->jobMutex.lock();
        this->jobPointers.erase(id);
        this->jobMutex.unlock();

        //Try to get as much as possible. Should be no more than 8KB bytes. If so, send entity too large.
        //This is consistent across most servers. Apache limits to 8KB by default.
        std::vector<unsigned char> buffer = std::vector<unsigned char>(8192);
        std::vector<unsigned char> body;
        
        int size = this->getNetworkConnection()->peek(buffer, 8192, id);

        if(size <= 0)
            return;

        size_t actualBytesUsedInRequest = 0;
        int bytesReadIntoBody = 0;
        WebRequest req;
        
        if(req.init(buffer.data(), buffer.size(), &actualBytesUsedInRequest))
        {
            this->getNetworkConnection()->receiveMessage(buffer.data(), actualBytesUsedInRequest, id);

            //read body next
            int contentSize = 0;
            std::string contentSizeStr = req.readKeyValue("Content-Length");
            if(!contentSizeStr.empty())
            {
                contentSize = StringTools::toInt(req.readKeyValue("Content-Length"));
                body.resize(contentSize);
            }
            
            //read from connection
            while(true)
            {
                int actualBytesRead = this->getNetworkConnection()->receiveMessage(body.data()+bytesReadIntoBody, contentSize-bytesReadIntoBody, id);

                if(actualBytesRead < 0)
                {
                    StringTools::println("%d - ERROR ATTEMPTING TO READ FROM BODY.", id);
                    this->getNetworkConnection()->disconnect(id);
                    return;
                }

                bytesReadIntoBody += actualBytesRead;
                if(bytesReadIntoBody == contentSize)
                    break;
                
                System::sleep(1);
            }
        }
        else
        {
            if(actualBytesUsedInRequest >= 8192)
            {
                //Do not accept. Too large of a request.
                //Disconnect to remove all from the queue.
                send413Error(id);
                this->getNetworkConnection()->disconnect(id);
            }
            //Couldn't read everything.
            return;
        }

        if(this->getLoggingInfo())
        {
            logMutex.lock();
            int portNum = this->getNetworkConnection()->getPort();
            StringTools::println("%s on port %d as ID = %llu : %s", ip.c_str(), portNum, id, req.getHeader().c_str());
            logMutex.unlock();
        }

        //handling recv should be a separate job altogether
        //priority can be given for individual files, APIs, ip addresses, etc.
        char status = this->handleRecv(req, body, ip, id);

        if(status == STATUS_ERROR || (status == STATUS_DONE && !allowKeepAlive))
        {
            this->getNetworkConnection()->disconnect(id);
        }
    }

    void HttpServer::onDisconnection(std::string ip, size_t id)
    {
        //does not need to go into the job queue
        //May need to adjust job queue to remove a job. It may have already been removed though.
        this->jobMutex.lock();
        this->jobQueue->removeJob( this->jobPointers[id] );
        this->jobPointers.erase(id);
        
        this->jobQueue->removeJob( this->jobSendPointers[id] );
        this->jobSendPointers.erase(id);
        this->jobMutex.unlock();

        if(this->getLoggingInfo())
        {
            logMutex.lock();
            int portNum = this->getNetworkConnection()->getPort();

            StringTools::println("DISCONNECTION FROM %s on port %d as ID = %d BY CLIENT\n", ip.c_str(), portNum, id);
            logMutex.unlock();
        }
    }
    
    void HttpServer::setGetFuncMapper(std::function<char(HttpServer*, WebRequest&, std::vector<unsigned char>&, std::string, size_t)> func)
    {
        getFuncMapper = func;
    }

    void HttpServer::setPostFuncMapper(std::function<char(HttpServer*, WebRequest&, std::vector<unsigned char>&, std::string, size_t)> func)
    {
        postFuncMapper = func;
    }
    
    void HttpServer::setExtraRequestHandlerFuncMapper(std::function<char(HttpServer*, WebRequest&, std::vector<unsigned char>&, std::string, size_t)> func)
    {
        extraReqHandler = func;
    }
    
    void HttpServer::setResponseHandlerFuncMapper(std::function<void(HttpServer*, WebRequest&, std::string, size_t, WebRequest&)> func)
    {
        extraResponseHandler = func;
    }

    char HttpServer::defaultGetFunction(WebRequest& req, std::string ip, size_t id)
    {
        if(conn == nullptr)
        {
            return false;
        }
        return internalSendFile(req, ip, id);
    }

    char HttpServer::defaultGetFunction(WebRequest& req, File f, std::string ip, size_t id)
    {
        if(conn == nullptr)
        {
            return false;
        }
        return internalSendFile(req, f, ip, id);
    }

    char HttpServer::handleRecv(WebRequest& req, std::vector<unsigned char>& body, std::string ip, size_t id)
    {
        if(req.getType() == WebRequest::TYPE_GET || req.getType() == WebRequest::TYPE_HEAD)
        {
            if(getFuncMapper != nullptr)
                return getFuncMapper(this, req, body, ip, id);
            else
                return defaultGetFunction(req, ip, id);
        }
        else if(req.getType() == WebRequest::TYPE_POST)
        {
            if(postFuncMapper != nullptr)
                return postFuncMapper(this, req, body, ip, id);
        }
        else if(req.getType() == WebRequest::TYPE_OPTIONS)
        {
            return handleOptions(req, ip, id);
        }
        else
        {
            if(extraReqHandler != nullptr)
                return extraReqHandler(this, req, body, ip, id);
        }
        return STATUS_ERROR;
    }

    void HttpServer::setOptionsForURL(std::string url, unsigned int allowedRequestFlags, std::string allowedCustomHeaders)
    {
        urlsAndAllowedMethodsAndHeaders[url] = {allowedRequestFlags, allowedCustomHeaders};
    }
    
    char HttpServer::handleOptions(WebRequest& req, std::string ip, size_t id)
    {
        //Must return allowed methods and request headers.
        //Lazy and will just say that by default, GET, HEAD, and OPTIONS are available

        WebRequest response = WebRequest();
        response.setHeader(WebRequest::TYPE_SERVER, "HTTP/1.1 200 OK", false);
        std::string expectedURL = req.getUrl();
        unsigned int allowFlags = WebRequest::TYPE_OPTIONS;
        std::string allowedMethodsStr = "";
        std::string allowedHeaders = "";
        
        auto it = urlsAndAllowedMethodsAndHeaders.find(expectedURL);
        if(it != urlsAndAllowedMethodsAndHeaders.end())
        {
            allowFlags |= it->second.first;
            allowedHeaders = it->second.second;
        }
        else
        {
            //assume just GET and HEAD
            allowFlags |= WebRequest::TYPE_HEAD;
            allowFlags |= WebRequest::TYPE_GET;
        }

        //lazy code
        if(allowFlags & WebRequest::TYPE_GET)
            allowedMethodsStr += "GET, ";
        if(allowFlags & WebRequest::TYPE_HEAD)
            allowedMethodsStr += "HEAD, ";
        if(allowFlags & WebRequest::TYPE_POST)
            allowedMethodsStr += "POST, ";
        
        allowedMethodsStr += "OPTIONS";
        
        fillGetResponseHeaders(response);
        response.addKeyValue("Content-Length", "0");
        response.addKeyValue("Access-Control-Allow-Origin", "*");
        response.addKeyValue("Access-Control-Allow-Methods", allowedMethodsStr);
        if(!allowedHeaders.empty())
            response.addKeyValue("Access-Control-Allow-Headers", allowedHeaders);
        response.addKeyValue("Access-Control-Max-Age", StringTools::toString(maxAgeCache)); //In seconds. Basically 5 minutes for debugging

        //send header first. Should be able to send the entire thing at once.
        int bytesSent = sendResponse(response, ip, id, req);
        if(bytesSent < 0)
        {
            //problem
            if(getLoggingInfo())
            {
                logMutex.lock();
                StringTools::println("ERROR ON SEND OPTIONS - 200 ID = %llu", id);
                logMutex.unlock();
            }
            return STATUS_ERROR;
        }

        return STATUS_DONE;
    }
    
    void HttpServer::start()
    {
        if(conn != nullptr)
            conn->startNetwork();
    }
    
    bool HttpServer::getRunning()
    {
        if(conn != nullptr)
            return conn->getRunning();
        return false;
    }
    
    void HttpServer::setLogInfo(bool l)
    {
        logMutex.lock();
        logInfo = l;
        logMutex.unlock();
    }
    
    bool HttpServer::getLoggingInfo()
    {
        logMutex.lock();
        bool retVal = logInfo;
        logMutex.unlock();
        return retVal;
    }

    Network* HttpServer::getNetworkConnection()
    {
        return conn;
    }
    
    bool HttpServer::isHTTPS()
    {
        if(conn != nullptr)
            return conn->isSecure();
        return false;
    }

    void HttpServer::setRangeRequestLimit(int bytes)
    {
        rangeLimit = bytes;
    }

    int HttpServer::getRangeRequestLimit()
    {
        return rangeLimit;
    }

    void HttpServer::setRedirectToIndex(bool b)
    {
        redirectToIndex = b;
    }

    bool HttpServer::getRedirectToIndex()
    {
        return redirectToIndex;
    }

    void HttpServer::setAssumeHTML(bool b)
    {
        assumeHtml = b;
    }

    bool HttpServer::getAssumeHTML()
    {
        return assumeHtml;
    }

    void HttpServer::setTimeoutLength(long millis)
    {
        knownTimeout = millis;
        conn->setTimeoutLength(millis);
        if(millis <= 0)
            allowKeepAlive = false;
    }

    void HttpServer::setAllowPersistentConnection(bool b)
    {
        allowKeepAlive = b;
        if(conn->getTimeoutLength() <= 0)
            allowKeepAlive = false;
    }

    bool HttpServer::getAllowPersistentConnection()
    {
        return allowKeepAlive;
    }
    
    void HttpServer::fillGetResponseHeaders(WebRequest& req)
    {
        if(getAllowPersistentConnection())
        {
            req.addKeyValue("Connection", "keep-alive");
            req.addKeyValue("Keep-Alive", "timeout=" + StringTools::toString(knownTimeout / 1000));
        }
        else
            req.addKeyValue("Connection", "close");
        
        req.addKeyValue("Date", WebRequest::getDateAsGMT());
        req.addKeyValue("Server", "SimpleHTTPServer/1.0 C++");
    }

    void HttpServer::send404Error(size_t id)
    {
        if(conn == nullptr)
        {
            return;
        }

        if(getLoggingInfo())
        {
            logMutex.lock();
            StringTools::println("404 ERROR ID = %llu", id);
            logMutex.unlock();
        }

        WebRequest resp = WebRequest();
        resp.setHeader(WebRequest::TYPE_SERVER, "HTTP/1.1 404 Not Found", false);
        resp.addKeyValue("Content-Length", "0");
        fillGetResponseHeaders(resp);

        //send header first. Should be able to send the entire thing at once.
        int bytesSent = sendError(resp, id);
        if(bytesSent < 0)
        {
            //problem
            if(getLoggingInfo())
            {
                logMutex.lock();
                StringTools::println("ERROR ON SEND - 404 ID = %llu", id);
                logMutex.unlock();
            }
            return;
        }
    }

    void HttpServer::send403Error(size_t id)
    {
        if(conn == nullptr)
        {
            return;
        }

        if(getLoggingInfo())
        {
            logMutex.lock();
            StringTools::println("403 ERROR ID = %llu", id);
            logMutex.unlock();
        }

        WebRequest resp = WebRequest();
        resp.setHeader(WebRequest::TYPE_SERVER, "HTTP/1.1 403 Forbidden", false);
        resp.addKeyValue("Content-Length", "0");
        fillGetResponseHeaders(resp);
        
        //send header first. Should be able to send the entire thing at once.
        int bytesSent = sendError(resp, id);
        if(bytesSent < 0)
        {
            //problem
            if(getLoggingInfo())
            {
                logMutex.lock();
                StringTools::println("ERROR ON SEND - 403 ID = %llu", id);
                logMutex.unlock();
            }
            return;
        }
    }

    void HttpServer::sendRangeError(size_t id, std::string ranges)
    {
        if(conn == nullptr)
        {
            return;
        }

        if(getLoggingInfo())
        {
            logMutex.lock();
            StringTools::println("416 ERROR ID = %llu", id);
            logMutex.unlock();
        }

        WebRequest resp = WebRequest();
        resp.setHeader(WebRequest::TYPE_SERVER, "HTTP/1.1 416 Range Not Statisfiable", false);
        resp.addKeyValue("Content-Length", "0");
        resp.addKeyValue("Content-Range", ranges);
        fillGetResponseHeaders(resp);

        //send header first. Should be able to send the entire thing at once.
        int bytesSent = sendError(resp, id);
        if(bytesSent < 0)
        {
            //problem
            if(getLoggingInfo())
            {
                logMutex.lock();
                StringTools::println("ERROR ON SEND - 416 ID = %llu", id);
                logMutex.unlock();
            }
            return;
        }
    }

    
    void HttpServer::send400Error(size_t id)
    {
        if(conn == nullptr)
        {
            return;
        }

        if(getLoggingInfo())
        {
            logMutex.lock();
            StringTools::println("400 ERROR ID = %llu", id);
            logMutex.unlock();
        }

        WebRequest resp = WebRequest();
        resp.setHeader(WebRequest::TYPE_SERVER, "HTTP/1.1 400 Bad Request", false);
        resp.addKeyValue("Content-Length", "0");
        fillGetResponseHeaders(resp);

        //send header first. Should be able to send the entire thing at once.
        int bytesSent = sendError(resp, id);
        if(bytesSent < 0)
        {
            //problem
            if(getLoggingInfo())
            {
                logMutex.lock();
                StringTools::println("ERROR ON SEND - 400");
                logMutex.unlock();
            }
            return;
        }
    }
    
    void HttpServer::send413Error(size_t id)
    {
        if(conn == nullptr)
        {
            return;
        }

        if(getLoggingInfo())
        {
            logMutex.lock();
            StringTools::println("413 ERROR ID = %llu", id);
            logMutex.unlock();
        }

        WebRequest resp = WebRequest();
        resp.setHeader(WebRequest::TYPE_SERVER, "HTTP/1.1 413 Entity Too Large", false);
        resp.addKeyValue("Content-Length", "0");
        fillGetResponseHeaders(resp);

        //send header first. Should be able to send the entire thing at once.
        int bytesSent = sendError(resp, id);
        if(bytesSent < 0)
        {
            //problem
            if(getLoggingInfo())
            {
                logMutex.lock();
                StringTools::println("ERROR ON SEND - 413");
                logMutex.unlock();
            }
            return;
        }
    }

    char HttpServer::internalSendFile(WebRequest& req, std::string ip, size_t id)
    {
        if(conn == nullptr)
        {
            return STATUS_ERROR;
        }

        if(req.getType() != WebRequest::TYPE_GET && req.getType() != WebRequest::TYPE_HEAD)
        {
            //problem. send error. Must be a get request
            send404Error(id);
            return STATUS_ERROR;
        }
                
        //parse the request for the resource name
        std::string expectedURL = req.getUrl();
        
        //must be local to the starting directory.
        std::string fStr = "./";
        if(!expectedURL.empty())
        {
            if(expectedURL.front() == '/')
                fStr += expectedURL.substr(1);
        }

        //convert from URL to normal string
        fStr = StringTools::urlDecode(fStr);
        std::vector<std::string> urlStringSplit = StringTools::splitString(fStr, '?');

        
        if(urlStringSplit[0] == "./" && redirectToIndex)
            urlStringSplit[0] = "./index.html";
        
        File filename = urlStringSplit[0];
        File filenameAlt = urlStringSplit[0];
        if(filename.getExtension().empty() && assumeHtml)
        {
            filename = urlStringSplit[0] + ".html";
        }

        if(filename.getExtension() == ".html")
        {
            filenameAlt = filename.getPath() + filename.getFileName() + ".htm";
        }
        else if(filename.getExtension() == ".htm")
        {
            filenameAlt = filename.getPath() + filename.getFileName() + ".html";
        }

        filename = SimpleDir::getReferenceFullPath(filename.getFullFileName());
        filenameAlt = SimpleDir::getReferenceFullPath(filenameAlt.getFullFileName());
        
        //Is not a local path if fStr contains : in it.
        //Also can't contain ../
        if(fStr.find(':') != SIZE_MAX || fStr.find("../") != SIZE_MAX)
        {
            //probably not a local path.
            //reject
            send403Error(id);
            return STATUS_ERROR;
        }

        //check for alt as well
        bool valid = false;
        File validFName = filename;
        //check if file exist
        if(filename.doesExist() && filename.isFile())
        {
            valid = true;
        }
        else if(filenameAlt.doesExist() && filename.isFile())
        {
            valid = true;
            validFName = filenameAlt;
        }

        if(!valid)
        {
            //problem. send 404 error.
            send404Error(id);
            return STATUS_ERROR;
        }

        //use other send function
        return internalSendFile(req, validFName, ip, id);
    }
    
    char HttpServer::internalSendFile(WebRequest& req, File f, std::string ip, size_t id)
    {
        if(conn == nullptr)
        {
            return STATUS_ERROR;
        }

        if(req.getType() != WebRequest::TYPE_GET && req.getType() != WebRequest::TYPE_HEAD)
        {
            //problem. send error. Must be a get request
            send404Error(id);
            return STATUS_ERROR;
        }
                
        //use f regardless of what the request was.
        bool valid = f.doesExist() && f.isFile();

        if(!valid)
        {
            //problem. send 404 error.
            send404Error(id);
            return STATUS_ERROR;
        }

        //get content length
        size_t filesize = SimpleDir::getReferenceSize(f.getFullFileName());

        //check for range request
        std::string ranges = req.readKeyValue("Range");
        bool usingRanges = false;
        size_t startRange = 0;
        size_t endRange = SIZE_MAX-1;
        if(!ranges.empty())
        {
            usingRanges = true;
            //asking for range.
            //should be in format bytes=xxxx-xxxx
            //skip first 6
            std::vector<std::string> splitRanges = StringTools::splitString(ranges.substr(6), '-', false);
            if(splitRanges.size() > 2 || splitRanges.size() == 0)
            {
                //problem. send error.
                sendRangeError(id, ranges);
                return STATUS_ERROR;
            }

            if(splitRanges.size() == 2 && splitRanges[0].empty())
            {
                //special case. Get the last x bytes
                startRange = (filesize-1) - StringTools::toLong(splitRanges[1]);
                endRange = filesize-1;
            }

            if(splitRanges.size() > 0)
                startRange = StringTools::toLong(splitRanges[0]);
            if(splitRanges.size() > 1 && !splitRanges[1].empty())
                endRange = StringTools::toLong(splitRanges[1]);
            else
                endRange = filesize-1;
        }

        if(endRange <= startRange || startRange >= filesize)
        {
            //problem send error
            sendRangeError(id, ranges);
            return STATUS_ERROR;
        }

        //check user agent. Browser should have Mozilla in it.
        //If browser, assume that they accept ranges that are less than what was asked
        //Otherwise, send the entire thing like
        bool canSendLess = false;
        std::string userAgent = req.readKeyValue("User-Agent");
        if(userAgent.find("Mozilla") != SIZE_MAX)
        {
            //can apply range limit
            canSendLess = true;
        }

        //set desired size. May be the same as the filesize
        if(endRange >= filesize)
            endRange = filesize-1;
        
        size_t desiredSize = (endRange - startRange)+1;
        if(desiredSize > filesize)
        {
            desiredSize = filesize;
            endRange = (desiredSize-1) + startRange;
        }

        if(desiredSize >= rangeLimit && usingRanges && canSendLess)
        {
            endRange = (startRange + rangeLimit)-1;
            desiredSize = rangeLimit;
        }

        //send http ok and correct type
        WebRequest resp = WebRequest();
        if(!usingRanges)
        {
            resp.setHeader(WebRequest::TYPE_SERVER, "HTTP/1.1 200 OK", false);
            resp.addKeyValue("Accept-Ranges", "bytes");
            resp.addKeyValue("Content-Length", StringTools::toString(filesize));
        }
        else
        {
            resp.setHeader(WebRequest::TYPE_SERVER, "HTTP/1.1 206 Partial Content", false);
            resp.addKeyValue("Accept-Ranges", "bytes");
            resp.addKeyValue("Content-Range", "bytes " + StringTools::toString(startRange) + "-" + StringTools::toString(endRange)+"/"+StringTools::toString(filesize));
            resp.addKeyValue("Content-Length", StringTools::toString(desiredSize));
        }

        fillGetResponseHeaders(resp);
        
        //determine mime type
        resp.addKeyValue("Content-Type", WebRequest::getMimeTypeFromExt(f.getExtension()));

        if(req.getType() == WebRequest::TYPE_HEAD)
        {
            //end early
            int bytesSent = sendResponse(resp, ip, id, req);
            if(bytesSent < 0)
            {
                //problem
                if(getLoggingInfo())
                {
                    logMutex.lock();
                    StringTools::println("ERROR ON SEND HEAD - 200 ID = %llu", id);
                    logMutex.unlock();
                }
                return STATUS_ERROR;
            }
            return STATUS_DONE;
        }
        
        int bytesSent = sendResponse(resp, ip, id, req);
        return staggeredSend(id, f, startRange, endRange);
    }

    char HttpServer::staggeredSend(size_t id, File f, size_t startPoint, size_t endPoint)
    {
        this->jobMutex.lock();
        this->jobSendPointers.erase(id);
        this->jobMutex.unlock();

        int bytesSent = conn->sendFile((char*)f.getFullFileName().c_str(), 1+endPoint-startPoint, startPoint, id);
        if(bytesSent < 0)
        {
            //problem
            if(getLoggingInfo())
            {
                logMutex.lock();
                StringTools::println("ERROR ON SEND GET - 200 ID = %llu", id);
                logMutex.unlock();
            }

            return STATUS_ERROR;
        }

        if(startPoint + bytesSent < endPoint)
        {
            //add to jobs
            this->jobMutex.lock();
            this->jobSendPointers[id] = this->jobQueue->addJob( [this, id, f, startPoint, endPoint, bytesSent]() ->void { 
                char status = this->staggeredSend(id, f, startPoint + bytesSent, endPoint);
                if((status == STATUS_DONE && !allowKeepAlive) || status == STATUS_ERROR)
                {
                    //should close socket
                    conn->disconnect(id);
                }
            });
            this->jobMutex.unlock();
        }
        else
            return STATUS_DONE;
        
        return STATUS_CONTINUE;
    }

    //WRAPPERS AROUND NETWORK FOR EASIER OPERATION

    int HttpServer::sendResponse(WebRequest& response, std::string ip, size_t id, WebRequest& req)
    {
        if(extraResponseHandler != nullptr)
            extraResponseHandler(this, req, ip, id, response);
        return conn->sendMessage(response, id);
    }
    int HttpServer::sendError(WebRequest& response, size_t id)
    {
        return conn->sendMessage(response, id);
    }
    int HttpServer::sendMessage(std::vector<unsigned char>& message, size_t id)
    {
        return conn->sendMessage(message, id);
    }
    int HttpServer::sendMessage(const std::string& msg, size_t id)
    {
        return conn->sendMessage(msg, id);
    }
    int HttpServer::sendFile(char* filename, size_t length, size_t offset, size_t id)
    {
        return conn->sendFile(filename, length, offset, id);
    }
    int HttpServer::receiveMessage(std::vector<unsigned char>& message, size_t id)
    {
        return conn->receiveMessage(message, id);
    }
    int HttpServer::receiveMessage(const std::string& msg, size_t id)
    {
        return conn->receiveMessage(msg, id);
    }
    int HttpServer::peek(std::vector<unsigned char>& buffer, int expectedSize, size_t id)
    {
        return conn->peek(buffer, expectedSize, id);
    }
    int HttpServer::dumpReceiveBytes(int bytesToDump, size_t id)
    {
        return conn->dumpReceiveBytes(bytesToDump, id);
    }
    size_t HttpServer::getReadSizeAvailable(size_t id)
    {
        return conn->getReadSizeAvailable(id);
    }
}

#endif