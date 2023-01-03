#include "StringTools.h"
#include <string>
#include "Input.h"

#ifdef __unix__
	#include <unistd.h>
	#include <termios.h>
	#include <errno.h>
	
	//Note, works a bit better than the windows equivalent
	int _getwch()
	{
		int buf = 0;
		struct termios old = { 0 };
		fflush(stdout);
		if(tcgetattr(0, &old) < 0)
			perror("tcsetattr()");
		
		old.c_lflag &= ~ICANON; //local modes = Non Canonical Mode
		old.c_lflag &= ~ECHO; //local modes = Disable echo.
		old.c_cc[VMIN] = 1; //control chars (MIN value) = 1
		old.c_cc[VTIME] = 0; //control chars (TIME value) = 0 (No time)
		
		if(tcsetattr(0, TCSANOW, &old) < 0)
			perror("tcsetattr ICANON");
		
		if(read(0, &buf, 4) < 0)
			perror("read()");
		
		old.c_lflag |= ICANON; //local modes = Canonical mode
		old.c_lflag |= ECHO; //local modes = Enable echo
		
		if(tcsetattr(0, TCSADRAIN, &old) < 0)
			perror("tcsetattr ~ICANON");
		
		std::vector<unsigned char> bytes;
		bytes.push_back( buf & 0xFF );

		if(((buf>>8) & 0xFF) > 0)
		{
			bytes.push_back((buf>>8) & 0xFF);
			if(((buf>>16) & 0xFF) > 0)
			{
				bytes.push_back((buf>>16) & 0xFF);
				if(((buf>>24) & 0xFF) > 0)
				{
					bytes.push_back((buf>>24) & 0xFF);
				}
			}
		}
		
		return glib::StringTools::utf8ToChar(bytes);
	}
#else
	#include<io.h>
#endif

#define getch() (_getwch() % 0xFF)
#define getwch() _getwch()

namespace glib
{

	//Used in base64 conversions
	unsigned char base64Lookup[64] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                                'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

	unsigned char base64LookupURLSafe[64] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
									'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
									'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_'};


	bool StringTools::hasInit = false;
	wchar_t const StringTools::lineBreak = L'\n';

	std::wstreambuf* StringTools::inputBuffer = std::wcin.rdbuf();
	std::wstreambuf* StringTools::outputBuffer = std::wcout.rdbuf();
	std::wstreambuf* StringTools::errorBuffer = std::wcerr.rdbuf();

	void StringTools::init()
	{
		#ifdef __unix__
			setlocale(LC_CTYPE, "");
		#else
			//Note that the following return values are not used and cause warnings
			int outRet = _setmode(_fileno(stdout), _O_U16TEXT);
			int inRet = _setmode(_fileno(stdin), _O_U16TEXT);
			int errRet = _setmode(_fileno(stderr), _O_U16TEXT);
		#endif

		std::ios_base::sync_with_stdio(true);

		hasInit = true;
	}

	int StringTools::stringLength(char * text)
	{
		return strlen(text);
	}

	int StringTools::stringLength(const char * text)
	{
		return strlen(text);
	}

	int StringTools::stringLength(wchar_t * text)
	{
		return wcslen(text);
	}

	int StringTools::stringLength(const wchar_t * text)
	{
		return std::wcslen(text);
	}

	bool StringTools::isAlphaNumerial(char v, bool underScoreAllowed, bool dashAllowed)
	{
		if(v >= 48 && v <=57)
		{
			return true;
		}
		if(v >= 65 && v <=90)
		{
			return true;
		}
		if(v >= 97 && v <=122)
		{
			return true;
		}

		if(dashAllowed && v=='-')
		{
			return true;
		}

		if(underScoreAllowed && v=='_')
		{
			return true;
		}

		return false;
	}

	bool StringTools::isValidChar(int v)
	{
		return v>=32 && v<=126;
	}

	int StringTools::convertKeyToAscii(int keyVal, bool shiftHeld)
	{
		if(keyVal>='A' && keyVal<='Z')
		{
			if(shiftHeld)
				return keyVal;
			else
				return keyVal+0x20;
		}
		else if(keyVal>='0' && keyVal<='9')
		{
			if(shiftHeld)
			{
				char conversionTable[10] = {')', '!', '@', '#', '$', '%', '^', '&', '*', '('};
				return conversionTable[ keyVal-'0' ];
			}
			else
			{
				return keyVal;
			}
		}
		else if(keyVal>=Input::KEY_NUMPAD0 && keyVal<=Input::KEY_NUMPAD9)
		{
			return '0'+(keyVal-Input::KEY_NUMPAD0);
		}
		else if(keyVal==Input::KEY_PLUS)
		{
			if(shiftHeld)
				return '+';
			else
				return '=';
		}
		else if(keyVal==Input::KEY_COMMA)
		{
			if(shiftHeld)
				return '<';
			else
				return ',';
		}
		else if(keyVal==Input::KEY_PERIOD)
		{
			if(shiftHeld)
				return '>';
			else
				return '.';
		}
		else if(keyVal==Input::KEY_MINUS)
		{
			if(shiftHeld)
				return '_';
			else
				return '-';
		}
		else if(keyVal==Input::KEY_SPACE)
		{
			return ' ';
		}
		else if(keyVal==Input::KEY_FORWARD_SLASH)
		{
			if(shiftHeld)
				return '?';
			else
				return '/';
		}
		else if(keyVal==Input::KEY_TILDE)
		{
			if(shiftHeld)
				return '~';
			else
				return '`';
		}
		else if(keyVal==Input::KEY_SEMICOLON)
		{
			if(shiftHeld)
				return ':';
			else
				return ';';
		}
		else if(keyVal==Input::KEY_LEFT_SQUARE_BRACKET)
		{
			if(shiftHeld)
				return '{';
			else
				return '[';
		}
		else if(keyVal==Input::KEY_RIGHT_SQUARE_BRACKET)
		{
			if(shiftHeld)
				return '}';
			else
				return ']';
		}
		else if(keyVal==Input::KEY_BACK_SLASH)
		{
			if(shiftHeld)
				return '|';
			else
				return '\\';
		}
		else if(keyVal==Input::KEY_SINGLE_QUOTE)
		{
			if(shiftHeld)
				return '"';
			else
				return '\'';
		}
		else if(keyVal==Input::KEY_NUMPAD_ADD)
		{
			return '+';
		}
		else if(keyVal==Input::KEY_NUMPAD_SUBTRACT)
		{
			return '-';
		}
		else if(keyVal==Input::KEY_NUMPAD_DECIMAL)
		{
			return '.';
		}
		else if(keyVal==Input::KEY_NUMPAD_MULTIPLY)
		{
			return '*';
		}
		else if(keyVal==Input::KEY_NUMPAD_DIVIDE)
		{
			return '/';
		}
		else if(keyVal==Input::KEY_NUMPAD_DECIMAL)
		{
			return '.';
		}

		return -1;
	}
		

	char StringTools::base10ToBase16(char val)
	{
		if(val<10)
			return (char)(48+val);
		else
			return (char)(65+val-10);
	}

	int StringTools::base16ToBase10(char val)
	{
		if(val >= '0' && val <= '9')
		{
			return (int)(val-'0');
		}
		else if(val >= 'A' && val <='F')
		{
			return ((int)(val-'A')) + 10;
		}
		else if(val >= 'a' && val <= 'f')
		{
			return ((int)(val-'a')) + 10;
		}
		else
		{
			return -1;
		}
	}

	std::vector<unsigned char> StringTools::toUTF8(int c)
	{
		if(c <= 0x7F)
		{
			return {(unsigned char)c};
		}
		else if(c <= 0x7FF)
		{
			unsigned char c1 = 0b11000000;
			unsigned char c2 = 0b10000000;
			c1 += (c >> 6);
			c2 += c & 0b00111111;

			return {c1, c2};
		}
		else if(c <= 0xFFFF)
		{
			unsigned char c1 = 0b11100000;
			unsigned char c2 = 0b10000000;
			unsigned char c3 = 0b10000000;
			c1 += (c >> 12);
			c2 += (c >> 6) & 0b00111111;
			c3 += c & 0b00111111;
			return {c1, c2, c3};
		}
		else
		{
			unsigned char c1 = 0b11110000;
			unsigned char c2 = 0b10000000;
			unsigned char c3 = 0b10000000;
			unsigned char c4 = 0b10000000;

			c1 += (c >> 18);
			c2 += (c >> 12) & 0b00111111;
			c3 += (c >> 6) & 0b00111111;
			c4 += c & 0b00111111;
			return {c1, c2, c3, c4};
		}
	}

	short StringTools::byteSwap(short v)
	{
		#ifdef __unix__
			return StringTools::leftRotate(v, 8);
		#else
			return _byteswap_ushort(v);
		#endif
	}

	int StringTools::byteSwap(int v)
	{
		#ifdef __unix__
			return(
				((v & 0x000000FF) << 24) |
				((v & 0x0000FF00) <<  8) |
				((v & 0x00FF0000) >>  8) |
				((v & 0xFF000000) >> 24)
			);
		#else
			return _byteswap_ulong(v);
		#endif
	}

	size_t StringTools::byteSwap(size_t v)
	{
		#ifdef __unix__
			return(
				((v & 0x00000000000000FF) << 56) |
				((v & 0x000000000000FF00) << 48) |
				((v & 0x0000000000FF0000) << 40) |
				((v & 0x00000000FF000000) << 32) |
				((v & 0xFF00000000000000) >> 56) |
				((v & 0x00FF000000000000) >> 48) |
				((v & 0x0000FF0000000000) >> 40) |
				((v & 0x000000FF00000000) >> 32)
			);
		#else
			return _byteswap_uint64(v);
		#endif
	}

	int StringTools::utf8ToChar(std::vector<unsigned char> utf8Char)
	{
		// BinarySet b;
		// b.setBitOrder(BinarySet::RMSB);
		// b.setValues(utf8Char.data(), utf8Char.size());

		// BinarySet result;

		// int i = 0;

		// while(i<b.size())
		// {
		// 	if(!b.getBit(i))
		// 	{
		// 		i++;
		// 		int count = 8 - (i % 8);

		// 		for(int k=0; k<count; k++)
		// 		{
		// 			result.add( b.getBit(i) );
		// 			i++;
		// 		}
		// 	}
		// 	else
		// 	{
		// 		i++;
		// 	}
		// }

		// if(result.size()>0)
		// 	return result.getBits(0, result.size(), true);
		// else
		// 	return 0;

		int bytesToRead = 0;
		int runningCount = 0;
		for(unsigned char& c : utf8Char)
		{
			if(bytesToRead < 0)
			{
				if(c <= 127)
				{
					runningCount = c;
					break;
				}
				else
				{
					//utf8
					if(c >= 0b11110000)
					{
						bytesToRead = 3;
						runningCount += c & 0b111;
					}
					else if(c >= 0b11100000)
					{
						bytesToRead = 2;
						runningCount += c & 0b1111;
					}
					else
					{
						bytesToRead = 1;
						runningCount += c & 0b11111;
					}
				}
			}
			else
			{
				runningCount <<= 6;
				runningCount += c & 0b111111;
				if(bytesToRead == 0)
				{
					break;
				}
			}
		}

		return runningCount;
	}

	std::vector<int> StringTools::utf8ToIntString(std::string validUTF8String)
	{
		std::vector<int> finalChars;
		int bytesToRead = 0;
		int runningCount = 0;
		for(char& c : validUTF8String)
		{
			unsigned char v = (unsigned char)c;
			if(bytesToRead <= 0)
			{
				if(v <= 127)
					finalChars.push_back(v);
				else
				{
					//utf8
					if((v >> 3) == 0b11110)
					{
						bytesToRead = 3;
						runningCount += v & 0b111;
					}
					else if((v >> 4) == 0b1110)
					{
						bytesToRead = 2;
						runningCount += v & 0b1111;
					}
					else  if((v >> 5) == 0b110)
					{
						bytesToRead = 1;
						runningCount += v & 0b11111;
					}
					else
					{
						//error probably
						break;
					}
				}
			}
			else
			{
				runningCount <<= 6;
				runningCount += v & 0b111111;
				bytesToRead--;
				if(bytesToRead == 0)
				{
					finalChars.push_back(runningCount);
					runningCount = 0;
				}
			}
		}

		return finalChars;
	}
	
	std::vector<int> StringTools::wideStringToIntString(std::wstring str)
	{
		std::vector<int> results = std::vector<int>(str.size());
		for(size_t i=0; i<str.size(); i++)
		{
			results[i] = str[i];
		}
		return results;
	}

	int StringTools::base64CharToNum(unsigned char base64Char)
	{
		if(base64Char >= 'A' && base64Char <= 'Z')
			return base64Char - 'A';
		else if(base64Char >= 'a' && base64Char <= 'z')
			return (base64Char - 'a') + 26;
		else if(base64Char >= '0' && base64Char <= '9')
			return (base64Char - '0') + 52;
		else if(base64Char == '+' || base64Char == '-')
			return 62;
		else if(base64Char == '/' || base64Char == '_')
			return 63;
			
		return -1;
	}
	std::string StringTools::base64Encode(std::vector<unsigned char> bytes, bool urlSafe)
	{
		return base64Encode(bytes.data(), bytes.size(), urlSafe);
	}
	std::string StringTools::base64Encode(unsigned char* bytes, size_t size, bool urlSafe)
	{
		std::string result;
		
		if(size <= 0)
			return "";
		
		size_t adjustedSize = size - (size%3);
		size_t remainder = size - adjustedSize;
		
		size_t index = 0;
		while(index < adjustedSize)
		{
			unsigned char c1,c2,c3,c4;
			c1 = (bytes[index] & 0b11111100) >> 2;
			c2 = ((bytes[index] & 0b00000011) << 4) + ((bytes[index+1] & 0b11110000) >> 4);
			c3 = ((bytes[index+1] & 0b00001111) << 2) + ((bytes[index+2] & 0b11000000) >> 6);
			c4 = ((bytes[index+2] & 0b00111111));
			
			if(urlSafe)
			{
				result += base64LookupURLSafe[c1];
				result += base64LookupURLSafe[c2];
				result += base64LookupURLSafe[c3];
				result += base64LookupURLSafe[c4];
			}
			else
			{
				result += base64Lookup[c1];
				result += base64Lookup[c2];
				result += base64Lookup[c3];
				result += base64Lookup[c4];
			}
			index += 3;
		}

		if(remainder == 1)
		{
			unsigned char c1, c2;
			c1 = (bytes[adjustedSize] & 0b11111100) >> 2;
			c2 = ((bytes[adjustedSize] & 0b00000011) << 4);
			
			if(urlSafe)
			{
				result += base64LookupURLSafe[c1];
				result += base64LookupURLSafe[c2];
			}
			else
			{
				result += base64Lookup[c1];
				result += base64Lookup[c2];
			}
			result += "=="; //padding
		}
		else if(remainder == 2)
		{
			unsigned char c1, c2, c3;
			c1 = (bytes[adjustedSize] & 0b11111100) >> 2;
			c2 = ((bytes[adjustedSize] & 0b00000011) << 4) + ((bytes[adjustedSize+1] & 0b11110000) >> 4);
			c3 = ((bytes[adjustedSize+1] & 0b00001111) << 2);
			
			if(urlSafe)
			{
				result += base64LookupURLSafe[c1];
				result += base64LookupURLSafe[c2];
				result += base64LookupURLSafe[c3];
			}
			else
			{
				result += base64Lookup[c1];
				result += base64Lookup[c2];
				result += base64Lookup[c3];
			}
			result += "="; //padding
		}

		return result;
	}

	std::vector<unsigned char> StringTools::base64Decode(std::vector<unsigned char> bytes)
	{
		return base64Decode(bytes.data(), bytes.size());
	}
	std::vector<unsigned char> StringTools::base64Decode(unsigned char* bytes, size_t size)
	{
		std::vector<unsigned char> result;
		if(size == 0)
			return {};
		
		unsigned int temp = 0;
		int bitsAvaliable = 0;
		for(size_t i=0; i<size; i++)
		{
			if(bytes[i] != '=')
			{
				//Line breaks can sometimes exist in the base64 data even though the standard does not specify it.
				//Other base64 decoders fail due to this.
				//Also, the line breaks are written in plain text instead of the byte for a line break
				if(bytes[i] == '\\')
				{
					if(i < size-1)
					{
						if(bytes[i+1] == 'n')
						{
							continue;
						}
					}
					else
					{
						// StringTools::println("ERROR AT %x for %x", i, bytes[i]);
						return {}; //Error
					}
				}
				else if(bytes[i] == 'n')
				{
					if(i>0)
					{
						if(bytes[i-1] == '\\')
						{
							continue;
						}
					}
				}

				int charToNum = base64CharToNum(bytes[i]);
				if(charToNum < 0)
				{
					// StringTools::println("ERROR AT %x for %x", i, bytes[i]);
					return {}; //error
				}
				
				temp = temp << 6;
				temp += charToNum;
				bitsAvaliable += 6;

				if(bitsAvaliable >= 8)
				{
					unsigned char nByte = temp >> (bitsAvaliable-8);
					result.push_back(nByte);

					unsigned int andValue = (1<<(bitsAvaliable-8))-1;
					temp = (temp&andValue);
					bitsAvaliable-=8;
				}
			}
			else
			{
				//If there are bits available, use them to create bytes
				if(bitsAvaliable > 0)
				{
					result.push_back(temp);
					temp = 0;
				}

				bitsAvaliable = 0;
			}
		}

		//If there are bits available, use them to create bytes
		if(bitsAvaliable > 0)
		{
			result.push_back(temp);
			temp = 0;
		}

		bitsAvaliable = 0;

		return result;
	}

	std::vector<std::string> StringTools::splitString(std::string s, const char delim, bool removeEmpty)
	{
		std::vector<std::string> stringArray = std::vector<std::string>();

		std::string temp = "";

		size_t i = 0;
		while (i < s.size())
		{
			if (s.at(i) != delim)
			{
				temp += s.at(i);
			}
			else
			{
				if(removeEmpty)
				{
					if(temp!="")
						stringArray.push_back(temp);
				}
				else
				{
					stringArray.push_back(temp);
				}
				
				temp = "";
			}
			i++;
		}

		stringArray.push_back(temp);

		return stringArray;
	}

	std::vector<std::string> StringTools::splitStringMultipleDeliminators(std::string s, std::string delim, bool removeEmpty)
	{
		std::vector<std::string> stringArray = std::vector<std::string>();

		std::string temp = "";

		size_t i = 0;
		size_t dSize = delim.size();

		while (i < s.size())
		{
			bool valid = true;

			for(size_t x=0; x<dSize; x++)
			{
				if (s.at(i) == delim[x])
				{
					valid = false;
				}
			}

			if (valid)
			{
				temp += s.at(i);
			}
			else
			{
				if(removeEmpty)
				{
					if(temp!="")
						stringArray.push_back(temp);
				}
				else
				{
					stringArray.push_back(temp);
				}
				
				temp = "";
			}
			
			i++;
		}

		stringArray.push_back(temp);

		return stringArray;
	}

	std::vector<std::string> StringTools::splitString(std::string s, std::string delim, bool removeEmpty)
	{
		std::vector<std::string> stringArray = std::vector<std::string>();

		std::string temp = "";
		std::string otherString = "";

		size_t dSize = delim.size();

		size_t i = 0;
		size_t count = 0;

		while (i < s.size())
		{
			if (s.at(i) != delim[0])
			{
				temp += s.at(i);
				i++;
			}
			else
			{
				if (dSize + i > s.size())
				{
					//can't contain the substring.
					temp+=s.at(i);
					i++;
					continue;
				}

				while (count < dSize)
				{
					if (s.at(i + count) == delim[count])
					{
						otherString += delim[count];
					}
					else
					{
						break;
					}
					count++;
				}

				if (count == dSize)
				{
					//Found subString
					if(removeEmpty)
					{
						if(temp!="")
							stringArray.push_back(temp);
					}
					else
					{
						stringArray.push_back(temp);
					}
					
					temp = "";
					i += count;

					count = 0;
					otherString = "";
				}
				else
				{
					temp += otherString;
					i += count;
					count = 0;
					otherString = "";
				}

			}
		}

		stringArray.push_back(temp);

		return stringArray;
	}


	std::vector<std::wstring> StringTools::splitString(std::wstring s, const wchar_t delim, bool removeEmpty)
	{
		std::vector<std::wstring> stringArray = std::vector<std::wstring>();

		std::wstring temp = L"";

		size_t i = 0;
		while (i < s.size())
		{
			if (s.at(i) != delim)
			{
				temp += s.at(i);
			}
			else
			{
				if(removeEmpty)
				{
					if(temp!=L"")
						stringArray.push_back(temp);
				}
				else
				{
					stringArray.push_back(temp);
				}
				
				temp = L"";
			}
			i++;
		}

		stringArray.push_back(temp);

		return stringArray;
	}

	std::vector<std::wstring> StringTools::splitStringMultipleDeliminators(std::wstring s, std::wstring delim, bool removeEmpty)
	{
		std::vector<std::wstring> stringArray = std::vector<std::wstring>();

		std::wstring temp = L"";

		size_t i = 0;
		size_t dSize = delim.size();

		while (i < s.size())
		{
			bool valid = true;

			for(size_t x=0; x<dSize; x++)
			{
				if (s.at(i) == delim[x])
				{
					valid = false;
				}
			}

			if (valid)
			{
				temp += s.at(i);
			}
			else
			{
				if(removeEmpty)
				{
					if(temp!=L"")
						stringArray.push_back(temp);
				}
				else
				{
					stringArray.push_back(temp);
				}
				
				temp = L"";
			}
			
			i++;
		}

		stringArray.push_back(temp);

		return stringArray;
	}

	std::vector<std::wstring> StringTools::splitString(std::wstring s, std::wstring delim, bool removeEmpty)
	{
		std::vector<std::wstring> stringArray = std::vector<std::wstring>();

		std::wstring temp = L"";
		std::wstring otherString = L"";

		size_t dSize = delim.size();

		size_t i = 0;
		size_t count = 0;

		while (i < s.size())
		{
			if (s.at(i) != delim[0])
			{
				temp += s.at(i);
				i++;
			}
			else
			{
				if (dSize + i > s.size())
				{
					//can't contain the substring.
					temp+=s.at(i);
					i++;
					continue;
				}

				while (count < dSize)
				{
					if (s.at(i + count) == delim[count])
					{
						otherString += delim[count];
					}
					else
					{
						break;
					}
					count++;
				}

				if (count == dSize)
				{
					//Found subString
					if(removeEmpty)
					{
						if(temp!=L"")
							stringArray.push_back(temp);
					}
					else
					{
						stringArray.push_back(temp);
					}
					
					temp = L"";
					i += count;

					count = 0;
					otherString = L"";
				}
				else
				{
					temp += otherString;
					i += count;
					count = 0;
					otherString = L"";
				}

			}
		}

		stringArray.push_back(temp);

		return stringArray;
	}

	std::string StringTools::removeWhitespace(std::string originalStr, bool removeTabs, bool onlyLeadingAndTrailing)
	{
		std::string nStr;
		if(!onlyLeadingAndTrailing)
		{
			for(char& c : originalStr)
			{
				if(c == ' ')
					continue;

				if(removeTabs)
				{
					if(c == '\t')
						continue;
				}

				nStr += c;
			}
		}
		else
		{
			int firstInvalidSpot = -1;
			bool hitNormalChar = false;
			for(size_t i=0; i<originalStr.size(); i++)
			{
				char c = originalStr[i];
				if(!hitNormalChar)
				{
					if(c == ' ')
						continue;

					if(removeTabs)
					{
						if(c == '\t')
							continue;
					}
				}
				else
				{
					if(c == ' ')
					{
						if(firstInvalidSpot < 0)
							firstInvalidSpot = i;
						continue;
					}

					if(removeTabs)
					{
						if(c == '\t')
						{
							if(firstInvalidSpot < 0)
								firstInvalidSpot = i;
							continue;
						}
					}
				}

				//Add data that was skipped since it is not empty white space at the end.
				if(firstInvalidSpot >= 0)
				{
					for(size_t k=firstInvalidSpot; k<i; k++)
					{
						nStr += originalStr[k];
					}
				}
				hitNormalChar = true;
				firstInvalidSpot = -1;

				nStr += c;
			}
		}
		return nStr;
	}

	std::wstring StringTools::removeWhitespace(std::wstring originalStr, bool removeTabs, bool onlyLeadingAndTrailing)
	{
		std::wstring nStr;
		if(!onlyLeadingAndTrailing)
		{
			for(wchar_t& c : originalStr)
			{
				if(c == L' ')
					continue;

				if(removeTabs)
				{
					if(c == L'\t')
						continue;
				}

				nStr += c;
			}
		}
		else
		{
			int firstInvalidSpot = -1;
			bool hitNormalChar = false;
			for(size_t i=0; i<originalStr.size(); i++)
			{
				wchar_t c = originalStr[i];
				if(!hitNormalChar)
				{
					if(c == ' ')
						continue;

					if(removeTabs)
					{
						if(c == L'\t')
							continue;
					}
				}
				else
				{
					if(c == L' ')
					{
						if(firstInvalidSpot < 0)
							firstInvalidSpot = i;
						continue;
					}

					if(removeTabs)
					{
						if(c == L'\t')
						{
							if(firstInvalidSpot < 0)
								firstInvalidSpot = i;
							continue;
						}
					}
				}

				//Add data that was skipped since it is not empty white space at the end.
				if(firstInvalidSpot >= 0)
				{
					for(size_t k=firstInvalidSpot; k<i; k++)
					{
						nStr += originalStr[k];
					}
				}
				hitNormalChar = true;
				firstInvalidSpot = -1;

				nStr += c;
			}
		}
		return nStr;
	}

	int StringTools::toInt(std::string s)
	{
		return std::stoi(s.c_str());
	}

	long StringTools::toLong(std::string s)
	{
		return std::stol(s.c_str());
	}

	double StringTools::toDouble(std::string s)
	{
		return std::stod(s.c_str());
	}

	float StringTools::toFloat(std::string s)
	{
		return std::stof(s.c_str());
	}

	int StringTools::toInt(std::wstring s)
	{
		return std::stoi(s.c_str());
	}

	long StringTools::toLong(std::wstring s)
	{
		return std::stol(s.c_str());
	}

	double StringTools::toDouble(std::wstring s)
	{
		return std::stod(s.c_str());
	}

	float StringTools::toFloat(std::wstring s)
	{
		return std::stof(s.c_str());
	}

	std::string StringTools::toString(int k)
	{
		return std::to_string(k);
	}
	std::string StringTools::toString(long k)
	{
		return std::to_string(k);
	}
	std::string StringTools::toString(unsigned int k)
	{
		return std::to_string(k);
	}
	std::string StringTools::toString(unsigned long k)
	{
		return std::to_string(k);
	}
	std::string StringTools::toString(long long k)
	{
		return std::to_string(k);
	}
	std::string StringTools::toString(unsigned long long k)
	{
		return std::to_string(k);
	}
	std::string StringTools::toString(float k)
	{
		return std::to_string(k);
	}
	std::string StringTools::toString(double k)
	{
		return std::to_string(k);
	}

	std::wstring StringTools::getWideString()
	{
		if (!hasInit)
		{
			init();
		}

		std::wstring temp = L"";
		std::getline(std::wcin, temp);
		return temp;
	}

	std::string StringTools::getString()
	{
		std::wstring temp = L"";
		std::getline(std::wcin, temp);
		std::string text = StringTools::toCString(temp);

		return text;
	}

	std::string StringTools::getHiddenString(bool showAsterisk)
	{
		std::string text = "";
		while(true)
		{
			int c = getch();
			if(c == 0x0D || c == 0x0A)
			{
				if(showAsterisk)
					std::wcout << "\n";
				break;
			}
			else if(c == 0x08)
			{
				if(showAsterisk)
				{
					//move cursor back one
					std::wcout << "\x1B[D";
					//clear to end of line
					std::wcout << "\x1B[K";
				}

				if(text.size()>0)
					text.pop_back();
			}
			else if(c >= 32)
			{
				if(showAsterisk)
					std::wcout << "*";
				text += c;
			}
		}

		return text;

	}

	std::wstring StringTools::getHiddenWideString(bool showAsterisk)
	{
		std::wstring text = L"";
		while(true)
		{
			int c = getwch();
			if(c == 0x0D || c == 0x0A)
			{
				if(showAsterisk)
					std::wcout << "\n";
				break;
			}
			else if(c == 0x08)
			{
				if(showAsterisk)
				{
					//move cursor back one
					std::wcout << "\x1B[D";
					//clear to end of line
					std::wcout << "\x1B[K";
				}

				if(text.size()>0)
					text.pop_back();
			}
			else if(c >= 32)
			{
				if(showAsterisk)
					std::wcout << "*";
				text += c;
			}
		}

		return text;
	}

	char StringTools::getChar()
	{
		std::string temp = getString();
		if(temp.empty())
			return 0;
		else
			return temp[0];
	}

	wchar_t StringTools::getWideChar()
	{
		if(!hasInit)
			init();
		
		std::wstring temp = getWideString();
		if(temp.empty())
			return 0;
		else
			return temp[0];
	}

	int StringTools::getInt()
	{
		std::wstring temp;
		std::getline(std::wcin, temp);
		return toInt(temp);
	}

	float StringTools::getFloat()
	{
		std::wstring temp;
		std::getline(std::wcin, temp);
		return toFloat(temp);
	}

	char StringTools::getCharLessLock(bool noFunctionKeys)
	{
		if(noFunctionKeys)
		{
			while(true)
			{
				int c = getch();

				if( c == 0xE0 || c == 0x00 )
				{
					if(c == 0xE0)
					{
						//Could be a valid character.
						//Is a valid character if no character appears after
						//however, there is no way to determine if there is another character
						//without blocking with another getwch call
					}
					getch();
				}
				else if(c >= 0x20)
				{
					return c;
				}
			}
		}
		else
		{
			return getch();
		}
	}

	int StringTools::getWideCharLessLock(bool noFunctionKeys)
	{
		if(noFunctionKeys)
		{
			while(true)
			{
				int c = getwch();
				if( c == 0xE0 || c == 0x00 )
				{
					if(c == 0xE0)
					{
						//Could be a valid character.
						//Is a valid character if no character appears after
						//however, there is no way to determine if there is another character
						//without blocking with another getwch call
					}
					c = getwch();
				}
				else if(c >= 0x20)
				{
					return c;
				}
			}
		}
		else
		{
			return getwch();
		}
	}

	void StringTools::findLongestMatch(std::string base, std::string match, int* index, int* length)
	{
		StringTools::KMP(base.c_str(), base.size(), match.c_str(), match.size(), index, length);
	}

	std::string StringTools::formatStringInternal(std::string text, va_list orgArgs)
	{
		std::string finalText = "";
		std::vector<std::string> splits = splitString(text, "%ls", false);

		va_list args;
		va_copy(args, orgArgs);

		size_t i=0;

		while(i<splits.size())
		{
			std::string str = splits[i];
			
			int bufferSize = 1024;
			char* nText = new char[bufferSize];
			while(true)
			{
				int size = vsnprintf(nText, bufferSize, str.c_str(), args);
				if(size < 0)
				{
					bufferSize*=2;
					delete[] nText;
					nText = new char[bufferSize];
				}
				else
				{
					break;
				}
			}
			
			finalText += nText;
			delete[] nText;
			
			int count = 0;
			size_t loc = 0;
			
			while(true)
			{
				size_t nLoc = str.find('%', loc);
				if(nLoc != SIZE_MAX)
				{
					loc = nLoc;
					
					while(loc < str.size())
					{
						loc++;
						//read till flag
						if(str[loc] == 'd' || str[loc] == 'i' || str[loc] == 'u' || str[loc] == 'o'
						|| str[loc] == 'x' || str[loc] == 'X' || str[loc] == 'c')
						{
							va_arg(args, size_t);
							count++;
							break;
						}
						else if(str[loc] == 'f' || str[loc] == 'F' || str[loc] == 'e' || str[loc] == 'E'
						|| str[loc] == 'g' || str[loc] == 'G' || str[loc] == 'a' || str[loc] == 'A')
						{
							va_arg(args, long double);
							count++;
							break;
						}
						else if(str[loc] == 's')
						{
							//should always be char*
							va_arg(args, char*);
							count++;
							break;
						}
						else if(str[loc] == 'p')
						{
							va_arg(args, void*);
							count++;
							break;
						}
						else if(str[loc] == 'n')
						{
							va_arg(args, void*);
							count++;
							break;
						}
						else if(str[loc] == L'%')
						{
							break;
						}
						else if(str[loc] == '*')
						{
							va_arg(args, int);
							count++;
						}
					}
				}
				else
				{
					break;
				}
			}

			if(i < splits.size()-1)
			{
				std::wstring delayedStr = va_arg(args, wchar_t*);
				finalText += StringTools::toCString(delayedStr);
				count++;
			}

			i++;
		}

		va_end(args);

		return finalText;
	}

	std::string StringTools::formatString(std::string text, ...)
	{
		std::string finalText = "";

		va_list args;
		va_start(args, text);

		finalText = StringTools::formatStringInternal(text, args);

		va_end(args);

		return finalText;
	}

	std::wstring StringTools::formatWideStringInternal(std::wstring text, va_list orgArgs)
	{
		std::wstring finalText = L"";
		std::vector<std::wstring> splits = splitString(text, L"%s", false);

		va_list args;
		va_copy(args, orgArgs);

		size_t i = 0;
		while(i<splits.size())
		{
			std::wstring str = splits[i];
			
			int bufferSize = 1024;
			wchar_t* nText = new wchar_t[bufferSize];
			while(true)
			{
				int size = vswprintf(nText, bufferSize, str.c_str(), args);
				if(size < 0)
				{
					bufferSize*=2;
					delete[] nText;
					nText = new wchar_t[bufferSize];
				}
				else
				{
					break;
				}
			}
			
			finalText += nText;
			delete[] nText;
			
			int count = 0;
			size_t loc = 0;
			
			while(true)
			{
				size_t nLoc = str.find(L'%', loc);
				if(nLoc != SIZE_MAX)
				{
					loc = nLoc;
					
					while(loc < str.size())
					{
						loc++;
						//read till flag
						if(str[loc] == L'd' || str[loc] == L'i' || str[loc] == L'u' || str[loc] == L'o'
						|| str[loc] == L'x' || str[loc] == L'X' || str[loc] == L'c')
						{
							va_arg(args, size_t);
							count++;
							break;
						}
						else if(str[loc] == L'f' || str[loc] == L'F' || str[loc] == L'e' || str[loc] == L'E'
						|| str[loc] == L'g' || str[loc] == L'G' || str[loc] == L'a' || str[loc] == L'A')
						{
							va_arg(args, long double);
							count++;
							break;
						}
						else if(str[loc] == L's')
						{
							//should always be wchar_t*
							va_arg(args, wchar_t*);
							count++;
							break;
						}
						else if(str[loc] == L'p')
						{
							va_arg(args, void*);
							count++;
							break;
						}
						else if(str[loc] == L'n')
						{
							va_arg(args, void*);
							count++;
							break;
						}
						else if(str[loc] == L'%')
						{
							break;
						}
						else if(str[loc] == L'*')
						{
							va_arg(args, int);
							count++;
						}
					}
				}
				else
				{
					break;
				}
			}

			if(i < splits.size()-1)
			{
				std::string delayedStr = va_arg(args, char*);
				finalText += StringTools::toWideString(delayedStr);
				count++;
			}

			i++;
		}

		va_end(args);

		return finalText;
	}

	std::wstring StringTools::formatWideString(std::wstring text, ...)
	{
		std::wstring finalText = L"";

		va_list args;
		va_start(args, text);

		finalText = StringTools::formatWideStringInternal(text, args);

		va_end(args);

		return finalText;
	}

	void StringTools::clearConsole(bool clearScrollBuffer)
	{
		if(clearScrollBuffer)
			StringTools::print("\x1B[2J\x1B[3J\x1B[H");
		else
			StringTools::print("\x1B[2J\x1B[H");
	}

	void StringTools::moveConsoleCursor(int horizontal, int vertical, bool absolute)
	{
		if(absolute)
		{
			int realHVal = (horizontal>=1) ? horizontal : 1;
			int realVVal = (vertical>=1) ? vertical : 1;
			
			StringTools::print("\x1B[%d;%dH", realVVal, realHVal);
		}
		else
		{
			if(horizontal>0)
				StringTools::print("\x1B[%dC", horizontal);
			else if(horizontal<0)
				StringTools::print("\x1B[%dD", std::abs(horizontal));

			if(vertical>0)
				StringTools::print("\x1B[%dB", vertical);
			else if(vertical<0)
				StringTools::print("\x1B[%dA", std::abs(vertical));
		}
	}

	void StringTools::eraseConsoleLine(bool eraseFromCursor)
	{
		if(eraseFromCursor)
			StringTools::print("\x1B[K");
		else
			StringTools::print("\x1B[2K\r");
	}

	void StringTools::reroutOutput(std::wstreambuf* file)
	{
		std::wcout.rdbuf(file);
	}

	void StringTools::reroutInput(std::wstreambuf* file)
	{
		std::wcin.rdbuf(file);
	}

	void StringTools::reroutErrorOutput(std::wstreambuf* file)
	{
		std::wcerr.rdbuf(file);
	}

	void StringTools::resetOutputInputStreams()
	{
		std::wcin.rdbuf(inputBuffer);
		std::wcout.rdbuf(outputBuffer);
		std::wcerr.rdbuf(errorBuffer);
	}

} //NAMESPACE glib END