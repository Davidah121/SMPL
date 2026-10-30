#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>

namespace smpl
{
	class URL
	{
	public:
		URL();
		URL(const std::string& str);

		const std::string& getProtocol() const;
		const std::string& getHost() const;
		const std::string& getPath() const;
		const uint16_t& getPort() const;
		const std::vector<std::pair<std::string, std::string>>& getArguments() const;

	private:
		void parseString(const std::string& urlString);
		std::string protocol = "http";
		std::string host;
		std::string path;
		uint16_t port = 80;
		std::vector<std::pair<std::string, std::string>> arguments;

		static const std::unordered_map<std::string, uint16_t> knownProtocolPorts;
	};

	inline const std::unordered_map<std::string, uint16_t> URL::knownProtocolPorts = {
		{"http", 80},
		{"https", 443},
		{"ws", 80},
		{"wss", 443}
	};
}