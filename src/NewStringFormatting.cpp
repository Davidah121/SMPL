#include "NewStringFormatting.h"

namespace smpl
{
    void formatStringInternal(StringStream& output, const char* format)
    {
        while(true)
        {
            if(*format == '\0')
                break;
            output.write(*format);
            format++;
        }
    }
	
	void formatToString(StringStream& stream, const std::string& v, const std::string& options)
	{
		stream.write(v);
	}
    
    StringStream::StringStream()
	{

	}
	StringStream::~StringStream()
	{

	}
	void StringStream::write(const std::string& s)
	{
		buffer += s;
	}
    
    void StringStream::write(const char& s)
    {
        buffer += s;
    }
	
    std::string& StringStream::getBuffer()
	{
		return buffer;
	}
	void StringStream::pop()
	{
		buffer.pop_back();
	}
	size_t StringStream::size()
	{
		return buffer.size();
	}
}
