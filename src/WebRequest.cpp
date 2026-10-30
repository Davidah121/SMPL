#include "WebRequest.h"
#include "StringTools.h"
#include <iomanip>

namespace smpl
{
	
	const unsigned int WebRequest::TYPE_GET = 0;
	const unsigned int WebRequest::TYPE_POST = 1;
	const unsigned int WebRequest::TYPE_HEAD = 2;
	const unsigned int WebRequest::TYPE_PUT = 3;
	const unsigned int WebRequest::TYPE_DELETE = 4;
	const unsigned int WebRequest::TYPE_CONNECT = 5;
	const unsigned int WebRequest::TYPE_OPTIONS = 6;
	const unsigned int WebRequest::TYPE_TRACE = 7;
	const unsigned int WebRequest::TYPE_PATCH = 8;
	const unsigned int WebRequest::TYPE_SERVER = 0xFFFFFFFF;

	WebRequest::WebRequest()
	{
	}

	WebRequest::WebRequest(char* buffer, size_t size)
	{
		init((unsigned char*)buffer, size, nullptr);
	}

	WebRequest::WebRequest(const std::string& buffer)
	{
		init((unsigned char*)buffer.data(), buffer.size(), nullptr);
	}

	WebRequest::WebRequest(const std::vector<unsigned char>& buffer)
	{
		//fill out data using buffer
		init((unsigned char*)buffer.data(), buffer.size(), nullptr);
	}
	
	WebRequest::WebRequest(const WebRequest& other)
	{
		reset();
		type = other.type;
		bytesInHeader = other.bytesInHeader;
		header = other.header;
		data = other.data;
		pathOrMessage = other.pathOrMessage;
		cookieMap = other.cookieMap;
	}

	void WebRequest::operator=(const WebRequest& other)
	{
		reset();
		type = other.type;
		bytesInHeader = other.bytesInHeader;
		header = other.header;
		data = other.data;
		pathOrMessage = other.pathOrMessage;
		cookieMap = other.cookieMap;
	}
	
	WebRequest::WebRequest(WebRequest&& other) noexcept
	{
		reset();
		type = std::move(other.type);
		bytesInHeader = std::move(other.bytesInHeader);
		header = std::move(other.header);
		data = std::move(other.data);
		pathOrMessage = std::move(other.pathOrMessage);
		cookieMap = std::move(other.cookieMap);
		other.reset();
	}
	void WebRequest::operator=(WebRequest&& other) noexcept
	{
		reset();
		type = std::move(other.type);
		bytesInHeader = std::move(other.bytesInHeader);
		header = std::move(other.header);
		data = std::move(other.data);
		pathOrMessage = std::move(other.pathOrMessage);
		cookieMap = std::move(other.cookieMap);
		other.reset();
	}

	void WebRequest::reset()
	{
		type = WebRequest::TYPE_SERVER;
		bytesInHeader = 0;
		header = "";
		cookieMap.clear();
		data.clear();
	}
	
	WebRequest WebRequest::createQuickWebRequest(unsigned int type, const std::string& urlPath, const SimpleHashMap<std::string, std::string>& pairs)
	{
		WebRequest m = WebRequest();
		m.setHeader(type, urlPath);

		//required because you may be sending over cookies which gets processed a little differently
		for(auto& kv : pairs)
		{
			m.addKeyValue(kv.first, kv.second);
		}
		return m;
	}
	WebRequest WebRequest::createQuickServerResponse(unsigned short statusCode, const SimpleHashMap<std::string, std::string>& pairs)
	{
		WebRequest m = WebRequest();
		std::string statusMsg;
		m.setHeader(WebRequest::WebRequest::TYPE_SERVER, StringTools::toString(statusCode) + " " + getStatusMessage(statusCode));
		//required because you may be sending over cookies which gets processed a little differently
		for(auto& kv : pairs)
		{
			m.addKeyValue(kv.first, kv.second);
		}

		return m;
	}

	bool WebRequest::init(unsigned char* buffer, size_t size, size_t* bytesRead)
	{
		static const std::unordered_map<std::string, unsigned int> WebRequestTypeToString = {
			{"GET", WebRequest::TYPE_GET}, {"POST", WebRequest::TYPE_POST}, {"HEAD", WebRequest::TYPE_HEAD}, {"PUT", WebRequest::TYPE_PUT}, {"DELETE", WebRequest::TYPE_DELETE}, 
			{"CONNECT", WebRequest::TYPE_CONNECT}, {"OPTIONS", WebRequest::TYPE_OPTIONS}, {"TRACE", WebRequest::TYPE_TRACE}, {"PATCH", WebRequest::TYPE_PATCH}
		};

		//delete everything and restart
		reset();

		//assume first line is header which has the type and HTTP/1.1
		if(size == 0)
			return 0;
		
		size_t i = 0;
		while(i < size)
		{
			//read header
			if(buffer[i] >= 32)
				header += (char)buffer[i];
			else if(buffer[i] == '\n')
				break;
			i++;
		}
		i++;

		//get type from header
		size_t firstSpaceIndex = header.find_first_of(' ');
		size_t lastSpaceIndex = header.find_last_of(' ');
		
		std::string typeName = StringTools::toUppercase(header.substr(0, firstSpaceIndex));
		auto it = WebRequestTypeToString.find(typeName);
		if(it != WebRequestTypeToString.end())
			type = it->second;
		else
		 	type = WebRequest::TYPE_SERVER;
		
		if(this->type == WebRequest::TYPE_SERVER)
		{
			pathOrMessage = header.substr(firstSpaceIndex+1);
			statusCode = 0;
			//the first value in the data part should be a number which is the status code
			//on the off case, remove starting spaces
			std::string tempNumber;
			for(int i=0; i<pathOrMessage.size(); i++)
			{
				if(pathOrMessage[i] == ' ' && !pathOrMessage.empty())
					break;
				else if(pathOrMessage[i] != ' ')
					tempNumber += pathOrMessage[i];
			}
			if(!tempNumber.empty())
				statusCode = StringTools::toInt(tempNumber);
		}
		else
		{
			//path is everything between header and http version
			//basically, stuff between the first space and the last space
			if(header[firstSpaceIndex+1] == '/')
				pathOrMessage = header.substr(firstSpaceIndex+1, lastSpaceIndex-firstSpaceIndex-1);
			else
				pathOrMessage = "/" + header.substr(firstSpaceIndex+1, lastSpaceIndex-firstSpaceIndex-1);
		}

		//read all key value pairs
		std::string key, value;
		bool mode = false;
		bool properEnd = false;
		while(i < size)
		{
			if(mode == false)
			{
				if(buffer[i] == ':')
				{
					mode = true;
					i++;
				}
				else if(buffer[i] >= 32)
					key += buffer[i];
				else if(buffer[i] == '\n')
				{
					i++;
					properEnd = true;
					break; //Empty line. Everything after is body
				}
			}
			else
			{
				if(buffer[i] >= 32)
				{
					value += buffer[i];
				}
				else if(buffer[i] == '\n')
				{
					//add key value pair
					addKeyValue(key, value);
					key = "";
					value = "";
					mode = false;
				}
			}
			i++;
		}

		bytesInHeader = i;
		if(bytesRead != nullptr)
			*bytesRead = i;
		return properEnd;
	}

	void WebRequest::setHeader(unsigned int type, const std::string& data)
	{
		static const std::vector<std::string> TypeToString = {"GET ", "POST ", "HEAD ", "PUT ", "DELETE ", "CONNECT ", "OPTIONS ", "TRACE ", "PATCH "};
		this->type = type;
		if(type < TypeToString.size())
			header = TypeToString[type];
		else
		{
			header = "HTTP/1.1 ";
			this->type = WebRequest::TYPE_SERVER;
		}
		pathOrMessage = data;
		header += data;
		if(this->type != WebRequest::TYPE_SERVER)
			header += " HTTP/1.1";
		
		statusCode = 0;
		if(this->type == WebRequest::TYPE_SERVER)
		{
			//the first value in the data part should be a number which is the status code
			//on the off case, remove starting spaces
			std::string tempNumber;
			for(int i=0; i<data.size(); i++)
			{
				if(data[i] == ' ' && !data.empty())
					break;
				else if(data[i] != ' ')
					tempNumber += data[i];
			}
			if(!tempNumber.empty())
				statusCode = StringTools::toInt(tempNumber);
		}
		else
		{
			if(pathOrMessage.front() != '/')
				pathOrMessage = "/" + pathOrMessage;
		}
	}

	std::string WebRequest::getHeader() const
	{
		return header;
	}

	std::string WebRequest::getPathOrMessage() const
	{
		return pathOrMessage;
	}

	unsigned int WebRequest::getType() const
	{
		return type;
	}

	void WebRequest::addKeyValue(const std::string& k, const std::string& v)
	{
		std::string lowerK = StringTools::toLowercase(k);
		if(lowerK == "cookie")
		{
			cookieMap.loadClientSentCookies(k+": "+v);
		}
		else if(lowerK == "set-cookie")
		{
			cookieMap.loadServerSentCookie(k+": "+v);
		}
		else
		{
			data[lowerK] = v;
		}
	}

	std::string WebRequest::readKeyValue(const std::string& k) const
	{
		auto it = data.find(StringTools::toLowercase(k));
		if(it != data.end())
			return it->second;
		return "";
	}

	bool WebRequest::empty() const
	{
		return header.empty() && data.empty();
	}

	size_t WebRequest::getBytesInRequest() const
	{
		size_t bytesInHeader = 0;
		if(header.empty())
			return 0;
		
		bytesInHeader += header.size() + 2;
		
		for(auto it = data.begin(); it != data.end(); ++it)
		{
			bytesInHeader += it->first.size() + 2 + it->second.size() + 2;
		}

		if(!cookieMap.empty())
		{
			//add cookies
			if(type == WebRequest::TYPE_SERVER)
			{
				for(auto& it : cookieMap.getAllCookies())
				{
					WebCookie wc = it.second;
					bytesInHeader += it.second.getCookieAsString(true).size() + 2;
				}
			}
			else
			{
				for(auto& it : cookieMap.getAllCookies())
				{
					WebCookie wc = it.second;
					bytesInHeader += it.second.getCookieAsString(false).size() + 1;
				}
				bytesInHeader += 2;
			}
		}
		bytesInHeader += 2;
		return bytesInHeader;
	}

	unsigned int WebRequest::getStatus() const
	{
		return statusCode;
	}
	
	void WebRequest::setCookies(const CookieManager& existingCookieMap)
	{
		cookieMap = existingCookieMap;
	}
	CookieManager& WebRequest::getCookieMap()
	{
		return cookieMap;
	}
	const CookieManager& WebRequest::getCookieMap() const
	{
		return cookieMap;
	}

	std::string WebRequest::getRequestAsString() const
	{
		std::string buffer;
		if(header.empty())
			return "";
		
		buffer += header + "\r\n";
		
		for(auto it = data.begin(); it != data.end(); ++it)
		{
			buffer += it->first + ": " + it->second + "\r\n";
		}

		if(!cookieMap.empty())
		{
			//add cookies
			if(type == WebRequest::TYPE_SERVER)
			{
				for(auto& it : cookieMap.getAllCookies())
				{
					WebCookie wc = it.second;
					buffer += it.second.getCookieAsString(true);
					buffer += "\r\n";
				}
			}
			else
			{
				for(auto& it : cookieMap.getAllCookies())
				{
					WebCookie wc = it.second;
					buffer += it.second.getCookieAsString(false) + " ";
				}
				buffer += "\r\n";
			}
		}
		buffer += "\r\n";

		// bytesInHeader = buffer.size();
		return buffer;
	}

	std::string WebRequest::getMimeTypeFromExt(const std::string& ext)
    {
        if(ext.empty())
            return "application/octet-stream";

        std::string actualExt = ext;
        if(actualExt.front() == '.')
        {
            actualExt = actualExt.substr(1);
        }
        actualExt = StringTools::toLowercase(actualExt);

        auto it = mimeTypes.find(actualExt);
        if(it != mimeTypes.end())
        {
            return it->second;
        }

        return "application/octet-stream";
    }

	std::string WebRequest::getDateAsGMT()
    {
        //get date time as GMT
        std::time_t currTime = time(nullptr);
        std::stringstream strBuffer;
		
        strBuffer << std::put_time(std::gmtime(&currTime), "%a, %d %b %Y %H:%M:%S GMT");

        return strBuffer.str();
    }

	
	bool WebRequest::isValidCookieFormat(const std::string& v)
	{
		//abdefghijklmnqrstuvxyzABDEFGHIJKLMNQRSTUVXYZ0123456789!#$%&'()*+-./:<>?@[]^_`{|}~
		for(char c : v)
		{
			//can't guarantee that the characters above 127 are safe to use
			if(c <= 32 || c==';' || c== '=' || c == '\\' || c==',' || c > 127)
			{
				return false;
			}
		}
		return true;
	}
	
	std::string WebRequest::getStatusMessage(unsigned short statusCode)
	{
		auto it = statusAndMessages.find(statusCode);
		if(it != statusAndMessages.end())
			return it->second;
		return "";
	}

	const std::string WebRequest::MIME_FORM_DATA = "application/x-www-form-urlencoded";
	const SimpleHashMap<std::string, std::string> WebRequest::mimeTypes = {
        {"aac", "audio/aac"},
        {"apng", "image/apng"},
        {"avif", "image/avif"},
        {"avi", "video/x-msvideo"},
        {"bin", "application/octet-stream"},
        {"bmp", "image/bmp"},
        {"bz", "application/x-bzip"},
        {"bz2", "application/x-bzip2"},
        {"css", "text/css"},
        {"csv", "text/csv"},
        {"doc", "application/msword"},
        {"docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
        {"eot", "application/vnd.ms-fontobject"},
        {"gz", "application/gzip"},
        {"gif", "image/gif"},
        {"htm", "text/html"},
        {"html", "text/html"},
        {"ico", "image/vnd.microsoft.icon"},
        {"ics", "text/calendar"},
        {"jar", "application/java-archive"},
        {"jpg", "image/jpg"},
        {"jpeg", "image/jpeg"},
        {"js", "text/javascript"},
        {"json", "application/json"},
        {"jsonld", "application/ld+json"},
        {"mid", "audio/midi"},
        {"midi", "audio/midi"},
        {"mjs", "text/javascript"},
		{"mkv", "video/x-matroska"},
        {"mp3", "audio/mpeg"},
        {"mp4", "video/mp4"},
        {"mpeg", "video/mpeg"},
        {"oga", "audio/ogg"},
        {"ogg", "video/ogg"},
        {"ogv", "video/ogg"},
        {"ogx", "application/ogg"},
        {"opus", "audio/opus"},
        {"otf", "font/otf"},
        {"png", "image/png"},
        {"pdf", "application/pdf"},
        {"php", "application/x-httpd-php"},
        {"ppt", "application/vnd.ms-powerpoint"},
        {"pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
        {"rar", "application/vnd.rar"},
        {"rtf", "application/rtf"},
        {"sh", "application/x-sh"},
        {"svg", "image/svg+xml"},
        {"tar", "application/x-rar"},
        {"tif", "image/tiff"},
        {"tiff", "image/tiff"},
        {"ts", "video/mp2t"},
        {"ttf", "font/ttf"},
        {"txt", "text/plain"},
        {"wav", "audio/wav"},
        {"weba", "audio/webm"},
        {"webm", "video/webm"},
        {"webp", "image/webp"},
        {"woff", "font/woff"},
        {"woff2", "font/woff2"},
        {"xhtml", "application/xhtml+xml"},
        {"xls", "application/vnd.ms-excel"},
        {"xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
        {"xml", "application/xml"},
        {"zip", "application/zip"},
        {"7z", "application/x-7z-compressed"},
    };

	const SimpleHashMap<unsigned short, std::string> WebRequest::statusAndMessages = {
        {100, "Continue"},
		{101, "Switching Protocols"},
		{102, "Processing"},
		{103, "Early Hints"},
		
		
		{200, "OK"},
		{201, "Created"},
		{202, "Accepted"},
		{203, "Non-Authoritative Information"},
		{204, "No Content"},
		{205, "Reset Content"},
		{206, "PartialContent"},
		{207, "Multi-Status"},
		{208, "Already Reported"},
		{226, "IM Used"},

		
		{300, "Multiple Choices"},
		{301, "Moved Permanently"},
		{302, "Found"},
		{303, "See Other"},
		{304, "Not Modified"},
		{305, "Use Proxy"},
		{306, "Unused"},
		{307, "Temporary Redirect"},
		{308, "Permanent Redirect"},

		
		{400, "Bad Request"},
		{401, "Unauthorized"},
		{402, "Payment Required"},
		{403, "Forbidden"},
		{404, "Not Found"},
		{405, "Method Not Allowed"},
		{406, "Not Acceptable"},
		{407, "Proxy Authentication Required"},
		{408, "Request Timeout"},
		{409, "Conflict"},
		{410, "Gone"},
		{411, "Length Required"},
		{412, "Precondition Failed"},
		{413, "Content Too Large"},
		{414, "URI Too Long"},
		{415, "Unsupported Media Type"},
		{416, "Range Not Satisfiable"},
		{417, "Expectation Failed"},
		{418, "I'm a teapot"},
		{421, "Misdirected Request"},
		{422, "Unprocessable Content"},
		{423, "Locked"},
		{424, "Failed Dependency"},
		{425, "Too Early"},
		{426, "Upgrade Required"},
		{428, "Precondition Required"},
		{429, "Too Many Requests"},
		{431, "Request Header Fields Too Large"},
		{451, "Unavailable For Legal Reasons"},

		
		{500, "Internal Server Error"},
		{501, "Not Implemented"},
		{502, "Bad Gateway"},
		{503, "Service Unavailable"},
		{504, "Gateway Timeout"},
		{505, "HTTP Version Not Supported"},
		{506, "Variant Also Negotiates"},
		{507, "Insufficient Storage"},
		{508, "Loop Detected"},
		{510, "Not Extended"},
		{511, "Network Authentication Required"},
    };
}