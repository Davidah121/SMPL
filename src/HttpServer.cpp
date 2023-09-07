#include "HttpServer.h"
#include "SimpleFile.h"
#include "SimpleDir.h"
#include "System.h"


namespace glib
{
    HttpServer::HttpServer(std::string ip, int port, int connectionsAllowed)
    {
        conn = new Network(Network::TYPE_SERVER, port, ip, connectionsAllowed);
        conn->setOnConnectFunction([this](int id) -> void {
            if(this->getLoggingInfo())
            {
                std::string ipString = this->getNetworkConnection()->getIPFromConnection(id);
                int portNum = this->getNetworkConnection()->getPort();

                StringTools::println("CONNECTION ESTABLISHED WITH %s on port %d\n", ipString.c_str(), portNum);
            }
        });

        conn->setOnDataAvailableFunction([this](int id) -> void {
            //Recv WebRequest
            std::vector<unsigned char> buffer;
            int size = this->getNetworkConnection()->receiveMessage(buffer, id);

            WebRequest req = WebRequest(buffer);
            if(this->getLoggingInfo())
            {
                std::string ipString = this->getNetworkConnection()->getIPFromConnection(id);
                int portNum = this->getNetworkConnection()->getPort();

                StringTools::println("%s on port %d : %s", ipString.c_str(), portNum, req.getHeader().c_str());
            }

            this->handleRecv(req, id);
            this->getNetworkConnection()->disconnect(id);
            // StringTools::println("");
        });

        conn->setOnDisconnectFunction([this](int id) -> void {
            if(this->getLoggingInfo())
            {
                std::string ipString = this->getNetworkConnection()->getIPFromConnection(id);
                int portNum = this->getNetworkConnection()->getPort();

                StringTools::println("DISCONNECTION FROM %s on port %d BY CLIENT\n", ipString.c_str(), portNum);
            }
        });
    }

    HttpServer::~HttpServer()
    {
        if(conn != nullptr)
        {
            conn->endNetwork();
            delete conn;
        }
    }
    
    void HttpServer::setGetFuncMapper(std::function<void(HttpServer*, WebRequest&, int)> func)
    {
        getFuncMapper = func;
    }

    void HttpServer::setPostFuncMapper(std::function<void(HttpServer*, WebRequest&, int)> func)
    {
        postFuncMapper = func;
    }

    void HttpServer::defaultGetFunction(WebRequest& req, int id)
    {
        if(conn == nullptr)
        {
            return;
        }

        sendFile(req, id);
    }

    void HttpServer::handleRecv(WebRequest& req, int id)
    {
        if(req.getType() == WebRequest::TYPE_GET || req.getType() == WebRequest::TYPE_HEAD)
        {
            if(getFuncMapper != nullptr)
                getFuncMapper(this, req, id);
            else
                defaultGetFunction(req, id);
        }
        else if(req.getType() == WebRequest::TYPE_POST)
        {
            if(postFuncMapper != nullptr)
                postFuncMapper(this, req, id);
        }
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
        logInfo = l;
    }
    
    bool HttpServer::getLoggingInfo()
    {
        return logInfo;
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

    void HttpServer::send404Error(int id)
    {
        if(conn == nullptr)
        {
            return;
        }

        if(logInfo)
        {
            StringTools::println("404 ERROR");
        }

        WebRequest resp = WebRequest();
        resp.setHeader(WebRequest::TYPE_UNKNOWN, "HTTP/1.1 404 Not Found", false);
        resp.addKeyValue("Connection", "close");
        resp.addKeyValue("Date", WebRequest::getDateAsGMT());
        
        std::string s = resp.getRequestAsString();

        //send header first. Should be able to send the entire thing at once.
        int bytesSent = conn->sendMessage(s, id);
        if(bytesSent < 0)
        {
            //problem
            StringTools::println("ERROR ON SEND");
            return;
        }
    }

    void HttpServer::send403Error(int id)
    {
        if(conn == nullptr)
        {
            return;
        }

        if(logInfo)
        {
            StringTools::println("403 ERROR");
        }

        WebRequest resp = WebRequest();
        resp.setHeader(WebRequest::TYPE_UNKNOWN, "HTTP/1.1 403 Forbidden", false);
        resp.addKeyValue("Connection", "close");
        resp.addKeyValue("Date", WebRequest::getDateAsGMT());
        
        std::string s = resp.getRequestAsString();

        //send header first. Should be able to send the entire thing at once.
        int bytesSent = conn->sendMessage(s, id);
        if(bytesSent < 0)
        {
            //problem
            StringTools::println("ERROR ON SEND");
            return;
        }
    }

    void HttpServer::sendRangeError(int id, std::string ranges)
    {
        if(conn == nullptr)
        {
            return;
        }

        if(logInfo)
        {
            StringTools::println("416 ERROR");
        }

        WebRequest resp = WebRequest();
        resp.setHeader(WebRequest::TYPE_UNKNOWN, "HTTP/1.1 416 Range Not Statisfiable", false);
        resp.addKeyValue("Content-Range", ranges);
        resp.addKeyValue("Connection", "close");
        resp.addKeyValue("Date", WebRequest::getDateAsGMT());
        
        std::string s = resp.getRequestAsString();

        //send header first. Should be able to send the entire thing at once.
        int bytesSent = conn->sendMessage(s, id);
        if(bytesSent < 0)
        {
            //problem
            StringTools::println("ERROR ON SEND");
            return;
        }
    }

    void HttpServer::sendFile(WebRequest& req, int id)
    {
        if(conn == nullptr)
        {
            return;
        }

        if(req.getType() != WebRequest::TYPE_GET && req.getType() != WebRequest::TYPE_HEAD)
        {
            //problem. send error. Must be a get request
            send404Error(id);
            return;
        }
                
        //parse the request for the resource name
        std::string header = req.getHeader();
        std::vector<std::string> splitStr = StringTools::splitString(header, ' ');
        
        //must be local to the starting directory.
        std::string fStr = "./";
        if(!splitStr[1].empty())
        {
            if(splitStr[1].front() == '/')
                fStr += splitStr[1].substr(1);
        }

        if(splitStr[1] == "/" && redirectToIndex)
        {
            fStr = "./index.html";
        }

        //convert from URL to normal string
        fStr = StringTools::urlDecode(fStr);
        File filename = SimpleDir::getReferenceFullPath(fStr);

        //Is not a local path if fStr contains : in it.
        //Also can't contain ..
        if(fStr.find(':') != SIZE_MAX || fStr.find("..") != SIZE_MAX)
        {
            //probably not a local path.
            //reject
            send403Error(id);
            return;
        }

        //check if file exist
        if(!filename.doesExist() || !filename.isFile())
        {
            //problem. send 404 error.
            send404Error(id);
            return;
        }

        //get content length
        size_t filesize = SimpleDir::getReferenceSize(filename.getFullFileName());

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
                send404Error(id);
                return;
            }

            if(splitRanges.size() == 2 && splitRanges[0].empty())
            {
                //special case. Get the last x bytes
                startRange = filesize - StringTools::toLong(splitRanges[1]);
                endRange = filesize;
            }

            if(splitRanges.size() > 0)
                startRange = StringTools::toLong(splitRanges[0]);
            if(splitRanges.size() > 1 && !splitRanges[1].empty())
                endRange = StringTools::toLong(splitRanges[1]);
        }

        if(endRange <= startRange)
        {
            //problem send error
            sendRangeError(id, ranges);
        }

        //set desired size. May be the same as the filesize
        size_t desiredSize = (endRange - startRange)+1;
        if(desiredSize > filesize)
            desiredSize = filesize;
        
        if(desiredSize >= rangeLimit)
        {
            endRange = startRange + rangeLimit;
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

        resp.addKeyValue("Connection", "close");
        resp.addKeyValue("Date", WebRequest::getDateAsGMT());

        //determine mime type
        resp.addKeyValue("Content-Type", WebRequest::getMimeTypeFromExt(filename.getExtension()));


        if(req.getType() == WebRequest::TYPE_HEAD)
        {
            //end early
            int bytesSent = conn->sendMessage(resp.getRequestAsString(), id);
            if(bytesSent < 0)
            {
                //problem
                StringTools::println("ERROR ON SEND");
                return;
            }
            return;
        }

        //set body
        SimpleFile rawFile = SimpleFile(filename, SimpleFile::READ);
        if(!rawFile.isOpen())
        {
            //problem. send 404 error.
            send404Error(id);
            return;
        }

        std::vector<unsigned char> buffer = std::vector<unsigned char>(desiredSize);
        rawFile.seek(startRange);
        rawFile.readBytes((char*)buffer.data(), desiredSize);
        rawFile.close();
        resp.setBody(buffer.data(), buffer.size());

        //send data.
        int bytesSent = conn->sendMessage(resp.getRequestAsString(), id);
        if(bytesSent < 0)
        {
            //problem
            StringTools::println("ERROR ON SEND");
            return;
        }
    }

}