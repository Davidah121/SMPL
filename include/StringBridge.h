#pragma once
#include <string>

namespace smpl
{
    class StringBridge
    {
    public:
        StringBridge(const std::string& s);
        StringBridge(const char* s);
        StringBridge(const std::wstring& s);
        StringBridge(const wchar_t* s);
        StringBridge(const std::u32string& s);
        StringBridge(const char32_t* s);

        const std::u32string& getData() const;
        std::string toPrintableFormat() const;
    protected:
        StringBridge();
        std::u32string data;
    };

    class UnicodeStringBridge : public StringBridge
    {
    public:
        UnicodeStringBridge(const std::string& s);
        UnicodeStringBridge(const char* s);
        UnicodeStringBridge(const std::wstring& s);
        UnicodeStringBridge(const wchar_t* s);
        UnicodeStringBridge(const std::u32string& s);
        UnicodeStringBridge(const char32_t* s);
    };
}