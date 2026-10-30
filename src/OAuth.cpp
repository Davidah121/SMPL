#include "OAuth.h"
#include "System.h"

namespace smpl
{
	const std::string OAuth::DEFAULT_USERAGENT = "SMPL OAuth Application";

	OAuth OAuth::getOAuthToken(const SimpleJSON& clientSecret, const std::string& scope, const std::string& userAgent)
	{
		return OAuth(clientSecret, SimpleJSON(), scope, false, userAgent); //no existing Token
	}
	OAuth OAuth::useExistingToken(const SimpleJSON& clientSecret, const SimpleJSON& existingToken, const std::string& userAgent)
	{
		return OAuth(clientSecret, existingToken, "", true, userAgent); //Existing Token has scope in it already
	}

	SimpleJSON& OAuth::getRawToken()
	{
		return tokenAndRefreshToken;
	}
	SimpleJSON& OAuth::getClientSecret()
	{
		return clientSecretJSON;
	}

	OAuth::OAuth(const SimpleJSON& clientSecret, const SimpleJSON& existingToken, const std::string& scopeURL, bool existing, const std::string& userAgent)
	{
		clientSecretJSON = clientSecret;
		tokenAndRefreshToken = existingToken;
		this->userAgent = userAgent;

		if(!existing)
			setupNewToken(scopeURL);
		else
			refreshToken();
	}

	std::string OAuth::getAccessToken()
	{
		if(!tokenAndRefreshToken.isEmpty())
			return tokenAndRefreshToken["access_token"].getPair().getValue();
		
		return ""; //THROW AN EXCEPTION PROBABLY
	}
	std::string OAuth::getTokenType()
	{
		if(!tokenAndRefreshToken.isEmpty())
			return tokenAndRefreshToken["token_type"].getPair().getValue();
		
		return ""; //THROW AN EXCEPTION PROBABLY
	}
	std::string OAuth::getScope()
	{
		if(!tokenAndRefreshToken.isEmpty())
			return tokenAndRefreshToken["scope"].getPair().getValue();

		return ""; //THROW AN EXCEPTION PROBABLY
	}
	
	void OAuth::saveToken(const File& f)
	{
		SimpleFile outputFile = SimpleFile(f, SimpleFile::WRITE);
		if(outputFile.isOpen())
		{
			outputFile.writeString(tokenAndRefreshToken.getString(true));
			outputFile.close();
		}
	}

	
	bool OAuth::refreshToken()
	{
		bool refreshedSuccessfully = true;
		//need client secret for this
		std::string clientID = clientSecretJSON["installed"]["client_id"].getPair().getValue();
		std::string clientSecret = clientSecretJSON["installed"]["client_secret"].getPair().getValue();
		URL tokenURL = URL(clientSecretJSON["installed"]["token_uri"].getPair().getValue());

		std::string refreshT = tokenAndRefreshToken["refresh_token"].getPair().getValue();
		std::string body = "client_id="+clientID+"&client_secret="+clientSecret+"&refresh_token="+refreshT
					+ "&grant_type=refresh_token";
		
		//upon delete, client auto disconnects
		HttpClient client = HttpClient(tokenURL);
		client.waitOnConnect();
		if(client.getTimeoutOccurred())
		{
			//error. Server Timeout
			StringTools::println("Refreshing Token - OAUTH Server Timeout");
			return false;
		}

		//Non Blocking Calls (for this thread specifically)
		bool successfulSend = client.sendRequest(WebRequest::createQuickWebRequest(WebRequest::TYPE_POST, tokenURL.getPath(), {
			{"Host", tokenURL.getHost()},
			{"User-Agent", userAgent},
			{"Content-Length", StringTools::toString(body.size())},
			{"Content-Type", "application/x-www-form-urlencoded"}
		}));
		int bytesSent = client.sendMessage(body);

		//Blocking (for this thread specifically)
		client.waitOnDataReceived();

		//If everything worked, you'll have the response header and its body here
		if(client.getResponse().getStatus() == 200)
		{
			if(client.getResponseBody().size() > 0)
			{
				// StreamableVector<unsigned char> streamableBuffer(client.getResponseBody());
				StreamableArray<unsigned char> streamableBuffer((unsigned char*)client.getResponseBody().data(), client.getResponseBody().size(), false);
				SimpleJSON newAccessToken = SimpleJSON(&streamableBuffer);

				try
				{
					tokenAndRefreshToken["access_token"] = newAccessToken["access_token"];
				}
				catch(...)
				{
					//something went wrong
					StringTools::println("Failed to refresh token: %s", newAccessToken.getString(true));
					refreshedSuccessfully = false;
				}
			}
			else
			{
				StringTools::println("Failed to refresh token: No response body");
			}
		}
		else
		{
			//failed. THROW EXCEPTION
			StringTools::println("Failed to refresh token - HTTP STATUS: %u", client.getResponse().getStatus());
			refreshedSuccessfully = false;
		}

		return refreshedSuccessfully;
	}

	void OAuth::setupNewToken(const std::string& scopeURL)
	{
		std::string clientID = clientSecretJSON["installed"]["client_id"].getPair().getValue();
		std::string clientSecret = clientSecretJSON["installed"]["client_secret"].getPair().getValue();
		std::string authURL = clientSecretJSON["installed"]["auth_uri"].getPair().getValue();
		URL tokenURL = URL(clientSecretJSON["installed"]["token_uri"].getPair().getValue());
		std::string redirect_uri = "urn:ietf:wg:oauth:2.0:oob"; //if type is manual code auth

		std::string finalURL = authURL + "?client_id=" + clientID
						+ "&response_type=code&scope=" + scopeURL
						+ "&access_type=offline&redirect_uri=";
		
		NetworkConfig config;
		config.amountOfConnectionsAllowed = 4;
		config.location = "127.0.0.1";
		config.port = 0;
		config.secure = false;
		config.TCP = true;
		config.type = Network::TYPE_SERVER;

		std::mutex quickMut;
		std::string responseURL = "";

		std::string indexHTML = "<!DOCTYPE html><html><body><h1>Safe to close now</h1></body></html>";
		HttpServer server = HttpServer(config, 1);
		server.setGetFuncMapper([&indexHTML, &responseURL, &quickMut](HttpServer* servP, const WebRequest& req, const std::vector<unsigned char>& body, std::string location, size_t id) -> char{
			
			// url should contain the response code needed
			// DO NOT LET THE USER GO TO THE LOCAL HOST AS IT WILL MESS UP THIS PROCESS. SERVER WILL CLOSE RIGHT AFTER DOING THIS
			quickMut.lock();
			responseURL = req.getPathOrMessage();
			quickMut.unlock();

			//send html response back. Doesn't matter if they asked for index.html or anything. Always send this. Make it look pretty one day
			WebRequest response = WebRequest::createQuickServerResponse(200, {
				{"Content-Type", WebRequest::getMimeTypeFromExt("html")},
				{"Content-Length", StringTools::toString(indexHTML.size())}
			});
			
			servP->sendResponse(response, location, id, req);
			servP->staggeredSend(id, (unsigned char*)indexHTML.c_str(), 0, indexHTML.size());
			
			return HttpServer::STATUS_DONE;
		});

		server.start();
		int actualPort = server.getNetworkConnection()->getPort();
		redirect_uri = "http://127.0.0.1:" + StringTools::toString(actualPort);
		
		finalURL += redirect_uri;
		system(("start \"\" \"" + finalURL + "\"").c_str());

		while(server.getRunning())
		{
			System::sleep(10, 0, false);

			quickMut.lock();
			bool canQuit = !responseURL.empty();
			quickMut.unlock();

			if(canQuit)
				break;
		}

		//parse URL
		auto output = StringTools::parseURLArguments(responseURL);

		std::string finalAuthCode = output["code"];
		std::string body = "client_id="+clientID+"&client_secret="+clientSecret+"&code="+finalAuthCode
					+ "&redirect_uri=" + redirect_uri + "&grant_type=authorization_code";
		
		//upon delete, client auto disconnects
		HttpClient client = HttpClient(tokenURL);
		client.waitOnConnect();
		if(client.getTimeoutOccurred())
		{
			//error. Server Timeout
			StringTools::println("Init Token - OAUTH Server Timeout");
			return;
		}

		//Non Blocking Calls (for this thread specifically)
		bool successfulSend = client.sendRequest(WebRequest::createQuickWebRequest(WebRequest::TYPE_POST, tokenURL.getPath(), {
			{"Host", tokenURL.getHost()},
			{"User-Agent", userAgent},
			{"Content-Length", StringTools::toString(body.size())},
			{"Content-Type", "application/x-www-form-urlencoded"}
		}));
		int bytesSent = client.sendMessage(body);

		//Blocking (for this thread specifically)
		client.waitOnDataReceived();

		//If everything worked, you'll have the response header and its body here
		if(client.getResponse().getStatus() == 200)
		{
			if(client.getResponseBody().size() > 0)
			{
				// StreamableVector<unsigned char> streamableBuffer(client.getResponseBody());
				StreamableArray<unsigned char> streamableBuffer((unsigned char*)client.getResponseBody().data(), client.getResponseBody().size(), false);
				this->tokenAndRefreshToken = SimpleJSON(&streamableBuffer);
			}
			else
			{
				StringTools::println("Failed to refresh token: No response body");
			}
		}
		else
		{
			//failed. THROW EXCEPTION
			StringTools::println("Failed to init token - HTTP STATUS: %u", client.getResponse().getStatus());
		}
	}
}