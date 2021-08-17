#pragma once
#include <vector>
#include <string>
#include "BinaryTree.h"
#include "BinarySet.h"
#include "FrequencyTable.h"

#include "GeneralExceptions.h"

namespace glib
{

	struct HuffmanNode
	{
		int frequency = 0;
		int value = 0;
	};

	struct HuffmanCodeData
	{
		int codeLength = 0;
		int codeVal = 0;
		int value = 0;
	};

	struct lengthPair
	{
		bool literal;
		int v1;	//if literal, this is the value else it is length
		int v2;	//backwards distance if not literal; otherwise 0
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

		struct LZW_ERROR_L317 : public std::exception
		{
			const char* what() noexcept { return "Error Code L317 when decompressing LZW data."; }
		};

		struct LZW_ERROR_L340 : public std::exception
		{
			const char* what() noexcept { return "Error Code L340 when decompressing LZW data."; }
		};

		static std::vector<unsigned char> decompressLZW(std::vector<unsigned char> data, int dictionarySize);
		static std::vector<unsigned char> decompressLZW(unsigned char* data, int size, int dictionarySize);

		static std::vector<unsigned char> compressLZW(std::vector<unsigned char> data, int* codeSizePointer = nullptr);
		static std::vector<unsigned char> compressLZW(unsigned char* data, int size, int* codeSizePointer = nullptr);

		static std::vector<unsigned char> compressLZ77(std::vector<unsigned char> data, int maxBufferSize);
		static std::vector<unsigned char> compressLZ77(unsigned char* data, int size, int maxBufferSize);

		static std::vector<unsigned char> decompressLZ77(std::vector<unsigned char> data, int maxBufferSize);
		static std::vector<unsigned char> decompressLZ77(unsigned char* data, int size, int maxBufferSize);

		struct LZSS_ERROR : public std::exception
		{
			const char* what() noexcept { return "Error when decompressing LZSS data."; }
		};

		static std::vector<unsigned char> compressLZSS(std::vector<unsigned char> data, int maxBufferSize);
		static std::vector<unsigned char> compressLZSS(unsigned char* data, int size, int maxBufferSize);

		static std::vector<unsigned char> decompressLZSS(std::vector<unsigned char> data, int maxBufferSize);
		static std::vector<unsigned char> decompressLZSS(unsigned char* data, int size, int maxBufferSize);

		struct HUFFMAN_TREE_ERROR : public std::exception
		{
			const char* what() noexcept { return "Invalid Huffman tree used."; }
		};

		static std::vector<unsigned char> compressHuffman(std::vector<unsigned char> data, BinaryTree<HuffmanNode>* tree);
		static std::vector<unsigned char> compressHuffman(unsigned char* data, int size, BinaryTree<HuffmanNode>* tree);

		struct HUFFMAN_CANONICAL_ERROR : public std::exception
		{
			const char* what() noexcept { return "Error creating Cannonical Huffman Tree."; }
		};

		template<typename T>
		static BinaryTree<HuffmanNode>* buildHuffmanTree(T* data, int size, int maxCodeLength = -1);

		static BinaryTree<HuffmanNode>* buildCanonicalHuffmanTree(int* dataValue, int sizeOfData, int* codeLength, int sizeOfCodeLengths, bool separateCodes = true, bool reversed = false);

		static std::vector<unsigned char> decompressHuffman(std::vector<unsigned char> data, int messageSize, BinaryTree<HuffmanNode>* tree);
		static std::vector<unsigned char> decompressHuffman(unsigned char* data, int size, int messageSize, BinaryTree<HuffmanNode>* tree);

		static std::vector<unsigned char> compressDeflate(std::vector<unsigned char> data, int blocks, int compressionLevel = 7, bool customTable = false);
		static std::vector<unsigned char> compressDeflate(unsigned char* data, int size, int blocks, int compressionLevel = 7, bool customTable = false);

		struct DEFLATE_INVALID_MODE : public std::exception
		{
			const char* what() noexcept { return "Error when decompressing deflate block. Invalid compression mode found."; }
		};
		
		static std::vector<unsigned char> decompressDeflate(std::vector<unsigned char> data);
		static std::vector<unsigned char> decompressDeflate(unsigned char* data, int size);

		static double compressArithmetic(std::vector<unsigned char> data, std::vector<double>& percentages);
		static std::vector<unsigned char> decompressArithmetic(double data, int messageSize, std::vector<double> percentages);

		static unsigned int adler32(std::vector<unsigned char> data);
		static unsigned int adler32(unsigned char* data, int size);

		static const unsigned char CRC_8 = 0;
		static const unsigned char CRC_16 = 1;
		static const unsigned char CRC_32 = 2;
		
		static unsigned int crc(std::vector<unsigned char> data, unsigned char type = CRC_32);
		static unsigned int crc(unsigned char* data, int size, unsigned char type = CRC_32);

	private:

		static BinaryTree<HuffmanNode>* buildHuffmanTreeSubFunc(FrequencyTable<int>* freqTable);
		static BinaryTree<HuffmanNode>* buildLimitedHuffmanTreeSubFunc(FrequencyTable<int>* freqTable, int maxCodeLength);
		
		static void fillHuffmanTable(BinaryTreeNode<HuffmanNode>* treeNode, unsigned int* table, int length, int code);

		static BinaryTree<HuffmanNode>* buildDeflateDefaultTree();
		static BinaryTree<HuffmanNode>** buildDynamicDeflateTree(BinarySet* data, int* currentLocation);

		static void getCopyLengthInformation(int code, int* baseValue, int* extraBits);
		static void getBackDistanceInformation(int code, int* baseValue, int* extraBits);

		static void compressDeflateSubFunction(unsigned char* data, int size, std::vector<lengthPair>* outputData, int compressionLevel = 7);
		static void compressDeflateSubFunction2(std::vector<lengthPair>* block, BinarySet* output, bool dynamic, bool lastBlock);
		
		static void buildCanonicalHuffTreeFromHuffTreeSubFunc(BinaryTreeNode<HuffmanNode>* tree);

	};

	template<typename T>
	BinaryTree<HuffmanNode>* Compression::buildHuffmanTree(T* data, int size, int maxCodeLength)
	{
		if(size <= 0)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidSizeError();
			#endif
			return nullptr;
		}

		if(data == nullptr)
		{
			#ifdef USE_EXCEPTIONS
			throw InvalidDataError();
			#endif
			return nullptr;
		}

		//First pass is to fill the frequency table
		time_t t1, t2;
		FrequencyTable<int> freqTable = FrequencyTable<int>();
		for (int i = 0; i < size; i++)
		{
			freqTable.add((int)data[i]);
		}

		if(maxCodeLength<=0)
		{
			BinaryTree<HuffmanNode>* retVal = buildHuffmanTreeSubFunc(&freqTable);
			return retVal;
		}
		else
		{
			BinaryTree<HuffmanNode>* retVal = buildLimitedHuffmanTreeSubFunc(&freqTable, maxCodeLength);
			return retVal;
		}
	}

}  //NAMESPACE glib END