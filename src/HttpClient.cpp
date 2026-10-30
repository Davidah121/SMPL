#include "HttpClient.h"
#include "StringTools.h"
#include "System.h"

namespace smpl
{
	HttpClient::HttpClient(const URL& url)
	{
		//should reject anything that isn't HTTP or HTTPS
		this->url = url;
		setupNewConfig(url);
		internalConnect();
	}
	
	void HttpClient::setupNewConfig(const URL& url)
	{
		config = {};
		config.amountOfConnectionsAllowed = 1;
		config.location = url.getHost();
		config.port = url.getPort();
		config.type = Network::TYPE_CLIENT;
		config.TCP = true;
	}

	void HttpClient::internalConnect()
	{
		bool valid = url.getProtocol() == "http" || url.getProtocol() == "https";
		if(url.getProtocol() == "https")
			config.secure = true;
		
		if(valid)
		{
			if(network != nullptr)
				delete network;
			network = new Network(config);
		}
		else
			doneReceiving = true;
		
		if(network != nullptr)
		{
			network->setOnConnectFunction( [this](const std::vector<std::pair<std::string, size_t>>& connList) ->void{
				if(shouldResendRequest)
					resendRequest();
			});
			network->setOnDisconnectFunction( [this](const std::vector<std::pair<std::string, size_t>>& connList) ->void{
				doneReceiving = true;
			});
			network->setOnDataAvailableFunction( [this](const std::vector<std::pair<std::string, size_t>>& connList) ->void{
				this->internalOnDataAvailable();
			});
			network->startNetwork();
		}
		else
			doneReceiving = true;
		
	}

	HttpClient::~HttpClient()
	{
		if(network != nullptr)
			delete network;
		
		network = nullptr;
		doneReceiving = true;
	}

	
	bool HttpClient::isValid()
	{
		return network != nullptr;
	}

	void HttpClient::setMaxBufferSize(unsigned int size)
	{
		maxBufferSize = size;
	}
	void HttpClient::setMaxRequestHeaderSize(unsigned int size)
	{
		maxHeaderSize = size;
	}

	void HttpClient::setUserAgent(const std::string& str)
	{
		userAgent = str;
	}

	void HttpClient::allowRedirection(bool v)
	{
		canRedirect = v;
	}
	
	void HttpClient::alwaysSendKeepAlive(bool v)
	{
		alwaysKeepAlive = v;
	}

	void HttpClient::setRedirectLimit(unsigned int v)
	{
		redirectLimit = v;
	}
	const URL& HttpClient::getURL()
	{
		return url;
	}
	
	CookieManager& HttpClient::getCookies()
	{
		return currentCookies;
	}

	bool HttpClient::getTimeoutOccurred()
	{
		return internalTimeout || network->getTimeoutOccurred();
	}

	void HttpClient::reconnect()
	{
		network->reconnect();
	}
	void HttpClient::disconnect()
	{
		if(network != nullptr)
			network->disconnect();
	}
	
	void HttpClient::internalOnDataAvailable()
	{
		//try to grab data till there is none left + make sure its a full request
		response = WebRequest();
		internalTimeout = false;
		int amountRead = network->peek(existingBuffer, maxHeaderSize, 0);
		
		if(amountRead == 0)
		{
			network->setDoneReceiving(0); //False alarm due to how SSL is a little different.
			return; //NOT DONE
		}
		if(amountRead < 0)
		{
			validRequest = false;
			errorStatus = TYPE_RECEIVE_ERROR;
			existingBuffer.clear();
			doneReceiving = true;
			return;
		}

		//try to put into a webrequest (Note that the body is not included here)
		size_t bytesRead = 0;
		validRequest = response.init(existingBuffer.data(), existingBuffer.size(), &bytesRead);

		if(bytesRead > maxHeaderSize)
		{
			network->dumpReceiveBytes(bytesRead, 0); //assume properly dumped
			validRequest = false;
			errorStatus = TYPE_HEADER_TOO_LARGE_ERROR;
			existingBuffer.clear();
			doneReceiving = true;
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

	bool HttpClient::sendRequest(const WebRequest& req)
	{
		if(network != nullptr)
		{
			internalTimeout = false;
			sentRequest = req; //make a copy of the request
			sentRequest.addKeyValue("User-Agent", userAgent); //assuming it doesn't exist already
			if(alwaysKeepAlive)
				sentRequest.addKeyValue("Connection", "keep-alive");
			sentRequest.addKeyValue("Cache-Control", "no-cache");
			sentRequest.addKeyValue("Pragma", "no-cache");
			sentRequest.addKeyValue("Host", url.getHost());

			
			int sentAmount = network->sendMessage(sentRequest, 0);
			if(sentAmount != sentRequest.getBytesInRequest())
				return false;
			else
				return true;
		}
		return false;
	}

	int HttpClient::sendMessage(const std::vector<unsigned char>& message)
	{
		return network->sendMessage(message);
	}

	int HttpClient::sendMessage(const std::string& msg)
	{
		return network->sendMessage(msg);
	}

	int HttpClient::sendFile(const File& filename, size_t length, size_t offset)
	{
		return network->sendFile(filename.getFullFileName().c_str(), length, offset);
	}
	
	void HttpClient::resendRequest()
	{
		if(network != nullptr)
		{
			internalTimeout = false;
			int sentAmount = network->sendMessage(sentRequest, 0);
		}
		shouldResendRequest = false;
	}
	
	void HttpClient::waitOnConnect()
	{
		if(network != nullptr)
		{
			while(!getTimeoutOccurred() && !network->getClientConnected())
			{
				System::sleep(1, 0, false);
			}
		}
	}
	
	bool HttpClient::waitOnDataReceived(bool handleRedirectsAutomatically)
	{
		while(!doneReceiving && !getTimeoutOccurred())
		{
			if(handleRedirectsAutomatically)
				completeRedirect();
			else
			 	return false;

			System::sleep(1, 0, false);
		}
		return true;
	}
	
	const WebRequest& HttpClient::getResponse()
	{
		return response;
	}
	const std::vector<unsigned char>& HttpClient::getResponseBody()
	{
		return responseBody;
	}


	void HttpClient::handleReceivedResponse(const WebRequest& response)
	{
		//Success = 200, 201, 206
		//Redirection = 300, 301, 302, 307, 308
		//Client Error = 400, 401, 403, 404, 405, 408, 410, 411, 414, 415, 416, 426, 429
		//Server Error = 500, 501, 502, 503, 504, 505

		//adjust existing cookies
		currentCookies.addCookies(response.getCookieMap());
		
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
					for(size_t i=0; i<bytesActuallyRead; i++)
						responseBody.push_back(bodyBuffer[i]);
					
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
			network->disconnect();
		
		if(!alwaysKeepAlive)
			network->disconnect();
		
		doneReceiving = true;
	}
	
	bool HttpClient::chunkedPreProcess(const WebRequest& response, unsigned char* bodyBuffer, size_t size)
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
				
				for(size_t i=0; i<actualAllowedSize; i++)
					responseBody.push_back(bodyBuffer[index+i]);

				temporaryChunkSize -= actualAllowedSize;
				index += actualAllowedSize;
			}
		}
		return false;
	}

	void HttpClient::handleRedirect(const WebRequest& response)
	{
		responseBody.clear();
		existingBuffer.clear();
		chunkSizeAsString.clear();
		
		currentRedirectCount++;
		std::string newLocation = response.readKeyValue("Location");
		//change the request and resend.
		if(newLocation.front() == '/')
		{
			//local location redirect
			sentRequest.setHeader(sentRequest.getType(), newLocation);
			sendRequest(sentRequest);
		}
		else
		{
			//absolute location redirect. //can't do this here. Must be on a different thread
			//delegate back to the main thread. Require that the user handle redirects (a simple complete redirect function)
			//also, its saying to send the same request that you just sent but to this location.
			setupNewConfig(newLocation);
			
			//adjust the sent request location.
			url = URL(newLocation);

			waitingOnRedirect = true;
			shouldResendRequest = true;
		}
	}
	
	bool HttpClient::getShouldRedirect()
	{
		return waitingOnRedirect;
	}

	void HttpClient::completeRedirect()
	{
		if(waitingOnRedirect)
			internalConnect();
		waitingOnRedirect = false;
	}
}