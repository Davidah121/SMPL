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
		WebCookie& getCookie(const std::string& name);
		SimpleHashMap<std::string, WebCookie>& getAllCookies();
		
		const WebCookie& getCookie(const std::string& name) const;
		const SimpleHashMap<std::string, WebCookie>& getAllCookies() const;
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
			static const unsigned int TYPE_GET = 0x01;
			static const unsigned int TYPE_POST = 0x02;
			static const unsigned int TYPE_HEAD = 0x04;
			static const unsigned int TYPE_PUT = 0x08;
			static const unsigned int TYPE_DELETE = 0x10;
			static const unsigned int TYPE_CONNECT = 0x20;
			static const unsigned int TYPE_OPTIONS = 0x40;
			static const unsigned int TYPE_TRACE = 0x80;
			static const unsigned int TYPE_PATCH = 0x100;
			static const unsigned int TYPE_SERVER = 0xFFFFFFFF;

			static const std::string MIME_FORM_DATA;

			WebRequest();
			WebRequest(char* buffer, size_t size);
			WebRequest(const std::string& buffer);
			WebRequest(const std::vector<unsigned char>& buffer);

			WebRequest(WebRequest& other);
			void operator=(WebRequest& other);
			WebRequest(WebRequest&& other) noexcept;
			void operator=(WebRequest&& other) noexcept;
			
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
			void setHeader(unsigned int type, const std::string& data, bool includeHTTP = true);

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
			 * @brief Gets the url from the header of the request/response
			 * 		Note that for responses, it will likely be empty.
			 * 
			 * @return std::string 
			 */
			std::string getUrl() const;

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
			 * @brief Gets all cookies for the request.
			 * 
			 * @param key 
			 * @return CookieMap
			 */
			CookieManager& getCookieMap();

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
			
		private:
			void reset();
			
			unsigned int type;
			size_t bytesInHeader = 0;
			std::string header;
			std::string url;
			SimpleHashMap<std::string, std::string> data;
			
			CookieManager cookieMap; //Should manage your own cookies
        	static const SimpleHashMap<std::string, std::string> mimeTypes;
	};
}