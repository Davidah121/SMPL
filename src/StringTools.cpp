#include "StringTools.h"
#include<string>

/*
std::wostream StringTools::out(std::wcout.rdbuf);
std::wistream StringTools::in(std::wcin.rdbuf);
std::wostream StringTools::err(std::wcerr.rdbuf);
*/

// std::wostream StringTools::out(nullptr);
// std::wistream StringTools::in(nullptr);
// std::wostream StringTools::err(nullptr);

bool StringTools::hasInit = false;

wchar_t const StringTools::lineBreak = '\n';


StringTools::StringTools()
{

}

StringTools::~StringTools()
{
}

void StringTools::init()
{
	// StringTools::out.rdbuf(std::wcout.rdbuf());
	// StringTools::in.rdbuf(std::wcin.rdbuf());
	// StringTools::err.rdbuf(std::wcerr.rdbuf());

	std::ios_base::sync_with_stdio(true);
	// int outRet = _setmode(_fileno(stdout), _O_U16TEXT);
	// int inRet = _setmode(_fileno(stdin), _O_U16TEXT);
	// int errRet = _setmode(_fileno(stderr), _O_U16TEXT);

	hasInit = true;
}

wchar_t * StringTools::toWideString(char * text)
{
	int len = stringLength(text);

	wchar_t* temp = new wchar_t[len];
	for (int i = 0; i < len; i++)
	{
		temp[i] = (wchar_t)text[i];
	}
	return temp;
}

wchar_t * StringTools::toWideString(const char * text)
{
	int len = stringLength(text);

	wchar_t* temp = new wchar_t[len];
	for (int i = 0; i < len; i++)
	{
		temp[i] = (wchar_t)text[i];
	}
	return temp;
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

char * StringTools::toCString(wchar_t * text)
{
	int len = stringLength(text);
	char* temp = new char[len];
	for (int i = 0; i < len; i++)
	{
		temp[i] = (char)(text[i] & 0xFF);
	}
	return temp;
}

char * StringTools::toCString(const wchar_t * text)
{
	int len = stringLength(text);
	char* temp = new char[len];
	for (int i = 0; i < len; i++)
	{
		temp[i] = (char)(text[i] & 0xFF);
	}
	return temp;
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

std::vector<std::string> StringTools::splitStringMultipleDeliminators(std::string s, const char* delim, bool removeEmpty)
{
	std::vector<std::string> stringArray = std::vector<std::string>();

	std::string temp = "";

	int i = 0;
	int dSize = std::strlen(delim);

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

std::vector<std::string> StringTools::splitString(std::string s, const char* delim, bool removeEmpty)
{
	std::vector<std::string> stringArray = std::vector<std::string>();

	std::string temp = "";
	std::string otherString = "";

	int dSize = std::strlen(delim);

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
			if (dSize + i <= s.size())
			{
				//possible that it could still contain
				//the substring

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

std::vector<std::wstring> StringTools::splitStringMultipleDeliminators(std::wstring s, const wchar_t* delim, bool removeEmpty)
{
	std::vector<std::wstring> stringArray = std::vector<std::wstring>();

	std::wstring temp = L"";

	int i = 0;
	int dSize = std::wcslen(delim);

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

std::vector<std::wstring> StringTools::splitString(std::wstring s, const wchar_t* delim, bool removeEmpty)
{
	std::vector<std::wstring> stringArray = std::vector<std::wstring>();

	std::wstring temp = L"";
	std::wstring otherString = L"";

	int dSize = std::wcslen(delim);

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
			if (dSize + i <= s.size())
			{
				//possible that it could still contain
				//the substring

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