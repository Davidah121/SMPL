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
    WebCookie::WebCookie(std::string name, std::string value)
    {
        this->name = name;
        this->value = value;
    }
    WebCookie::~WebCookie()
    {

    }
    
    void WebCookie::setDomain(std::string dm)
    {
        domain = dm;
    }
    void WebCookie::setIncludeSubdomains(bool b)
    {
        includeSubdomains = b;
    }
    void WebCookie::setPath(std::string path)
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
    
	std::string WebCookie::getName()
    {
        return name;
    }
	std::string WebCookie::getValue()
    {
        return value;
    }
    
    std::string WebCookie::getDomain()
    {
        return domain;
    }
    bool WebCookie::getIncludeSubdomains()
    {
        return includeSubdomains;
    }
    std::string WebCookie::getPath()
    {
        return path;
    }
    bool WebCookie::getSecure()
    {
        return secure;
    }
    bool WebCookie::getHttpOnly()
    {
        return httpOnly;
    }
    bool WebCookie::getPartitioned()
    {
        return partitioned;
    }
    size_t WebCookie::getExpiresTime()
    {
        return expiresTime;
    }
    
	std::string WebCookie::getCookieAsString(bool isServerSide)
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
    void CookieManager::addCookie(WebCookie c)
    {
        cookieMap[c.getName()] = c;
    }
    WebCookie& CookieManager::getCookie(std::string name)
    {
        return cookieMap[name];
    }
    std::unordered_map<std::string, WebCookie>& CookieManager::getAllCookies()
    {
        return cookieMap;
    }

    void CookieManager::loadServerSentCookie(std::string line)
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

    void CookieManager::loadClientSentCookies(std::string line)
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
    
    void CookieManager::loadCookiesJSON(std::string file)
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

                WebCookie newCookie = WebCookie(name, value);
                newCookie.setDomain(domain);
                newCookie.setIncludeSubdomains(false); //unsure
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
    void CookieManager::loadCookiesNetscape(std::string file)
    {
        //tab separating each field.
        cookieMap.clear();
        SimpleFile f = SimpleFile(file, SimpleFile::READ);
        if(f.isOpen())
        {
            std::string temp = "";
            std::vector<unsigned char> bytes = f.readFullFileAsBytes();
            for(size_t i=0; i < bytes.size(); i++)
            {
                if(bytes[i] >= 32)
                    temp+=bytes[i];
                else if(bytes[i] == '\n')
                {
                    if(temp.front() != '#')
                    {
                        std::vector<std::string> fields = StringTools::splitString(temp, '\t');
                        if(fields.size() == 7)
                        {
                            WebCookie newCookie = WebCookie(fields[5], fields[6]);
                            newCookie.setDomain(fields[0]);
                            newCookie.setIncludeSubdomains(fields[1] == "TRUE");
                            newCookie.setPath(fields[2]);
                            newCookie.setHttpOnly(fields[3] == "TRUE");
                            newCookie.setExpiresDate(StringTools::toULongLong(fields[4]));

                            addCookie(newCookie);
                        }
                    }
                    temp = "";
                }
            }
            if(!temp.empty())
            {
                if(temp.front() != '#')
                {
                    std::vector<std::string> fields = StringTools::splitString(temp, '\t');
                    if(fields.size() == 7)
                    {
                        WebCookie newCookie = WebCookie(fields[5], fields[6]);
                        newCookie.setDomain(fields[0]);
                        newCookie.setIncludeSubdomains(fields[1] == "TRUE");
                        newCookie.setPath(fields[2]);
                        newCookie.setHttpOnly(fields[3] == "TRUE");
                        newCookie.setExpiresDate(StringTools::toULongLong(fields[4]));

                        addCookie(newCookie);
                    }
                }
                temp = "";
            }
            f.close();
        }
    }

    void CookieManager::saveCookiesJSON(std::string file)
    {
        
    }

    void CookieManager::saveCookiesNetscape(std::string file)
    {
        
    }
}