#pragma once
#include "BuildOptions.h"

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <unordered_map>
#include <vector>
#include <string>
#include <ctime>

namespace smpl
{
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
			static const unsigned int TYPE_UNKNOWN = 0xFFFFFFFF;

			WebRequest();
			WebRequest(char* buffer, size_t size);
			WebRequest(std::string buffer);
			WebRequest(std::vector<unsigned char> buffer);

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
			void setHeader(unsigned int type, std::string data, bool includeHTTP = true);

			/**
			 * @brief Gets the full Header of the request/response
			 * 
			 * @return std::string 
			 */
			std::string getHeader();

			/**
			 * @brief Gets the Type of the request/response
			 * 		Note that for responses, it will likely be TYPE_UNKNOWN
			 * 
			 * @return unsigned int 
			 */
			unsigned int getType();

			/**
			 * @brief Gets the url from the header of the request/response
			 * 		Note that for responses, it will likely be empty.
			 * 
			 * @return std::string 
			 */
			std::string getUrl();

			/**
			 * @brief Adds a key value pair to the web request.
			 * 		Will replace the existing key value pair if it already exists.
			 * 
			 * @param key 
			 * @param value 
			 */
			void addKeyValue(std::string key, std::string value);

			/**
			 * @brief Attempts to read a key value pair. If the key does not
			 * 		exist, it returns an empty string.
			 * 
			 * @param key 
			 * @return std::string 
			 */
			std::string readKeyValue(std::string key);
			
			/**
			 * @brief Gets the entire request as a string that can be sent
			 * 		or stored directly.
			 * 
			 * @return std::string 
			 */
			std::string getRequestAsString();

			/**
			 * @brief Returns if the request is empty.
			 * 
			 * @return true 
			 * @return false 
			 */
			bool empty();

			/**
			 * @brief Get the total number of bytes used in the request.
			 * 		This is only modified by init or the constructor currently.
			 * 		Adding new key values or changing the header does not modify this value currently.
			 * 
			 * @return size_t 
			 */
			size_t getBytesInRequest();

			/**
			 * @brief Gets the Mime Type From the extension of a file.
			 *      Necessary to form the correct response if sending a file.
			 * 
			 * @param ext 
			 * @return std::string 
			 */
			static std::string getMimeTypeFromExt(std::string ext);

			/**
			 * @brief Get the Date in GMT format.
			 *      Necessary to form the correct response if date is included.
			 * 
			 * @return std::string 
			 */
			static std::string getDateAsGMT();
			
		private:
			void reset();
			
			unsigned int type;
			size_t bytesInHeader;
			std::string header;
			std::string url;
			std::unordered_map<std::string, std::string> data;
			
        	static const std::unordered_map<std::string, std::string> mimeTypes;
	};
}