#pragma once
#include <vector>
#include <string>
#include "BinaryTree.h"
#include "BinarySet.h"

struct HuffmanNode
{
	int frequency = 0;
	int value = 0;
};

class Compression
{
public:
	Compression();
	~Compression();

	static std::vector<unsigned char> compressRLE(std::vector<unsigned char> data);
	static std::vector<unsigned char> compressRLE(unsigned char* data, int size);

	static std::vector<unsigned char> decompressRLE(std::vector<unsigned char> data);
	static std::vector<unsigned char> decompressRLE(unsigned char* data, int size);

	static std::vector<unsigned char> decompressLZW(std::vector<unsigned char> data, int dictionarySize);
	static std::vector<unsigned char> decompressLZW(unsigned char* data, int size, int dictionarySize);

	static std::vector<unsigned char> compressLZW(std::vector<unsigned char> data);
	static std::vector<unsigned char> compressLZW(unsigned char* data, int size);

	static std::vector<unsigned char> compressLZ77(std::vector<unsigned char> data, int maxBufferSize);
	static std::vector<unsigned char> compressLZ77(unsigned char* data, int size, int maxBufferSize);

	static std::vector<unsigned char> decompressLZ77(std::vector<unsigned char> data, int maxBufferSize);
	static std::vector<unsigned char> decompressLZ77(unsigned char* data, int size, int maxBufferSize);

	static std::vector<unsigned char> compressLZSS(std::vector<unsigned char> data, int maxBufferSize);
	static std::vector<unsigned char> compressLZSS(unsigned char* data, int size, int maxBufferSize);

	static std::vector<unsigned char> decompressLZSS(std::vector<unsigned char> data, int maxBufferSize);
	static std::vector<unsigned char> decompressLZSS(unsigned char* data, int size, int maxBufferSize);

	static std::vector<unsigned char> compressHuffman(std::vector<unsigned char> data, BinaryTree<HuffmanNode>* tree);
	static std::vector<unsigned char> compressHuffman(unsigned char* data, int size, BinaryTree<HuffmanNode>* tree);

	static std::vector<unsigned char> decompressHuffman(std::vector<unsigned char> data, int messageSize, BinaryTree<HuffmanNode>* tree);
	static std::vector<unsigned char> decompressHuffman(unsigned char* data, int size, int messageSize, BinaryTree<HuffmanNode>* tree);

	static std::vector<unsigned char> compressDeflate(std::vector<unsigned char> data);
	static std::vector<unsigned char> compressDeflate(unsigned char* data, int size);

	static std::vector<unsigned char> decompressDeflate(std::vector<unsigned char> data);
	static std::vector<unsigned char> decompressDeflate(unsigned char* data, int size);

private:
	static BinaryTree<HuffmanNode>* buildHuffmanTree(unsigned char* data, int size);
	static void fillHuffmanTable(BinaryTreeNode<HuffmanNode>* treeNode, unsigned int* table, int length, int code);

	static BinaryTree<HuffmanNode>* buildDeflateDefaultTree();
	static BinaryTree<HuffmanNode>** buildDynamicDeflateTree(BinarySet* data, int* currentLocation);

	static void getCopyLengthInformation(int code, int* baseValue, int* extraBits);
	static void getBackDistanceInformation(int code, int* baseValue, int* extraBits);


	static BinaryTree<HuffmanNode>* buildCanonicalHuffmanTree(int* dataValue, int* codeLength, int size, bool reversed);
	
};

