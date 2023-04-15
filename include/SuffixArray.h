#pragma once
#include<vector>
#include<iostream>
#include<cstring>
#include<cstdio>

class SuffixArray
{
public:
	SuffixArray(std::string s);
	~SuffixArray();

	int search(std::string s);
	std::vector<int> searchRange(std::string s);

	std::vector<int>& getArray();
	std::vector<int>& getLCP();
	std::string& getString();
private:
	void buildNaive(std::string s);
	std::vector<int> arr;
	std::vector<int> lcp;
	std::string s;
};