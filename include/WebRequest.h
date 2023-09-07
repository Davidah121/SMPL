#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <ctime>

namespace glib
{
	class WebRequest
	{
		public:
			static const char TYPE_GET = 0;
			static const char TYPE_POST = 1;
			static const char TYPE_HEAD = 2;
			static const char TYPE_PUT = 3;
			static const char TYPE_DELETE = 4;
			static const char TYPE_CONNECT = 5;
			static const char TYPE_OPTIONS = 6;
			static const char TYPE_TRACE = 7;
			static const char TYPE_PATCH = 8;
			static const char TYPE_UNKNOWN = -1;

			WebRequest();
			WebRequest(char* buffer, size_t size);
			WebRequest(std::string buffer);
			WebRequest(std::vector<unsigned char> buffer);

			void setHeader(char type, std::string data, bool includeHTTP = true);
			std::string getHeader();
			char getType();
			void addKeyValue(std::string key, std::string value);
			std::string readKeyValue(std::string key);

			void setBody(std::string s);
			void setBody(unsigned char* buffer, size_t size);
			std::string getBody();
			
			std::string getRequestAsString();

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
			void init(unsigned char* buffer, size_t size);
			char type;
			std::string header;
			std::unordered_map<std::string, std::string> data;
			std::string body;
			
        	static const std::unordered_map<std::string, std::string> mimeTypes;
	};
}