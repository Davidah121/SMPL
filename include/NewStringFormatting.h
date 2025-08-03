#pragma once
#include <iostream>
#include <vector>

namespace smpl
{
    class StringStream
    {
    public:
        StringStream();
        ~StringStream();
        void write(const std::string& s);
        void write(const char& s);
        void pop();
        size_t size();
        std::string& getBuffer();
    private:
        std::string buffer;
    };

    template<typename T>
    void formatToString(StringStream& stream, const T& v, const std::string& options);

    void formatStringInternal(StringStream& stream, const char* format);

    template<typename T, typename... Args>
    void formatStringInternal(StringStream& stream, const char* format, const T& v, const Args&... args);

    
	template<typename T, typename... Args>
	void formatStringInternal(StringStream& stream, const char* format, const T& v, const Args&... args)
	{
		bool parsingOptions = false;
		bool shouldDoStuff = false;
		std::string options = "";
		while(true)
		{
			if(*format == '\0')
				break;
			if(*format == '%')
			{
				parsingOptions = !parsingOptions;
				shouldDoStuff = true;
				if(parsingOptions)
					options += '%';
			}
			else
			{
				if(!parsingOptions)
					stream.write(*format);
				else
				{
					//all of printf options. Will change eventually to a new method
					options += *format;
					if(*format == 'c' || *format == 'd' || *format == 's' || *format == 'i' || *format == 'o'
					|| *format == 'x' || *format == 'X' || *format == 'u' || *format == 'f' || *format == 'F'
					|| *format == 'e' || *format == 'E' || *format == 'a' || *format == 'A' || *format == 'g'
					|| *format == 'G' || *format == 'n' || *format == 'p')
						parsingOptions = false;
				}
			}

			if(parsingOptions == false && shouldDoStuff == true)
			{
				if(options.empty())
				{
					stream.write('%');
				}
				else
				{
					format++;
					formatToString(stream, v, options);
					formatStringInternal(stream, format, args...);
					break;
				}
				shouldDoStuff = false;
			}
			format++;
		}
	}

	template<typename T>
	void formatToString(StringStream& stream, const T& v, const std::string& options)
	{
		char tempBuffer[1024];
		std::string newOptions = options;
		sprintf(tempBuffer, options.c_str(), v);
		stream.write(tempBuffer);
	}
	
	void formatToString(StringStream& stream, const std::string& v, const std::string& options);
	
	template<typename T>
	void formatToString(StringStream& stream, const std::vector<T>& v, const std::string& options)
	{
		stream.write("{");
		for(const T& a : v)
		{
			formatToString(stream, a, options);
			stream.write(", ");
		}
		if(v.size() > 0)
		{
			stream.pop();
			stream.pop();
		}
		stream.write("}");
	}
}