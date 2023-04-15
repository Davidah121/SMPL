#include "WebRequest.h"

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
					break;
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
		return buffer;
	}
}