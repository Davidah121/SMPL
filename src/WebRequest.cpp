#include "WebRequest.h"
#include "StringTools.h"
#include <iomanip>

namespace glib
{
	WebRequest::WebRequest()
	{

	}

	WebRequest::WebRequest(char* buffer, size_t size)
	{
		init((unsigned char*)buffer, size);
	}

	WebRequest::WebRequest(std::string buffer)
	{
		init((unsigned char*)buffer.data(), buffer.size());
	}

	WebRequest::WebRequest(std::vector<unsigned char> buffer)
	{
		//fill out data using buffer
		init((unsigned char*)buffer.data(), buffer.size());
	}

	void WebRequest::init(unsigned char* buffer, size_t size)
	{
		//assume first line is header which has the type and HTTP/1.1

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
		std::string typeName = header.substr(0, header.find(' '));
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
			type = TYPE_UNKNOWN;
		
		//read all key value pairs
		std::string key, value;
		bool mode = false;
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
					break; //Empty line. Everything after is body
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

		//Process body. May not exist. It should be valid base64
		while(i < size)
		{
			body += (char)buffer[i];
			i++;
		}
	}

	void WebRequest::setHeader(char type, std::string data, bool includeHTTP)
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
			header = "";

		header += data;
		if(includeHTTP)
			header += " HTTP/1.1";
	}

	std::string WebRequest::getHeader()
	{
		return header;
	}

	char WebRequest::getType()
	{
		return type;
	}

	void WebRequest::addKeyValue(std::string k, std::string v)
	{
		data[k] = v;
	}

	std::string WebRequest::readKeyValue(std::string k)
	{
		auto it = data.find(k);
		if(it != data.end())
			return it->second;
		return "";
	}
	
	void WebRequest::setBody(std::string s)
	{
		body = s;
	}
	
	void WebRequest::setBody(unsigned char* buffer, size_t size)
	{
		body.clear();
		for(size_t i=0; i<size; i++)
		{
			body += (char)buffer[i];
		}
	}

	std::string WebRequest::getBody()
	{
		return body;
	}

	std::string WebRequest::getRequestAsString()
	{
		std::string buffer;
		if(header.empty())
			return "";
		
		buffer += header + "\r\n";
		
		for(auto it = data.begin(); it != data.end(); it++)
		{
			buffer += it->first + ": " + it->second + "\r\n";
		}
		buffer += "\r\n";

		if(!body.empty())
		{
			buffer += body;
			// buffer += "\r\n";
		}
		return buffer;
	}

	std::string WebRequest::getMimeTypeFromExt(std::string ext)
    {
        if(ext.empty())
            return "application/octet-stream";
        
        if(ext.front() == '.')
        {
            ext = ext.substr(1);
        }
        ext = StringTools::toLowercase(ext);

        auto it = mimeTypes.find(ext);
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

	const std::unordered_map<std::string, std::string> WebRequest::mimeTypes = {
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