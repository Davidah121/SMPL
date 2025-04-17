#include "StringBridge.h"
#include "StringTools.h"

namespace smpl
{
    
    const std::u32string& StringBridge::getData() const
    {
        return data;
    }
    
	std::string StringBridge::toPrintableFormat() const
    {
        return StringTools::toUTF8String(data);
    }

    StringBridge::StringBridge()
    {
        
    }

    StringBridge::StringBridge(const std::string& s)
    {
        data.reserve(s.size());
        for(const char c : s)
            data += (char32_t)c;
    }
    StringBridge::StringBridge(const char* s)
    {
        while(true)
        {
            if(*s == 0)
                break;
            
            data += (char32_t)*s;
            s++;
        }
    }
    StringBridge::StringBridge(const std::wstring& s)
    {
        data.reserve(s.size());
        for(const wchar_t c : s)
            data += (char32_t)c;
    }
    
    StringBridge::StringBridge(const wchar_t* s)
    {
        while(true)
        {
            if(*s == 0)
                break;
            
            data += (char32_t)*s;
            s++;
        }
    }
    StringBridge::StringBridge(const std::u32string& s)
    {
        data = s;
    }
    
    StringBridge::StringBridge(const char32_t* s)
    {
        while(true)
        {
            if(*s == 0)
                break;
            
            data += *s;
            s++;
        }
    }

    UnicodeStringBridge::UnicodeStringBridge(const std::string& s)
    {
        data = StringTools::utf8ToIntString(s);
    }
    UnicodeStringBridge::UnicodeStringBridge(const char* s)
    {
        data = StringTools::utf8ToIntString(s);
    }

    UnicodeStringBridge::UnicodeStringBridge(const std::wstring& s)
    {
        data.reserve(s.size());
        for(const wchar_t c : s)
            data += (char32_t)c;
    }
    UnicodeStringBridge::UnicodeStringBridge(const wchar_t* s)
    {
        while(true)
        {
            if(*s == 0)
                break;
            
            data += (char32_t)*s;
            s++;
        }
    }

    UnicodeStringBridge::UnicodeStringBridge(const std::u32string& s)
    {
        data = s;
    }
    UnicodeStringBridge::UnicodeStringBridge(const char32_t* s)
    {
        while(true)
        {
            if(*s == 0)
                break;
            
            data += *s;
            s++;
        }
    }

} // namespace smpl
