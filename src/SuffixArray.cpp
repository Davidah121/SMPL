#include"SuffixArray.h"
#include<algorithm>

struct SufStuff
{
	int index = 0;
	const char* str = nullptr;
};

SuffixArray::SuffixArray(std::string s)
{
	this->s = s;
	buildNaive(s);
}

SuffixArray::~SuffixArray()
{

}

std::vector<int>& SuffixArray::getArray()
{
	return arr;
}

std::vector<int>& SuffixArray::getLCP()
{
	return lcp;
}

std::string& SuffixArray::getString()
{
	return s;
}

int SuffixArray::search(std::string match)
{
	int l = 0;
	int r = match.size()-1;
	int foundIndex = -1;

	while(l <= r)
	{
		int mid = l + (r-l)/2;
		int equal = std::strncmp(match.c_str(), s.c_str()+arr[mid], match.size());

		if(equal == 0)
		{
			foundIndex = mid;
			break;
		}
		
		if(equal < 0)
			r = mid-1;
		else
			l = mid+1;
	}
	return foundIndex;
}

std::vector<int> SuffixArray::searchRange(std::string match)
{
	return {};
}

int cmp(const SufStuff& a1, const SufStuff& a2)
{
	return std::strcmp(a1.str, a2.str) < 0 ? 1 : 0;
}

void SuffixArray::buildNaive(std::string s)
{
	arr.clear();
	std::vector<SufStuff> suffixes;
	for(int i=0; i<s.size(); i++)
	{
		suffixes.push_back(SufStuff{i, s.data()+i});
	}

	std::sort(suffixes.begin(), suffixes.end(), cmp);
	
	for(int i=0; i<suffixes.size(); i++)
	{
		arr.push_back(suffixes[i].index);
	}
}