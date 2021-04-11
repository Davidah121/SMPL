#include "StringTools.h"
#include <string>

bool StringTools::hasInit = false;
wchar_t const StringTools::lineBreak = L'\n';

StringTools::StringTools()
{

}

StringTools::~StringTools()
{
}

void StringTools::init()
{
	int outRet = _setmode(_fileno(stdout), _O_U16TEXT);
	int inRet = _setmode(_fileno(stdin), _O_U16TEXT);
	int errRet = _setmode(_fileno(stderr), _O_U16TEXT);

	std::ios_base::sync_with_stdio(true);

	hasInit = true;
}

std::wstring StringTools::toWideString(std::string text)
{
	std::wstring finalText = L"";
	for (int i = 0; i < text.size(); i++)
	{
		finalText += (wchar_t)text[i];
	}

	return finalText;
}

std::string StringTools::toCString(std::wstring text)
{
	std::string finalText = "";
	for (int i = 0; i < text.size(); i++)
	{
		finalText += (char)(text[i] & 0xFF);
	}

	return finalText;
}

int StringTools::stringLength(char * text)
{
	return std::strlen(text);
}

int StringTools::stringLength(const char * text)
{
	return std::strlen(text);
}

int StringTools::stringLength(wchar_t * text)
{
	return std::wcslen(text);
}

int StringTools::stringLength(const wchar_t * text)
{
	return std::wcslen(text);
}

bool StringTools::equalsIgnoreCase(std::string a, std::string b)
{
	if(a.size() == b.size())
	{
		for(int i=0; i<a.size(); i++)
		{
			if( toupper(a[i]) != toupper(b[i]))
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	
	return true;
}

bool StringTools::equalsIgnoreCase(std::wstring a, std::wstring b)
{
	if(a.size() == b.size())
	{
		for(int i=0; i<a.size(); i++)
		{
			if( toupper(a[i]) != toupper(b[i]))
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	
	return true;
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

char StringTools::charToBase16(char val)
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

int StringTools::utf8ToChar(std::vector<unsigned char> utf8Char)
{
	BinarySet b;
	b.setBitOrder(BinarySet::RMSB);
	b.setValues(utf8Char.data(), utf8Char.size());

	BinarySet result;

	int i = 0;

	while(i<b.size())
	{
		if(!b.getBit(i))
		{
			i++;
			int count = 8 - (i % 8);

			for(int k=0; k<count; k++)
			{
				result.add( b.getBit(i) );
				i++;
			}
		}
		else
		{
			i++;
		}
	}

	if(result.size()>0)
		return result.getBits(0, result.size(), true);
	else
		return 0;
}

std::vector<std::string> StringTools::splitString(std::string s, const char delim, bool removeEmpty)
{
	std::vector<std::string> stringArray = std::vector<std::string>();

	std::string temp = "";

	int i = 0;
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

	int i = 0;
	int dSize = delim.size();

	while (i < s.size())
	{
		bool valid = true;

		for(int x=0; x<dSize; x++)
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

	int dSize = delim.size();

	int i = 0;
	int count = 0;

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

	int i = 0;
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

	int i = 0;
	int dSize = delim.size();

	while (i < s.size())
	{
		bool valid = true;

		for(int x=0; x<dSize; x++)
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

	int dSize = delim.size();

	int i = 0;
	int count = 0;

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

std::wstring StringTools::getWideString()
{
	if (hasInit)
	{
		std::wstring temp = L"";

		std::getline(std::wcin, temp);

		return temp;
	}
	return L"";
}

std::string StringTools::getString()
{
	if (hasInit)
	{
		std::wstring temp = L"";

		std::getline(std::wcin, temp);

		std::string text = StringTools::toCString(temp);

		return text;
	}
	return "";
}

char StringTools::getChar()
{
	if (hasInit)
	{
		std::wstring temp = L"";
		std::getline(std::wcin, temp);

		return (char)temp.at(0);
	}
	return 0;
}

wchar_t StringTools::getWideChar()
{
	if (hasInit)
	{
		std::wstring temp = L"";
		std::getline(std::wcin, temp);

		return temp.at(0);
	}
	return 0;
}

int StringTools::getInt()
{
	if(hasInit)
		return std::wcin.get();

	return 0;
}

void StringTools::findLongestMatch(std::string base, std::string match, int* length, int* index)
{
	findLongestMatch((char*)base.c_str(), base.size(), (char*)match.c_str(), match.size(), length, index);
}

void StringTools::findLongestMatch(char* base, int sizeOfBase, char* match, int sizeOfMatch, int* length, int* index)
{
	if(length!=nullptr && index!=nullptr)
	{
		int maxVal = 0;
		int indexOfMax = 0;

		int x = 0;
		int y = 0;

		int currSize = 0;
		int currStartIndex = -1;

		int nextPossibleIndex = -1;

		char* sB = base;
		char* sM = match;

		char startValue = match[0];
		
		while(x < sizeOfBase)
		{
			if(*sB == *sM)
			{
				if(currStartIndex!=-1)
				{
					if(*sB == startValue)
					{
						nextPossibleIndex = x; 
					}
				}

				if(currStartIndex==-1)
					currStartIndex = x;
				
				currSize++;
				sM++;

				if(currSize >= sizeOfMatch)
				{
					maxVal = currSize;
					indexOfMax = currStartIndex;
					break;
				}
			}
			else
			{
				if(currSize >= maxVal)
				{
					maxVal = currSize;
					indexOfMax = currStartIndex;
				}

				if(nextPossibleIndex>0)
				{
					x = nextPossibleIndex;
					sB = base + nextPossibleIndex;
				}

				currSize = 0;
				currStartIndex = -1;
				nextPossibleIndex = -1;

				sM = match;
			}

			sB++;
			x++;
		}
		
		if(currSize >= maxVal)
		{
			maxVal = currSize;
			indexOfMax = currStartIndex;
		}
		
		*length = maxVal;
		*index = indexOfMax;
	}
}

std::string StringTools::formatStringInternal(std::string text, va_list orgArgs)
{
	std::string finalText = "";
	std::vector<std::string> splits = splitString(text, "%ls", false);

	va_list args;
	va_copy(args, orgArgs);

	for(int i=0; i<splits.size(); i++)
	{
		std::string str = splits[i];

		int size = vsnprintf(nullptr, 0, str.c_str(), args);
		size++;

		char* nText = new char[size];
		
		vsnprintf(nText, size, str.c_str(), args);
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
				
				while(true)
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

	for(int i=0; i<splits.size(); i++)
	{
		std::wstring str = splits[i];

		int size = vswprintf(nullptr, 0, str.c_str(), args);
		size++;

		wchar_t* nText = new wchar_t[size];
		
		vswprintf(nText, size, str.c_str(), args);
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
				
				while(true)
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