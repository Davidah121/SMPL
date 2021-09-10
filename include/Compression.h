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

		struct ExceededExpectedSizeError : public std::exception
		{
			const char* what() noexcept { return "Error while decompressing data. Exceeded expected size."; }
		};
		/**
		 * @brief Compresses data using Run Length Encoding.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 
		 * 		Data is compressed as bytes where the run is specified first, then the data is specified after.
		 * 		Run is a byte in size and the data is a byte in size.
		 * @param data
		 * 		The data to compress
		 * @param size
		 * 		The size of the data.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the compressed data as a vector so that it can be saved into a file later if desired.
		 */
		static std::vector<unsigned char> compressRLE(unsigned char* data, int size);

		/**
		 * @brief Compresses data using Run Length Encoding.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 
		 * 		Data is compressed as bytes where the run is specified first, then the data is specified after.
		 * 		Run is a byte in size and the data is a byte in size.
		 * @param data
		 * 		The data to compress
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the compressed data as a vector so that it can be saved into a file later if desired.
		 */
		static std::vector<unsigned char> compressRLE(std::vector<unsigned char> data);

		/**
		 * @brief Decompresses Run Length Encoded data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		If the output exceeds the expected size and USE_EXCEPTIONS is defined, an ExceededExpectedSizeError is thrown.
		 * 			If USE_EXCEPTIONS is not defined, the current decompressed data is returned as is.
		 * 
		 * 		The data is expected to have both run and the data as bytes.
		 * 		Run is expected to be first and the data second.
		 * @param data
		 * 		The data to decompress
		 * @param size
		 * 		The size of the data.
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data as a vector.
		 */
		static std::vector<unsigned char> decompressRLE(unsigned char* data, int size, size_t expectedSize = -1);

		/**
		 * @brief Decompresses Run Length Encoded data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the output exceeds the expected size and USE_EXCEPTIONS is defined, an ExceededExpectedSizeError is thrown.
		 * 			If USE_EXCEPTIONS is not defined, the current decompressed data is returned as is.
		 * 
		 * 		The data is expected to have both run and the data as bytes.
		 * 		Run is expected to be first and the data second.
		 * @param data
		 * 		The data to decompress
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data as a vector.
		 */
		static std::vector<unsigned char> decompressRLE(std::vector<unsigned char> data, size_t expectedSize = -1);

		struct LZW_ERROR_L317 : public std::exception
		{
			const char* what() noexcept { return "Error Code L317 when decompressing LZW data."; }
		};

		struct LZW_ERROR_L340 : public std::exception
		{
			const char* what() noexcept { return "Error Code L340 when decompressing LZW data."; }
		};

		/**
		 * @brief Decompresses LZW (Lempel Ziv Welch) data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		If the output exceeds the expected size and USE_EXCEPTIONS is defined, an ExceededExpectedSizeError is thrown.
		 * 			If USE_EXCEPTIONS is not defined, the current decompressed data is returned as is.
		 * @param data
		 * 		The data to decompress
		 * @param size
		 * 		The size of the data.
		 * @param dictionarySize
		 * 		The size of the dictionary used to compress the data. Required to decompress.
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data as a vector. The decompressed data will just be the indicies in the dictionary.
		 * 		The original dictionary is still needed to fully decompress the data.
		 */
		static std::vector<unsigned char> decompressLZW(unsigned char* data, int size, int dictionarySize, size_t expectedSize = -1);

		/**
		 * @brief Decompresses LZW (Lempel Ziv Welch) data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the output exceeds the expected size and USE_EXCEPTIONS is defined, an ExceededExpectedSizeError is thrown.
		 * 			If USE_EXCEPTIONS is not defined, the current decompressed data is returned as is.
		 * @param data
		 * 		The data to decompress
		 * @param dictionarySize
		 * 		The size of the dictionary used to compress the data. Required to decompress.
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data as a vector. The decompressed data will just be the indicies in the dictionary.
		 * 		The original dictionary is still needed to fully decompress the data.
		 */
		static std::vector<unsigned char> decompressLZW(std::vector<unsigned char> data, int dictionarySize, size_t expectedSize = -1);


		/**
		 * @brief Compresses LZW (Lempel Ziv Welch) data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * @param data
		 * 		The data to compress
		 * @param size
		 * 		The size of the data.
		 * @param codeSizePointer
		 * 		A pointer to the code size parameter that should be used to compress the data.
		 * 		If left as a nullptr, the code size paramter will be determined by the function.
		 * 		If the code size is less that or equal to 0, the function will determine the code size and modify the pointer to reflect that.
		 * 		Default is nullptr.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the compressed data as a vector.
		 */
		static std::vector<unsigned char> compressLZW(unsigned char* data, int size, int* codeSizePointer = nullptr);

		/**
		 * @brief Compresses LZW (Lempel Ziv Welch) data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * @param data
		 * 		The data to compress
		 * @param codeSizePointer
		 * 		A pointer to the code size parameter that should be used to compress the data.
		 * 		If left as a nullptr, the code size paramter will be determined by the function.
		 * 		If the code size is less that or equal to 0, the function will determine the code size and modify the pointer to reflect that.
		 * 		Default is nullptr.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the compressed data as a vector.
		 */
		static std::vector<unsigned char> compressLZW(std::vector<unsigned char> data, int* codeSizePointer = nullptr);

		/**
		 * @brief Compresses LZ77 (Lempel Ziv 77) data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 
		 * 		The output uses triplets with the format
		 * 			(back_distance, copy_length, literal)
		 * 		The maximum buffer size also specifies how many bytes are used for the back_distance and copy_length in each distance by using
		 * 		the log base 2 of the maxBufferSize.
		 * @param data
		 * 		The data to compress
		 * @param size
		 * 		The size of the data.
		 * @param maxBufferSize
		 * 		The maximum allowed backwards distance allowed for compression in the sliding window.
		 * 		Default is 32767
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the compressed data as a vector.
		 */
		static std::vector<unsigned char> compressLZ77(unsigned char* data, int size, int maxBufferSize = 0x7FFF);

		/**
		 * @brief Compresses LZ77 (Lempel Ziv 77) data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 
		 * 		The output uses triplets with the format
		 * 			(back_distance, copy_length, literal)
		 * 		The maximum buffer size also specifies how many bytes are used for the back_distance and copy_length in each distance by using
		 * 		the log base 2 of the maxBufferSize.
		 * @param data
		 * 		The data to compress
		 * @param maxBufferSize
		 * 		The maximum allowed backwards distance allowed for compression in the sliding window.
		 * 		Default is 32767
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the compressed data as a vector.
		 */
		static std::vector<unsigned char> compressLZ77(std::vector<unsigned char> data, int maxBufferSize = 0x7FFF);

		/**
		 * @brief Decompresses LZ77 (Lempel Ziv 77) data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		If the output exceeds the expected size and USE_EXCEPTIONS is defined, an ExceededExpectedSizeError is thrown.
		 * 			If USE_EXCEPTIONS is not defined, the current decompressed data is returned as is.
		 * 
		 * 		The input is expected to use triplets with the format
		 * 			(back_distance, copy_length, literal)
		 * 		The maximum buffer size also specifies how many bytes are used for the back_distance and copy_length in each distance by using
		 * 		the log base 2 of the maxBufferSize.
		 * @param data
		 * 		The data to decompress
		 * @param size
		 * 		The size of the data.
		 * @param maxBufferSize
		 * 		The maximum allowed backwards distance allowed for compression in the sliding window.
		 * 		Default is 32767.
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data as a vector.
		 */
		static std::vector<unsigned char> decompressLZ77(unsigned char* data, int size, int maxBufferSize = 0x7FFF, size_t expectedSize = -1);

		/**
		 * @brief Decompresses LZ77 (Lempel Ziv 77) data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the output exceeds the expected size and USE_EXCEPTIONS is defined, an ExceededExpectedSizeError is thrown.
		 * 			If USE_EXCEPTIONS is not defined, the current decompressed data is returned as is.
		 * 
		 * 		The input is expected to use triplets with the format
		 * 			(back_distance, copy_length, literal)
		 * 		The maximum buffer size also specifies how many bytes are used for the back_distance and copy_length in each distance by using
		 * 		the log base 2 of the maxBufferSize.
		 * @param data
		 * 		The data to decompress
		 * @param maxBufferSize
		 * 		The maximum allowed backwards distance allowed for compression in the sliding window.
		 * 		Default is 32767.
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data as a vector.
		 */
		static std::vector<unsigned char> decompressLZ77(std::vector<unsigned char> data, int maxBufferSize = 0x7FFF, size_t expectedSize = -1);

		struct LZSS_ERROR : public std::exception
		{
			const char* what() noexcept { return "Error when decompressing LZSS data."; }
		};

		/**
		 * @brief Compresses data using LZSS (Lempel Ziv Storer Szymanski)
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 
		 * 		The output uses 9 bits for a literal or 24 bits for a reference pair
		 * 		The first bit specifies if it is a literal or not.
		 * 		If literal, the next 8 bits are the value.
		 * 		In a referencePair, the backwards distance is 15 bits and the copy length is 8 bits.
		 * @param data
		 * 		The data to compress.
		 * @param size
		 * 		The size of the data.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the compressed data as a vector.
		 */
		static std::vector<unsigned char> compressLZSS(unsigned char* data, int size);

		/**
		 * @brief Compresses data using LZSS (Lempel Ziv Storer Szymanski)
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 
		 * 		The output uses 9 bits for a literal or 24 bits for a reference pair
		 * 		The first bit specifies if it is a literal or not.
		 * 		If literal, the next 8 bits are the value.
		 * 		In a referencePair, the backwards distance is 15 bits and the copy length is 8 bits.
		 * @param data
		 * 		The data to compress
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the compressed data as a vector.
		 */
		static std::vector<unsigned char> compressLZSS(std::vector<unsigned char> data);
		
		/**
		 * @brief Decompresses LZSS (Lempel Ziv Storer Szymanski) data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		If the output exceeds the expected size and USE_EXCEPTIONS is defined, an ExceededExpectedSizeError is thrown.
		 * 			If USE_EXCEPTIONS is not defined, the current decompressed data is returned as is.
		 * 
		 * 		The input expects the format to be the same as the compression methods.
		 * 		The first bit specifies if it is a literal or not.
		 * 		If literal, the next 8 bits are the value.
		 * 		In a referencePair, the backwards distance is 15 bits and the copy length is 8 bits.
		 * @param data
		 * 		The data to compress.
		 * @param size
		 * 		The size of the data.
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data as a vector.
		 */
		static std::vector<unsigned char> decompressLZSS(unsigned char* data, int size, size_t expectedSize = -1);

		/**
		 * @brief Decompresses LZSS (Lempel Ziv Storer Szymanski) data.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the output exceeds the expected size and USE_EXCEPTIONS is defined, an ExceededExpectedSizeError is thrown.
		 * 			If USE_EXCEPTIONS is not defined, the current decompressed data is returned as is.
		 * 
		 * 		The input expects the format to be the same as the compression methods.
		 * 		The first bit specifies if it is a literal or not.
		 * 		If literal, the next 8 bits are the value.
		 * 		In a referencePair, the backwards distance is 15 bits and the copy length is 8 bits.
		 * @param data
		 * 		The data to compress.
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data as a vector.
		 */
		static std::vector<unsigned char> decompressLZSS(std::vector<unsigned char> data, size_t expectedSize = -1);

		struct HUFFMAN_TREE_ERROR : public std::exception
		{
			const char* what() noexcept { return "Invalid Huffman tree used."; }
		};

		/**
		 * @brief Compresses data using a Huffman tree.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * @param data
		 * 		The data to decompress
		 * @param size
		 * 		The size of the data.
		 * @param tree
		 * 		The pointer that will store the huffman tree generated.
		 * 		Must be a valid pointer and should not have any data stored in it.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the compressed data as a vector and the tree pointer should have the
		 * 		huffman tree used to compress the data.
		 */
		static std::vector<unsigned char> compressHuffman(unsigned char* data, int size, BinaryTree<HuffmanNode>* tree);

		/**
		 * @brief Compresses data using a Huffman tree.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * @param data
		 * 		The data to decompress
		 * @param tree
		 * 		The pointer that will store the huffman tree generated.
		 * 		Must be a valid pointer and should not have any data stored in it.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the compressed data as a vector and the tree pointer should have the
		 * 		huffman tree used to compress the data.
		 */
		static std::vector<unsigned char> compressHuffman(std::vector<unsigned char> data, BinaryTree<HuffmanNode>* tree);

		/**
		 * @brief Decompresses data using Huffman tree encoding.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		If the output exceeds the expected size and USE_EXCEPTIONS is defined, an ExceededExpectedSizeError is thrown.
		 * 			If USE_EXCEPTIONS is not defined, the current decompressed data is returned as is.
		 * 		If the tree is invalid for any reason such as being null or not being able to resolve data and USE_EXCEPTIONS
		 * 		is defined, a HUFFMAN_TREE_ERROR is thrown.
		 * 
		 * @param data
		 * 		The data to decompress
		 * @param size
		 * 		The size of the data.
		 * @param tree
		 * 		The huffman tree to use for decompression
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data as a vector.
		 */
		static std::vector<unsigned char> decompressHuffman(unsigned char* data, int size, BinaryTree<HuffmanNode>* tree, size_t expectedSize = -1);

		/**
		 * @brief Decompresses data using Huffman tree encoding.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the output exceeds the expected size and USE_EXCEPTIONS is defined, an ExceededExpectedSizeError is thrown.
		 * 			If USE_EXCEPTIONS is not defined, the current decompressed data is returned as is.
		 * 		If the tree is invalid for any reason such as being null or not being able to resolve data and USE_EXCEPTIONS
		 * 		is defined, a HUFFMAN_TREE_ERROR is thrown.
		 * 
		 * @param data
		 * 		The data to decompress
		 * @param tree
		 * 		The huffman tree to use for decompression
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data as a vector.
		 */
		static std::vector<unsigned char> decompressHuffman(std::vector<unsigned char> data, BinaryTree<HuffmanNode>* tree, size_t expectedSize = -1);

		struct HUFFMAN_CANONICAL_ERROR : public std::exception
		{
			const char* what() noexcept { return "Error creating Cannonical Huffman Tree."; }
		};

		/**
		 * @brief Creates a huffman tree from the specified data array.
		 * 		A length limited huffman tree can be created this way as well if desired.
		 * 		If the size of data is 0 or less and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		
		 * @param data
		 * 		The data to create a huffman tree from. It should be a numerical data type as
		 * 		the data will be cast to ints to find the tree.
		 * @param size
		 * 		The size of the data.
		 * @param maxCodeLength
		 * 		The maximum code length allowed for the tree.
		 * 		A value less than 0 will create a huffman tree that is not length limited.
		 * @return BinaryTree<HuffmanNode>
		 * 		Returns a valid huffman tree if successful. Otherwise, returns a nullptr.
		 */
		template<typename T>
		static BinaryTree<HuffmanNode>* buildHuffmanTree(T* data, int size, int maxCodeLength = -1);

		/**
		 * @brief Create a Canonical Huffman Tree based on the parameters given.
		 * 		The data can be given where each value in the dataValue array has a unique code length given in the codeLength array
		 * 		at the same spot or they can be separate where the codeLengths array tells how many codes have length specified by its
		 * 		index. Both representations are used in different data formats.
		 * 		
		 * 		If the size of data is 0 or less or there is not enough data to map code lengths to the data,
		 * 		and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		
		 * @param dataValue
		 * 		The data values used.
		 * @param sizeOfData
		 * 		The size of the dataValue array.
		 * @param codeLengths
		 * 		The code lengths for the data.
		 * @param sizeOfCodeLengths
		 * 		The size of the codeLength array.
		 * @param separateCodes
		 * 		Whether or not the code lengths have a one to one mapping.
		 * 		If false, codeLength[i] must specify the length of dataValue[i] and the sizes must match.
		 * 		Default value is true
		 * @param rmsb
		 * 		Determines whether the code values will be in LMSB or RMSB order.
		 * 		Default is false | which is LMSB
		 */
		static BinaryTree<HuffmanNode>* buildCanonicalHuffmanTree(int* dataValue, int sizeOfData, int* codeLength, int sizeOfCodeLengths, bool separateCodes = true, bool rmsb = false);

		/**
		 * @brief Compresses data using DELFATE
		 * 
		 * 		If the size of data is 0 or blocks is 0 and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		
		 * 		This method will use multiple threads to compress to maintain reasonable speed unless the amount of blocks is 1.
		 * 		Custom huffman trees are supported.
		 * 		Uses a zlib header.
		 * @param data
		 * 		The data to decompress
		 * @param size
		 * 		The size of the data.
		 * @param blocks
		 * 		The amount of blocks to separate the data into for compression.
		 * 		Adding more blocks allows multiple threads to compress them individually but can reduce the compression ratio.
		 * @param compressionLevel
		 * 		Changing the compression level adjusts the maximum backwards distance allowed.
		 * 		The lowest backwards distance is 256 where compressionLevel is set to 0
		 * 		The maximum distance is 32768 where compressionLevel is set to 7
		 * 		Default value is 7
		 * @param customTable
		 * 		Chooses whether to use a custom huffman table for compression.
		 * 		Slower but results in a unique table for each block and better compression.
		 * 		Default value is false
		 * @return std::vector<unsigned char>
		 * 		If successful, returns a valid deflate stream using a zlib header as a std::vector<unsigned char>.
		 * 		Otherwise, returns an empty vector.
		 */
		static std::vector<unsigned char> compressDeflate(unsigned char* data, int size, int blocks, int compressionLevel = 7, bool customTable = false);

		/**
		 * @brief Compresses data using DELFATE
		 * 
		 * 		If the size of data is 0 or blocks is 0 and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		
		 * 		This method will use multiple threads to compress to maintain reasonable speed unless the amount of blocks is 1.
		 * 		Custom huffman trees are supported.
		 * 		Uses a zlib header.
		 * @param data
		 * 		The data to decompress
		 * @param blocks
		 * 		The amount of blocks to separate the data into for compression.
		 * 		Adding more blocks allows multiple threads to compress them individually but can reduce the compression ratio.
		 * @param compressionLevel
		 * 		Changing the compression level adjusts the maximum backwards distance allowed.
		 * 		The lowest backwards distance is 256 where compressionLevel is set to 0
		 * 		The maximum distance is 32768 where compressionLevel is set to 7
		 * 		Default value is 7
		 * @param customTable
		 * 		Chooses whether to use a custom huffman table for compression.
		 * 		Slower but results in a unique table for each block and better compression.
		 * 		Default value is false
		 * @return std::vector<unsigned char>
		 * 		If successful, returns a valid deflate stream using a zlib header as a std::vector<unsigned char>.
		 * 		Otherwise, returns an empty vector.
		 */
		static std::vector<unsigned char> compressDeflate(std::vector<unsigned char> data, int blocks, int compressionLevel = 7, bool customTable = false);

		struct DEFLATE_INVALID_MODE : public std::exception
		{
			const char* what() noexcept { return "Error when decompressing deflate block. Invalid compression mode found."; }
		};
		
		/**
		 * @brief Decompresses a DEFLATE data stream.
		 * 
		 * 		If the size of data is 0 or blocks is 0 and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		
		 * 		Assumes that the data uses a ZLIB header. GZIP is not supported.
		 * @param data
		 * 		The data to decompress
		 * @param size
		 * 		The size of the data.
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data. Otherwise, an empty vector is returned.
		 */
		static std::vector<unsigned char> decompressDeflate(unsigned char* data, int size, size_t expectedSize = -1);

		/**
		 * @brief Decompresses a DEFLATE data stream.
		 * 
		 * 		If the size of data is 0 or blocks is 0 and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		
		 * 		Assumes that the data uses a ZLIB header. GZIP is not supported.
		 * @param data
		 * 		The data to decompress
		 * @param size
		 * 		The size of the data.
		 * @param expectedSize
		 * 		The expected size of the decompressed data. Set this if you know what the data size should be when decompressed.
		 * 		By default, it is set to the max that a vector can store.
		 * @return std::vector<unsigned char>
		 * 		If successful, returns the decompressed data. Otherwise, an empty vector is returned.
		 */
		static std::vector<unsigned char> decompressDeflate(std::vector<unsigned char> data, size_t expectedSize = -1);

		/**
		 * @brief Experimental Arithmetic compression.
		 * 		Should not be used currently.
		 */
		static double compressArithmetic(std::vector<unsigned char> data, std::vector<double>& percentages);

		/**
		 * @brief Experimental Arithmetic decompression.
		 * 		Should not be used currently.
		 */
		static std::vector<unsigned char> decompressArithmetic(double data, int messageSize, std::vector<double> percentages);

		/**
		 * @brief Generates a checksum using ADLER32.
		 * 		If the size of data is 0 and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * @param data
		 * 		The data to generate the checksum from
		 * @param size
		 * 		The size of the data.
		 * @return unsigned int
		 * 		Returns a valid checksum if successful. Otherwise, 0 is returned.
		 * 		Note that 0 can be a valid checksum return.
		 */
		static unsigned int adler32(unsigned char* data, int size);

		/**
		 * @brief Generates a checksum using ADLER32.
		 * 		If the size of data is 0 and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * @param data
		 * 		The data to generate the checksum from
		 * @return unsigned int
		 * 		Returns a valid checksum if successful. Otherwise, 0 is returned.
		 * 		Note that 0 can be a valid checksum return.
		 */
		static unsigned int adler32(std::vector<unsigned char> data);

		static const unsigned char CRC_8 = 0;
		static const unsigned char CRC_16 = 1;
		static const unsigned char CRC_32 = 2;
		
		/**
		 * @brief Generates a checksum using CRC.
		 * 		If the size of data is 0 and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		Three CRC methods are available:
		 * 			CRC_8
		 * 			CRC_16
		 * 			CRC_32
		 * @param data
		 * 		The data to generate the checksum from
		 * @param size
		 * 		The size of the data.
		 * @param type
		 * 		The type of CRC to use.
		 * 		DEFAULT is CRC_32
		 * @return unsigned int
		 * 		Returns a valid checksum if successful. Otherwise, 0 is returned.
		 * 		Note that 0 can be a valid checksum return.
		 */
		static unsigned int crc(unsigned char* data, int size, unsigned char type = CRC_32);

		/**
		 * @brief Generates a checksum using CRC.
		 * 		If the size of data is 0 and USE_EXCEPTIONS is defined, an InvalidSizeError is thrown.
		 * 		If the data pointer is nullptr and USE_EXCEPTIONS is defined, an InvalidDataError is thrown.
		 * 		Three CRC methods are available:
		 * 			CRC_8
		 * 			CRC_16
		 * 			CRC_32
		 * @param data
		 * 		The data to generate the checksum from
		 * @param size
		 * 		The size of the data.
		 * @param type
		 * 		The type of CRC to use.
		 * 		DEFAULT is CRC_32
		 * @return unsigned int
		 * 		Returns a valid checksum if successful. Otherwise, 0 is returned.
		 * 		Note that 0 can be a valid checksum return.
		 */
		static unsigned int crc(std::vector<unsigned char> data, unsigned char type = CRC_32);

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