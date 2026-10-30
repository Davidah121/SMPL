#include "URL.h"
namespace smpl
{
	URL::URL()
	{

	}
	URL::URL(const std::string& str)
	{
		parseString(str);
	}

	const std::string& URL::getProtocol() const
	{
		return protocol;
	}
	const std::string& URL::getHost() const
	{
		return host;
	}
	const std::string& URL::getPath() const
	{
		return path;
	}
	const uint16_t& URL::getPort() const
	{
		return port;
	}
	const std::vector<std::pair<std::string, std::string>>& URL::getArguments() const
	{
		return arguments;
	}

	void URL::parseString(const std::string& urlString)
	{
		if(urlString.empty())
			return;

		size_t knownOffset = 0;
		size_t locationOfProtocolSeparator = urlString.find("://");
		if(locationOfProtocolSeparator != std::string::npos)
		{
			protocol = urlString.substr(knownOffset, locationOfProtocolSeparator);
			knownOffset = locationOfProtocolSeparator+3;
		}

		size_t locationOfArgumentStart = urlString.find('?', knownOffset);
		size_t locationOfPathStart = urlString.find('/', knownOffset);

		if(locationOfPathStart != std::string::npos)
		{
			host = urlString.substr(knownOffset, locationOfPathStart-knownOffset);
			knownOffset = locationOfPathStart+1;
			//path may exist along with arguments
		}
		else if(locationOfArgumentStart != std::string::npos)
		{
			host = urlString.substr(knownOffset, locationOfPathStart-knownOffset);
			knownOffset = locationOfPathStart+1;
			//no path. arguments exist though
		}
		else
		{
			host = urlString.substr(knownOffset, std::string::npos); //grab remaining info as the host
			knownOffset = std::string::npos;
		}

		if(!host.empty())
		{
			size_t locationOfPortStart = host.find(':');
			bool parsedPort = false;
			if(locationOfPortStart != std::string::npos)
			{
				std::string portStr = host.substr(locationOfPortStart+1);
				try {
					port = std::stoi(portStr);
					parsedPort = true;
				} catch (...) {
					//ignore
				}
			}
			if(parsedPort == false)
			{
				if(!protocol.empty())
				{
					auto it = knownProtocolPorts.find(protocol);
					if(it != knownProtocolPorts.end())
						port = it->second;
				}
			}
			host = host.substr(0, locationOfPortStart);
		}

		if(locationOfPathStart != std::string::npos)
		{
			path = urlString.substr(knownOffset-1, locationOfArgumentStart-knownOffset);
			knownOffset = locationOfArgumentStart + 1;
		}

		if(locationOfArgumentStart != std::string::npos)
		{
			std::string currentKey = "";
			std::string currentValue = "";
			for(size_t i=knownOffset; i<urlString.size(); i++)
			{
				if(urlString[i] == '&')
				{
					if(!currentKey.empty() || !currentValue.empty())
						arguments.push_back({currentKey, currentValue});
					currentKey = "";
					currentValue = "";
				}
				else if(urlString[i] == '=')
				{
					currentKey = std::move(currentValue);
					currentValue = "";
				}
				else
				{
					currentValue += urlString[i];
				}
			}
			if(!currentKey.empty() || !currentValue.empty())
				arguments.push_back({currentKey, currentValue});
			currentKey = "";
			currentValue = "";
		}
	}
}