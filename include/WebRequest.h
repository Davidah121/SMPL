#pragma once
#include "BuildOptions.h"

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "SimpleHashTable.h"
#include <vector>
#include <string>
#include <ctime>
#include <limits.h>
#include "URL.h"

namespace smpl
{
	class DLL_OPTION WebCookie
	{
	public:
		WebCookie();
		WebCookie(const std::string& name, const std::string& value);
		~WebCookie();

		void setDomain(const std::string& dm);
		void setIncludeSubdomains(bool b);
		void setPath(const std::string& path);
		void setSecure(bool v);
		void setHttpOnly(bool v);
		void setPartitioned(bool v);
		void setExpiresDate(size_t date);
		void setMaxAge(size_t timeInSeconds);

		std::string getName() const;
		std::string getValue() const;
		std::string getDomain() const;
		bool getIncludeSubdomains() const;
		std::string getPath() const;
		bool getSecure() const;
		bool getHttpOnly() const;
		bool getPartitioned() const;
		size_t getExpiresTime() const;
		bool getIsMaxAge() const;

		std::string getCookieAsString(bool isServerSide) const;
		
	private:
		std::string name;
		std::string value;

		//all optional. Not all included as this isn't a web browser
		bool includeSubdomains = false;
		std::string domain;
		std::string path;
		
		bool isMaxAge = false; //false==date
		size_t expiresTime = SIZE_MAX;

		bool secure = false;
		bool httpOnly = false;
		bool partitioned = false;
	};

	class DLL_OPTION CookieManager
	{
	public:
		CookieManager();
		~CookieManager();

		void addCookies(const CookieManager& cm);
		void addCookie(const WebCookie& c);
		SimpleHashMap<std::string, WebCookie>::Iterator getCookie(const std::string& name);
		SimpleHashMap<std::string, WebCookie>& getAllCookies();
		
		const SimpleHashMap<std::string, WebCookie>::ConstIterator getCookie(const std::string& name) const;
		const SimpleHashMap<std::string, WebCookie>& getAllCookies() const;
		auto begin() {return cookieMap.begin();}
		auto end() {return cookieMap.end();}
		auto begin() const {return cookieMap.begin();}
		auto end() const {return cookieMap.end();}

		void clear();
		bool empty() const;

		void loadServerSentCookie(const std::string& line);
		void loadClientSentCookies(const std::string& line);
		
		void loadCookiesJSON(const std::string& file);
		void loadCookiesNetscape(const std::string& file);

		void saveCookiesJSON(const std::string& file);
		void saveCookiesNetscape(const std::string& file);
	private:
		SimpleHashMap<std::string, WebCookie> cookieMap;
	};

	class DLL_OPTION WebRequest
	{
		public:
			//HERE THE ORDER MATTERS. IF THIS CHANGES, CHANGE THE ARRAYS IN THE IMPLEMENTATION
			static const unsigned int TYPE_GET;
			static const unsigned int TYPE_POST;
			static const unsigned int TYPE_HEAD;
			static const unsigned int TYPE_PUT;
			static const unsigned int TYPE_DELETE;
			static const unsigned int TYPE_CONNECT;
			static const unsigned int TYPE_OPTIONS;
			static const unsigned int TYPE_TRACE;
			static const unsigned int TYPE_PATCH;
			static const unsigned int TYPE_SERVER;

			static const std::string MIME_FORM_DATA;

			WebRequest();
			WebRequest(char* buffer, size_t size);
			WebRequest(const std::string& buffer);
			WebRequest(const std::vector<unsigned char>& buffer);

			WebRequest(const WebRequest& other);
			void operator=(const WebRequest& other);
			WebRequest(WebRequest&& other) noexcept;
			void operator=(WebRequest&& other) noexcept;
			
			/**
			 * @brief Forms a quick web request in one line. Useful for quickly sending out HTTP requests from a client.
			 * 
			 * @param type 
			 *		One of the existing type constants except TYPE_SERVER
			 * @param urlPath 
			 *		The path of the URL specifically. No host, port, or arguments.
			 * @param pairs 
			 *		All of the data pairs to include in the header.
			 * @return WebRequest& 
			 */
			static WebRequest createQuickWebRequest(unsigned int type, const std::string& urlPath, const SimpleHashMap<std::string, std::string>& pairs = {});

			/**
			 * @brief Forms a quick web request in one line. Useful for quickly sending out HTTP requests from a server.
			 * 
			 * @param statusCode 
			 *		One of the many HTTP status codes.
			 *			Due to the total number of them, these will not be included as constants but something you should provide yourself.
			 *			A hashmap of existing known codes and their message is provided even if they may never be used.
			 * @param pairs 
			 *		All of the data pairs to include in the header.
			 * @return WebRequest& 
			 */
			static WebRequest createQuickServerResponse(unsigned short statusCode, const SimpleHashMap<std::string, std::string>& pairs = {});

			/**
			 * @brief Reads values and sets up the web request.
			 * 		Returns whether the web request properly ended.
			 * 			This indicates whether it should have more bytes or not.
			 * 			It properly ends when an empty line is found. Everything after 
			 * 				is the body which is not included in the request.
			 * @param buffer 
			 * @param size 
			 * @param bytesRead 
			 * 		A pointer to a size_t that will store how many bytes were actually read
			 * 		to form the web request.
			 * @return size_t 
			 */
			bool init(unsigned char* buffer, size_t size, size_t* bytesRead);

			/**
			 * @brief Sets the header for the request/response.
			 * 		If type is not UNKNOWN, data is assumed to be the url of the request.
			 * 		It is also assumed that HTTP should be included. This is best for request.
			 * 		
			 * 		If the type is UNKNOWN, url is assumed to be empty. This is best for responses.
			 * 
			 * @param type 
			 * @param data 
			 * @param includeHTTP 
			 */
			void setHeader(unsigned int type, const std::string& data);

			/**
			 * @brief Gets the full Header of the request/response
			 * 
			 * @return std::string 
			 */
			std::string getHeader() const;

			/**
			 * @brief Gets the Type of the request/response
			 * 		Note that for responses, it will likely be TYPE_SERVER
			 * 
			 * @return unsigned int 
			 */
			unsigned int getType() const;

			/**
			 * @brief Gets the message status code if it exists.
			 *		This is a value in [100, 599].
			 *		If you get a value of 0, there is no status code or it just couldn't be parsed. Assume it doesn't exist
			 * 
			 * @return unsigned int 
			 */
			unsigned int getStatus() const;

			/**
			 * @brief Gets the url from the header of the request/response
			 * 		Note that for responses, it will likely be empty.
			 * 
			 * @return std::string 
			 */
			std::string getPathOrMessage() const;

			/**
			 * @brief Adds a key value pair to the web request.
			 * 		Will replace the existing key value pair if it already exists.
			 * 
			 * @param key 
			 * @param value 
			 */
			void addKeyValue(const std::string& key, const std::string& value);

			/**
			 * @brief Attempts to read a key value pair. If the key does not
			 * 		exist, it returns an empty string.
			 * 
			 * @param key 
			 * @return std::string 
			 */
			std::string readKeyValue(const std::string& key) const;
			
			/**
			 * @brief Gets the entire request as a string that can be sent
			 * 		or stored directly.
			 * 
			 * @return std::string 
			 */
			std::string getRequestAsString() const;

			/**
			 * @brief Sets all of the cookies for this web request. These may come from an existing web request or stored cookies in a file or something.
			 * 
			 * @param existingCookieMap 
			 */
			void setCookies(const CookieManager& existingCookieMap);

			/**
			 * @brief Gets all cookies for the request.
			 * 
			 * @param key 
			 * @return CookieMap
			 */
			CookieManager& getCookieMap();
			const CookieManager& getCookieMap() const;

			/**
			 * @brief Returns if the request is empty.
			 * 
			 * @return true 
			 * @return false 
			 */
			bool empty() const;

			/**
			 * @brief Get the total number of bytes used in the request.
			 * 		This is only modified by init or the constructor currently.
			 * 		Adding new key values or changing the header does not modify this value currently.
			 * 
			 * @return size_t 
			 */
			size_t getBytesInRequest() const;

			/**
			 * @brief Gets the Mime Type From the extension of a file.
			 *      Necessary to form the correct response if sending a file.
			 * 
			 * @param ext 
			 * @return std::string 
			 */
			static std::string getMimeTypeFromExt(const std::string& ext);

			/**
			 * @brief Get the Date in GMT format.
			 *      Necessary to form the correct response if date is included.
			 * 
			 * @return std::string 
			 */
			static std::string getDateAsGMT();

			/**
			 * @brief Determines if the string is valid as a cookie key or value.
			 * 		They can't contain certain character. They may contain the following characters
			 * 			abdefghijklmnqrstuvxyzABDEFGHIJKLMNQRSTUVXYZ0123456789!#$%&'()*+-./:<>?@[]^_`{|}~=
			 * 
			 * @return true 
			 * @return false 
			 */
			static bool isValidCookieFormat(const std::string& v);

			static std::string getStatusMessage(unsigned short statusCode);
			
		private:
			void reset();
			
			unsigned int type;
			unsigned int statusCode;
			size_t bytesInHeader = 0;
			std::string header;
			std::string pathOrMessage;
			SimpleHashMap<std::string, std::string> data;
			
			CookieManager cookieMap; //Should manage your own cookies
        	static const SimpleHashMap<std::string, std::string> mimeTypes;
			static const SimpleHashMap<unsigned short, std::string> statusAndMessages;
	};
}