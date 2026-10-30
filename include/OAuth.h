#pragma once
#include "HttpServer.h"
#include "HttpClient.h"
#include "SimpleJSON.h"

namespace smpl
{
	class OAuth
	{
	public:
		static const std::string DEFAULT_USERAGENT;

		OAuth(const OAuth& other) = default;
		OAuth(OAuth&& other) = default;
		OAuth& operator=(const OAuth& other) = default;
		OAuth& operator=(OAuth&& other) = default;

		static OAuth getOAuthToken(const SimpleJSON& clientSecret, const std::string& scope, const std::string& userAgent=DEFAULT_USERAGENT);
		static OAuth useExistingToken(const SimpleJSON& clientSecret, const SimpleJSON& existingToken, const std::string& userAgent=DEFAULT_USERAGENT);

		bool refreshToken();
		SimpleJSON& getRawToken();
		SimpleJSON& getClientSecret();

		const std::string& getUserAgent();
		std::string getAccessToken();
		std::string getTokenType();
		std::string getScope();

		void saveToken(const File& f);

	private:
		OAuth(const SimpleJSON& clientSecret, const SimpleJSON& existingToken, const std::string& scope, bool existing, const std::string& userAgent);

		void setupNewToken(const std::string& scope);
		
		std::string userAgent;
		SimpleJSON tokenAndRefreshToken;
		SimpleJSON clientSecretJSON;
	};
}