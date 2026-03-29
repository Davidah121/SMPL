#include "WebRequest.h"
#include "StringTools.h"
#include "System.h"
#include "SimpleJSON.h"
#include "SimpleFile.h"

namespace smpl
{

	WebCookie::WebCookie()
	{

	}
	WebCookie::WebCookie(const std::string& name, const std::string& value)
	{
		this->name = name;
		this->value = value;
	}
	WebCookie::~WebCookie()
	{

	}

	void WebCookie::setDomain(const std::string& dm)
	{
		domain = dm;
	}
	void WebCookie::setIncludeSubdomains(bool b)
	{
		includeSubdomains = b;
	}
	void WebCookie::setPath(const std::string& path)
	{
		this->path = path;
	}
	void WebCookie::setSecure(bool v)
	{
		secure = v;
	}
	void WebCookie::setHttpOnly(bool v)
	{
		httpOnly = v;
	}
	void WebCookie::setPartitioned(bool v)
	{
		partitioned = v;
	}
	void WebCookie::setExpiresDate(size_t date)
	{
		isMaxAge = false;
		expiresTime = date;
	}
	void WebCookie::setMaxAge(size_t timeInSeconds)
	{
		isMaxAge = true;
		expiresTime = timeInSeconds;
	}

	std::string WebCookie::getName() const
	{
		return name;
	}
	std::string WebCookie::getValue() const
	{
		return value;
	}

	std::string WebCookie::getDomain() const
	{
		return domain;
	}
	bool WebCookie::getIncludeSubdomains() const
	{
		return includeSubdomains;
	}
	std::string WebCookie::getPath() const
	{
		return path;
	}
	bool WebCookie::getSecure() const
	{
		return secure;
	}
	bool WebCookie::getHttpOnly() const
	{
		return httpOnly;
	}
	bool WebCookie::getPartitioned() const
	{
		return partitioned;
	}
	size_t WebCookie::getExpiresTime() const
	{
		return expiresTime;
	}
    
	std::string WebCookie::getCookieAsString(bool isServerSide) const
	{
		std::string buffer = "";
		if(isServerSide)
		{
			buffer = "Set-Cookie: " + name + "=" + value + ";";
			if(secure)
				buffer += " Secure;";
			if(httpOnly)
				buffer += " HttpOnly;";
			if(partitioned)
				buffer += " Partitioned;";
			if(!path.empty())
				buffer += " Path=" + path + ";";
			if(!domain.empty())
				buffer += " Domain=" + domain + ";";
			if(expiresTime != SIZE_MAX)
			{
				if(isMaxAge)
					buffer += " Max-Age= " + StringTools::toString(expiresTime) + ";";
				else
					buffer += " Expires= " + System::timeToDateString(expiresTime) + " GMT;";
			}
		}
		else
		{
			buffer = "Cookie: " + name + "=" + value + ";";
		}
		return buffer;
	}


	CookieManager::CookieManager()
	{

	}
	CookieManager::~CookieManager()
	{

	}

	void CookieManager::addCookies(const CookieManager& cm)
	{
		for(auto& otherCookies : cm.cookieMap)
		{
			addCookie(otherCookies.second);
		}
	}
	void CookieManager::addCookie(const WebCookie& c)
	{
		cookieMap[c.getName()] = c;
	}
	SimpleHashMap<std::string, WebCookie>::Iterator CookieManager::getCookie(const std::string& name)
	{
		return cookieMap.find(name);
	}
	SimpleHashMap<std::string, WebCookie>& CookieManager::getAllCookies()
	{
		return cookieMap;
	}

	const SimpleHashMap<std::string, WebCookie>::ConstIterator CookieManager::getCookie(const std::string& name) const
	{
		return cookieMap.find(name);
	}
	const SimpleHashMap<std::string, WebCookie>& CookieManager::getAllCookies() const
	{
		return cookieMap;
	}
		
	void CookieManager::clear()
	{
		cookieMap.clear();
	}

	bool CookieManager::empty() const
	{
		return cookieMap.empty();
	}

	void CookieManager::loadServerSentCookie(const std::string& line)
	{
		std::string lineStart = line.substr(0, 11);
		if(!StringTools::equalsIgnoreCase<char>(lineStart, "Set-Cookie:"))
		{
			return;
		}
		
		std::vector<std::string> splits = StringTools::splitString(line.substr(12), ';');
		
		if(splits.size() >= 1)
		{
			//name=value
			//everything pass 12 is name minus leading spaces
			size_t indexOfEquals = splits[0].find_first_of('=');
			if(indexOfEquals == SIZE_MAX)
				throw std::runtime_error("ERROR PARSING COOKIES");
			
			
			std::string name = StringTools::removeWhitespace(splits[0].substr(0, indexOfEquals));
			std::string val = StringTools::removeWhitespace(splits[0].substr(indexOfEquals+1));
			WebCookie c = WebCookie(name, val);

			//rest not in any order
			for(size_t i=1; i<splits.size(); i++)
			{
				indexOfEquals = splits[i].find_first_of('=');
				std::string attribName = splits[i];
				std::string attribValue = "";
				if(indexOfEquals != SIZE_MAX)
				{
					attribName = StringTools::removeWhitespace(splits[i].substr(0, indexOfEquals));
					attribValue = StringTools::removeWhitespace(splits[i].substr(indexOfEquals+1));
				}

				if(StringTools::equalsIgnoreCase<char>(attribName, "Secure"))
				{
					c.setSecure(true);
				}
				else if(StringTools::equalsIgnoreCase<char>(attribName, "HttpOnly"))
				{
					c.setHttpOnly(true);
				}
				else if(StringTools::equalsIgnoreCase<char>(attribName, "Partitioned"))
				{
					c.setPartitioned(true);
				}
				else if(StringTools::equalsIgnoreCase<char>(attribName, "Path"))
				{
					c.setPath(attribValue);
				}
				else if(StringTools::equalsIgnoreCase<char>(attribName, "Domain"))
				{
					c.setDomain(attribValue);
					c.setIncludeSubdomains(true);
				}
				else if(StringTools::equalsIgnoreCase<char>(attribName, "Expires"))
				{
					c.setExpiresDate(System::timeFromDateString("%a, %d %b %Y %X GMT", attribValue));
				}
				else if(StringTools::equalsIgnoreCase<char>(attribName, "Max-Age"))
				{
					c.setMaxAge(StringTools::toULongLong(attribValue));
				}
				else
				{
					//unknown. Should throw an error.... maybe
				}
			}

			addCookie(c);
		}
	}

	void CookieManager::loadClientSentCookies(const std::string& line)
	{
		std::string lineStart = line.substr(0, 7);
		if(!StringTools::equalsIgnoreCase<char>(lineStart, "Cookie:"))
		{
			return;
		}
		
		//name=value; name=value;
		//everything pass 7 is the remaining cookies
		//split by the '=' sign and the ';' which separate name from value and each individual cookie respectively
		std::vector<std::string> remainingCookies = StringTools::splitString(line.substr(8), ';');
		for(size_t i=0; i<remainingCookies.size(); i++)
		{
			size_t indexOfFirstEquals = remainingCookies[i].find_first_of('=');
			if(indexOfFirstEquals == SIZE_MAX)
				throw std::runtime_error("ERROR PARSING COOKIES");
			
			WebCookie c = WebCookie(StringTools::removeWhitespace(remainingCookies[i].substr(0, indexOfFirstEquals)), remainingCookies[i].substr(indexOfFirstEquals+1));
			addCookie(c);
		}
	}

	//FIX LATER
	void CookieManager::loadCookiesJSON(const std::string& file)
	{
		cookieMap.clear();
		SimpleJSON json = SimpleJSON(file);

		std::vector<JNode*> nodes = json.getNodesPattern({""});//should be an array at the start so this should ideally get every object
		for(JNode* obj : nodes)
		{
			try
			{
				std::string name = ((JPair*)obj->getNodesPattern({"name"})[0])->getValue();
				std::string value = ((JPair*)obj->getNodesPattern({"value"})[0])->getValue();
				std::string domain = ((JPair*)obj->getNodesPattern({"domain"})[0])->getValue();
				std::string path = ((JPair*)obj->getNodesPattern({"path"})[0])->getValue();
				std::string expires = ((JPair*)obj->getNodesPattern({"expires"})[0])->getValue();
				std::string httpOnly = ((JPair*)obj->getNodesPattern({"httponly"})[0])->getValue();
				std::string secure = ((JPair*)obj->getNodesPattern({"secure"})[0])->getValue();

				//may not even exist. Doesn't have to either
				std::string hostOnly  = ((JPair*)obj->getNodesPattern({"hostonly"})[0])->getValue();

				WebCookie newCookie = WebCookie(name, value);
				newCookie.setDomain(domain);
				newCookie.setIncludeSubdomains(hostOnly == "true");
				newCookie.setPath(path);
				newCookie.setHttpOnly(httpOnly == "true");
				newCookie.setSecure(secure == "true");
				newCookie.setExpiresDate(System::timeFromDateString("%a, %d %b %Y %X GMT", expires));

				addCookie(newCookie);
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
			
		}
	}

	void CookieManager::loadCookiesNetscape(const std::string& file)
	{
		//tab separating each field.
		cookieMap.clear();
		SimpleFile f = SimpleFile(file, SimpleFile::READ);
		if(f.isOpen())
		{
			std::string temp = "";
			std::vector<unsigned char> bytes = f.readFullFileAsBytes();
			std::vector<size_t> tabLocations;
			for(size_t i=0; i < bytes.size(); i++)
			{
				if(bytes[i] >= 32)
					temp+=bytes[i];
				else if(bytes[i] == '\t')
				{
					tabLocations.push_back(i);
				}
				else if(bytes[i] == '\n')
				{
					if(temp.front() != '#')
					{
						std::string nameSubstr = temp.substr(tabLocations[4]+1, tabLocations[5]-tabLocations[4]-1);
						std::string valueSubstr = temp.substr(tabLocations[5]+1);
						std::string domainSubstr = temp.substr(0, tabLocations[0]);
						std::string subDomainSubstr = temp.substr(tabLocations[0]+1, tabLocations[1]-tabLocations[0]-1);
						std::string pathSubstr = temp.substr(tabLocations[1]+1, tabLocations[2]-tabLocations[1]-1);
						std::string httpOnlySubstr = temp.substr(tabLocations[2]+1, tabLocations[3]-tabLocations[2]-1);
						std::string expiresSubstr = temp.substr(tabLocations[3]+1, tabLocations[4]-tabLocations[3]-1);
						
						if(tabLocations.size() == 6) //should be exactly 6 tabs. No more or less
						{
							WebCookie newCookie = WebCookie(nameSubstr, valueSubstr);
							newCookie.setDomain(domainSubstr);
							newCookie.setIncludeSubdomains(subDomainSubstr == "TRUE");
							newCookie.setPath(pathSubstr);
							newCookie.setHttpOnly(httpOnlySubstr == "TRUE");
							newCookie.setExpiresDate(StringTools::toULongLong(expiresSubstr));

							addCookie(newCookie);
						}
					}
					tabLocations.clear();
					temp = "";
				}
			}
			if(!temp.empty())
			{
				if(temp.front() != '#')
				{
					std::string nameSubstr = temp.substr(tabLocations[4]+1, tabLocations[5]-tabLocations[4]-1);
					std::string valueSubstr = temp.substr(tabLocations[5]+1);
					std::string domainSubstr = temp.substr(0, tabLocations[0]);
					std::string subDomainSubstr = temp.substr(tabLocations[0]+1, tabLocations[1]-tabLocations[0]-1);
					std::string pathSubstr = temp.substr(tabLocations[1]+1, tabLocations[2]-tabLocations[1]-1);
					std::string httpOnlySubstr = temp.substr(tabLocations[2]+1, tabLocations[3]-tabLocations[2]-1);
					std::string expiresSubstr = temp.substr(tabLocations[3]+1, tabLocations[4]-tabLocations[3]-1);
					
					if(tabLocations.size() == 6) //should be exactly 6 tabs. No more or less
					{
						WebCookie newCookie = WebCookie(nameSubstr, valueSubstr);
						newCookie.setDomain(domainSubstr);
						newCookie.setIncludeSubdomains(subDomainSubstr == "TRUE");
						newCookie.setPath(pathSubstr);
						newCookie.setHttpOnly(httpOnlySubstr == "TRUE");
						newCookie.setExpiresDate(StringTools::toULongLong(expiresSubstr));

						addCookie(newCookie);
					}
				}
				tabLocations.clear();
				temp = "";
			}
			f.close();
		}
	}

	void CookieManager::saveCookiesJSON(const std::string& file)
	{
		SimpleFile f = SimpleFile(file, SimpleFile::WRITE);
		if(f.isOpen())
		{
			f.writeString("[\n");

			std::string line = "";
			for(auto& wc : cookieMap)
			{
				if(!line.empty())
				{
					f.writeString(line);
				}
				line = "";
				line += "\t{\n";
				line += "\t\t\"domain:\" \"" + wc.second.getDomain() + "\",\n";
				line += "\t\t\"name:\" \"" + wc.second.getName() + "\",\n";
				line += "\t\t\"value:\" \"" + wc.second.getValue() + "\",\n";
				line += "\t\t\"path:\" \"" + wc.second.getPath() + "\",\n";
				
				std::string v1 = (wc.second.getHttpOnly() ? "true" : "false");
				std::string v2 = (wc.second.getIncludeSubdomains() ? "true" : "false");
				std::string v3 = (wc.second.getSecure() ? "true" : "false");

				line += "\t\t\"httpOnly\": " + v1 + "\",\n";
				line += "\t\t\"hostOnly\": " + v2 + "\",\n";
				line += "\t\t\"secure\": " + v3 + "\",\n";
				line += "\t\t\"expires:\" \"" + StringTools::toString(wc.second.getExpiresTime()) + "\",\n";
				line += "\t},\n";
			}
			if(!line.empty())
			{
				//remove comma
				line.pop_back();
				line.pop_back();
				f.writeString(line+"\n");
			}
			
			f.writeString("]");
		}
		f.close();
	}

	void CookieManager::saveCookiesNetscape(const std::string& file)
	{
		SimpleFile f = SimpleFile(file, SimpleFile::WRITE);
		if(f.isOpen())
		{
			for(auto& wc : cookieMap)
			{
				f.writeString(wc.second.getDomain() + "\t");
				f.writeString((wc.second.getIncludeSubdomains() ? "TRUE\t" : "FALSE\t"));
				f.writeString(wc.second.getPath() + "\t");
				f.writeString((wc.second.getHttpOnly() ? "TRUE\t" : "FALSE\t"));
				f.writeString(StringTools::toString(wc.second.getExpiresTime()) + "\t");
				f.writeString(wc.second.getName() + "\t");
				f.writeString(wc.second.getValue() + "\n");
			}
		}
		f.close();
	}
}