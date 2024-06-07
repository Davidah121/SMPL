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
        conn = new Network(config);
        conn->setTimeoutLength(knownTimeout);

        jobPointers = std::map<size_t, SmartMemory<SLinkNode<std::function<void()>>>>();
        jobSendPointers = std::map<size_t, SmartMemory<SLinkNode<std::function<void()>>>>();

        //setup connection functions
        conn->setOnConnectFunction([this](size_t id) -> void {
            this->onConnection(id);
        });

        conn->setOnDataAvailableFunction([this](size_t id) -> void {
            //put the whole function in the job queue. Return immediately
            this->jobMutex.lock();
            this->jobPointers[id] = this->jobQueue->addJob( [this, id]() ->void { this->onDataArrived(id); } );
            this->jobMutex.unlock();
        });

        conn->setOnDisconnectFunction([this](size_t id) -> void {
            this->onDisconnection(id);
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

    void HttpServer::onConnection(size_t id)
    {
        //does not need to go into the job queue
        if(this->getLoggingInfo())
        {
            logMutex.lock();
            std::string ipString = this->getNetworkConnection()->getIPFromConnection(id);
            int portNum = this->getNetworkConnection()->getPort();

            StringTools::println("CONNECTION ESTABLISHED WITH %s on port %d as ID = %llu\n", ipString.c_str(), portNum, id);
            logMutex.unlock();
        }
    }

    void HttpServer::onDataArrived(size_t id)
    {
        //Recv WebRequest
        this->jobMutex.lock();
        this->jobPointers.erase(id);
        this->jobMutex.unlock();

        //Try to get as much as possible. Should be no more than 8KB bytes. If so, send entity too large.
        //This is consistent across most servers. Apache limits to 8KB by default.
        std::vector<unsigned char> buffer = std::vector<unsigned char>(8192);
        std::vector<unsigned char> body;
        int size = this->getNetworkConnection()->peek(buffer, buffer.size(), id);

        if(size == 0)
            return;

        size_t actualBytesUsedInRequest = 0;
        int bytesReadIntoBody = 0;
        WebRequest req;
        
        if(req.init(buffer.data(), buffer.size(), &actualBytesUsedInRequest))
        {
            this->getNetworkConnection()->receiveMessage(buffer.data(), actualBytesUsedInRequest, id);

            //read body next
            int contentSize = 0;
            
            contentSize = StringTools::toInt(req.readKeyValue("Content-Length"));
            body.resize(contentSize);
            
            //read from connection
            bytesReadIntoBody = this->getNetworkConnection()->receiveMessage(body.data(), contentSize, id);
            
            if(bytesReadIntoBody != contentSize && this->getLoggingInfo())
                StringTools::println("%d - SIZE READ DOES NOT MATCH REQUESTED SIZE: %d vs %d", id, contentSize, bytesReadIntoBody);
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
            std::string ipString = this->getNetworkConnection()->getIPFromConnection(id);
            int portNum = this->getNetworkConnection()->getPort();

            StringTools::println("%s on port %d as ID = %llu : %s", ipString.c_str(), portNum, id, req.getHeader().c_str());
            logMutex.unlock();
        }
        
        // ///no need to read, send 404
        // send404Error(id);
        bool status = this->handleRecv(req, body, id);

        if(!allowKeepAlive || !status)
        {
            this->getNetworkConnection()->disconnect(id);
        }
    }

    void HttpServer::onDisconnection(size_t id)
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
            std::string ipString = this->getNetworkConnection()->getIPFromConnection(id);
            int portNum = this->getNetworkConnection()->getPort();

            StringTools::println("DISCONNECTION FROM %s on port %d as ID = %d BY CLIENT\n", ipString.c_str(), portNum, id);
            logMutex.unlock();
        }
    }
    
    void HttpServer::setGetFuncMapper(std::function<bool(HttpServer*, WebRequest&, std::vector<unsigned char>&, size_t)> func)
    {
        getFuncMapper = func;
    }

    void HttpServer::setPostFuncMapper(std::function<bool(HttpServer*, WebRequest&, std::vector<unsigned char>&, size_t)> func)
    {
        postFuncMapper = func;
    }

    bool HttpServer::defaultGetFunction(WebRequest& req, size_t id)
    {
        if(conn == nullptr)
        {
            return false;
        }

        return sendFile(req, id);
    }

    bool HttpServer::defaultGetFunction(WebRequest& req, File f, size_t id)
    {
        if(conn == nullptr)
        {
            return false;
        }

        return sendFile(req, f, id);
    }

    bool HttpServer::handleRecv(WebRequest& req, std::vector<unsigned char>& body, size_t id)
    {
        if(req.getType() == WebRequest::TYPE_GET || req.getType() == WebRequest::TYPE_HEAD)
        {
            if(getFuncMapper != nullptr)
                return getFuncMapper(this, req, body, id);
            else
                return defaultGetFunction(req, id);
        }
        else if(req.getType() == WebRequest::TYPE_POST)
        {
            if(postFuncMapper != nullptr)
                return postFuncMapper(this, req, body, id);
        }
        else if(req.getType() == WebRequest::TYPE_OPTIONS)
        {
            return handleOptions(req, id);
        }
        return false;
    }

    void HttpServer::setOptionsForURL(std::string url, unsigned int allowedRequestFlags, std::string allowedCustomHeaders)
    {
        urlsAndAllowedMethodsAndHeaders[url] = {allowedRequestFlags, allowedCustomHeaders};
    }
    
    bool HttpServer::handleOptions(WebRequest& req, size_t id)
    {
        //Must return allowed methods and request headers.
        //Lazy and will just say that by default, GET, HEAD, and OPTIONS are available

        WebRequest response = WebRequest();
        response.setHeader(WebRequest::TYPE_UNKNOWN, "HTTP/1.1 200 OK", false);
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
        int bytesSent = conn->sendMessage(response, id);
        if(bytesSent < 0)
        {
            //problem
            if(getLoggingInfo())
            {
                logMutex.lock();
                StringTools::println("ERROR ON SEND OPTIONS - 200 ID = %llu", id);
                logMutex.unlock();
            }
            return false;
        }

        return true;
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
        req.addKeyValue("Server", "SimpleHTTPServer/0.8 C++");
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
        resp.setHeader(WebRequest::TYPE_UNKNOWN, "HTTP/1.1 404 Not Found", false);
        resp.addKeyValue("Content-Length", "5");
        fillGetResponseHeaders(resp);

        //send header first. Should be able to send the entire thing at once.
        int bytesSent = conn->sendMessage(resp, id);
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
        resp.setHeader(WebRequest::TYPE_UNKNOWN, "HTTP/1.1 403 Forbidden", false);
        resp.addKeyValue("Content-Length", "5");
        fillGetResponseHeaders(resp);
        
        //send header first. Should be able to send the entire thing at once.
        int bytesSent = conn->sendMessage(resp, id);
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
        resp.setHeader(WebRequest::TYPE_UNKNOWN, "HTTP/1.1 416 Range Not Statisfiable", false);
        resp.addKeyValue("Content-Length", "5");
        resp.addKeyValue("Content-Range", ranges);
        fillGetResponseHeaders(resp);

        //send header first. Should be able to send the entire thing at once.
        int bytesSent = conn->sendMessage(resp, id);
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
        resp.setHeader(WebRequest::TYPE_UNKNOWN, "HTTP/1.1 400 Bad Request", false);
        resp.addKeyValue("Content-Length", "5");
        fillGetResponseHeaders(resp);

        //send header first. Should be able to send the entire thing at once.
        int bytesSent = conn->sendMessage(resp, id);
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
        resp.setHeader(WebRequest::TYPE_UNKNOWN, "HTTP/1.1 413 Entity Too Large", false);
        resp.addKeyValue("Content-Length", "5");
        fillGetResponseHeaders(resp);

        //send header first. Should be able to send the entire thing at once.
        int bytesSent = conn->sendMessage(resp, id);
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

    bool HttpServer::sendFile(WebRequest& req, size_t id)
    {
        if(conn == nullptr)
        {
            return false;
        }

        if(req.getType() != WebRequest::TYPE_GET && req.getType() != WebRequest::TYPE_HEAD)
        {
            //problem. send error. Must be a get request
            send404Error(id);
            return false;
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
            return false;
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
            return false;
        }

        //use other send function
        return sendFile(req, validFName, id);
    }
    
    bool HttpServer::sendFile(WebRequest& req, File f, size_t id)
    {
        if(conn == nullptr)
        {
            return false;
        }

        if(req.getType() != WebRequest::TYPE_GET && req.getType() != WebRequest::TYPE_HEAD)
        {
            //problem. send error. Must be a get request
            send404Error(id);
            return false;
        }
                
        //use f regardless of what the request was.
        bool valid = f.doesExist() && f.isFile();

        if(!valid)
        {
            //problem. send 404 error.
            send404Error(id);
            return false;
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
                return false;
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
            return false;
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
            resp.setHeader(WebRequest::TYPE_UNKNOWN, "HTTP/1.1 200 OK", false);
            resp.addKeyValue("Accept-Ranges", "bytes");
            resp.addKeyValue("Content-Length", StringTools::toString(filesize));
        }
        else
        {
            resp.setHeader(WebRequest::TYPE_UNKNOWN, "HTTP/1.1 206 Partial Content", false);
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
            int bytesSent = conn->sendMessage(resp, id);
            if(bytesSent < 0)
            {
                //problem
                if(getLoggingInfo())
                {
                    logMutex.lock();
                    StringTools::println("ERROR ON SEND HEAD - 200 ID = %llu", id);
                    logMutex.unlock();
                }
                return false;
            }
            return true;
        }
        
        int bytesSent = conn->sendMessage(resp, id);

        staggeredSend(id, f, startRange, endRange);
        return true;
    }

    void HttpServer::staggeredSend(size_t id, File f, size_t startPoint, size_t endPoint)
    {
        this->jobMutex.lock();
        this->jobSendPointers[id] = nullptr;
        this->jobMutex.unlock();

        SimpleFile rawFile = SimpleFile(f, SimpleFile::READ);
        if(!rawFile.isOpen())
        {
            return;
        }

        std::vector<unsigned char> buffer = std::vector<unsigned char>(rangeLimit);
        rawFile.seek(startPoint);

        size_t bytesWanted = (endPoint - startPoint)+1;
        if(bytesWanted >= rangeLimit)
            bytesWanted = rangeLimit;
        
        size_t bytesRead = rawFile.readBytes((char*)buffer.data(), bytesWanted);
        rawFile.close();

        int bytesSent = conn->sendMessage((char*)buffer.data(), bytesRead, id);

        if(bytesSent < 0)
        {
            //problem
            if(getLoggingInfo())
            {
                logMutex.lock();
                StringTools::println("ERROR ON SEND GET - 200 ID = %llu", id);
                logMutex.unlock();
            }

            //should close socket
            conn->disconnect(id);
            return;
        }

        if(startPoint + bytesSent < endPoint)
        {
            //add to jobs
            this->jobMutex.lock();
            this->jobSendPointers[id] = this->jobQueue->addJob( [this, id, f, startPoint, endPoint, bytesSent]() ->void { this->staggeredSend(id, f, startPoint + bytesSent, endPoint); } );
            this->jobMutex.unlock();
        }
        else
        {
            // totalTimeForRequest += System::getCurrentTimeMillis() - timingPerRequest[id];
        }
    }
}

#endif