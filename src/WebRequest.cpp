#include "WebRequest.h"
#include "StringTools.h"
#include <iomanip>

namespace smpl
{
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
	
	WebRequest::WebRequest(WebRequest& other)
	{
		reset();
		type = other.type;
		bytesInHeader = other.bytesInHeader;
		header = other.header;
		data = other.data;
		url = other.url;
		cookieMap = other.cookieMap;
	}

	void WebRequest::operator=(WebRequest& other)
	{
		reset();
		type = other.type;
		bytesInHeader = other.bytesInHeader;
		header = other.header;
		data = other.data;
		url = other.url;
		cookieMap = other.cookieMap;
	}
	
	WebRequest::WebRequest(WebRequest&& other) noexcept
	{
		reset();
		type = std::move(other.type);
		bytesInHeader = std::move(other.bytesInHeader);
		header = std::move(other.header);
		data = std::move(other.data);
		url = std::move(other.url);
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
		url = std::move(other.url);
		cookieMap = std::move(other.cookieMap);
		other.reset();
	}

	void WebRequest::reset()
	{
		type = TYPE_SERVER;
		bytesInHeader = 0;
		header = "";
		cookieMap.clear();
		data.clear();
	}

	bool WebRequest::init(unsigned char* buffer, size_t size, size_t* bytesRead)
	{
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
		
		std::string typeName = header.substr(0, firstSpaceIndex);
		if(typeName == "CONNECT")
			type = TYPE_CONNECT;
		else if(typeName == "DELETE")
			type = TYPE_DELETE;
		else if(typeName == "GET")
			type = TYPE_GET;
		else if(typeName == "HEAD")
			type = TYPE_HEAD;
		else if(typeName == "OPTIONS")
			type = TYPE_OPTIONS;
		else if(typeName == "PATCH")
			type = TYPE_PATCH;
		else if(typeName == "POST")
			type = TYPE_POST;
		else if(typeName == "PUT")
			type = TYPE_PUT;
		else if(typeName == "TRACE")
			type = TYPE_TRACE;
		else
			type = TYPE_SERVER;
		
		//url is everything between header and http version
		//basically, stuff between the first space and the last space
		url = header.substr(firstSpaceIndex+1, lastSpaceIndex-firstSpaceIndex-1);
		
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

	void WebRequest::setHeader(unsigned int type, const std::string& data, bool includeHTTP)
	{
		this->type = type;
		if(type == TYPE_CONNECT)
			header = "CONNECT ";
		else if(type == TYPE_DELETE)
			header = "DELETE ";
		else if(type == TYPE_GET)
			header = "GET ";
		else if(type == TYPE_HEAD)
			header = "HEAD ";
		else if(type == TYPE_OPTIONS)
			header = "OPTIONS ";
		else if(type == TYPE_PATCH)
			header = "PATCH ";
		else if(type == TYPE_POST)
			header = "POST ";
		else if(type == TYPE_PUT)
			header = "PUT ";
		else if(type == TYPE_TRACE)
			header = "TRACE ";
		else
		{
			header = "";
			this->type = TYPE_SERVER;
		}

		if(this->type != TYPE_SERVER)
			url = data; //assume url is data
		else
			url = "";
		
		header += data;
		if(includeHTTP)
			header += " HTTP/1.1";
	}

	std::string WebRequest::getHeader() const
	{
		return header;
	}

	std::string WebRequest::getUrl() const
	{
		return url;
	}

	unsigned int WebRequest::getType() const
	{
		return type;
	}

	void WebRequest::addKeyValue(const std::string& k, const std::string& v)
	{
		std::string lowerK = StringTools::toLowercase(k);
		if(k == "cookie")
		{
			cookieMap.loadClientSentCookies(k+": "+v);
		}
		else if(k == "set-cookie")
		{
			cookieMap.loadServerSentCookie(k+": "+v);
		}
		else
		{
			data[k] = v;
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
			if(type == TYPE_SERVER)
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

	CookieManager& WebRequest::getCookieMap()
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
			if(type == TYPE_SERVER)
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
}